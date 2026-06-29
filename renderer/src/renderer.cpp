/**
 * @file renderer.cpp
 * @brief Renderer implementation.
 */

#include <qualquer/renderer/renderer.h>

#include <cuda_runtime.h>

#include <vulkan/vulkan.h>

#include <qualquer/optix/context.h>
#include <qualquer/optix/cuda_check.h>
#include <qualquer/optix/optix_check.h>
#include <qualquer/optix/pipeline.h>
#include <qualquer/renderer/imgui_backend.h>
#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/tonemap.h>
#include <qualquer/vulkan/interop.h>
#include <qualquer/vulkan/swapchain.h>

#include <optix_stubs.h>
#include <spdlog/spdlog.h>

namespace qualquer::renderer {
    void Renderer::init(const optix::Context &cuda_context,
                        const uint32_t width,
                        const uint32_t height,
                        const std::string &optixir_path) {
        // The launch-params constant name ("params") and its size are device-side
        // facts this layer cannot take from the renderer header (single-direction
        // dependency).
        pipeline_.init(cuda_context.device_context,
                       optixir_path,
                       sizeof(LaunchParams),
                       "params");

        // One SBT record per program group, header-only. Packing reuses one host
        // record since only the opaque header (written by the API) differs between
        // groups.
        SbtRecord record{};
        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.raygen_program, &record));
        sbt_raygen_.alloc(1);
        sbt_raygen_.upload(&record, 1, cuda_context.compute_stream);

        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.miss_program, &record));
        sbt_miss_.alloc(1);
        sbt_miss_.upload(&record, 1, cuda_context.compute_stream);

        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.hitgroup_program, &record));
        sbt_hit_.alloc(1);
        sbt_hit_.upload(&record, 1, cuda_context.compute_stream);

        // Two accumulation buffers for ping-pong HDR accumulation, cleared so the
        // first frame reads a defined zero background.
        const std::size_t pixel_count = static_cast<std::size_t>(width) * height;
        for (auto &buffer : accum_buffers_) {
            buffer.alloc(pixel_count);
            buffer.clear(cuda_context.compute_stream);
        }

        params_buffer_.alloc(1);
        accum_index_ = 0;

        // Events start recorded on compute_stream so the first frame's waits
        // (on slot 1, the "previous" slot) pass immediately — the stream ordering
        // guarantees the records complete after the buffer clears above.
        for (auto &event : event_raygen_done_) {
            CUDA_CHECK(cudaEventCreateWithFlags(&event, cudaEventDisableTiming));
            CUDA_CHECK(cudaEventRecord(event, cuda_context.compute_stream));
        }
        for (auto &event : event_tonemap_done_) {
            CUDA_CHECK(cudaEventCreateWithFlags(&event, cudaEventDisableTiming));
            CUDA_CHECK(cudaEventRecord(event, cuda_context.compute_stream));
        }

        spdlog::info("Renderer initialized ({}x{}, {} SBT records)",
                     width,
                     height,
                     3);
    }

    void Renderer::resize(const optix::Context &cuda_context,
                          const uint32_t width,
                          const uint32_t height) {
        // A size change invalidates prior HDR accumulation. Pipeline, SBT records,
        // and the params buffer are resolution-independent and stay.
        const std::size_t pixel_count = static_cast<std::size_t>(width) * height;
        for (auto &buffer : accum_buffers_) {
            buffer.resize(pixel_count);
            buffer.clear(cuda_context.compute_stream);
        }
        accum_index_ = 0;

        // Re-record events so the next frame's display_stream wait covers the
        // clears above. Without this, the wait would see the stale event from
        // the last pre-resize raygen (recorded before the clears) and start
        // tonemap before the clears finish.
        for (auto &event : event_raygen_done_) {
            CUDA_CHECK(cudaEventRecord(event, cuda_context.compute_stream));
        }
        for (auto &event : event_tonemap_done_) {
            CUDA_CHECK(cudaEventRecord(event, cuda_context.compute_stream));
        }

        spdlog::info("Renderer resized ({}x{})", width, height);
    }

    void Renderer::destroy() {
        // Reverse init creation order: the pipeline references the module and
        // program groups, so it is torn down before the SBT buffers whose device
        // memory it bound. Pipeline::destroy and CudaBuffer::free are both
        // idempotent (null-reset), so a repeat call is a no-op. State members
        // (accum_index_, frame_counter_) are intentionally not reset here —
        // release is the sole responsibility; a subsequent init resets them.
        pipeline_.destroy();
        sbt_raygen_.free();
        sbt_miss_.free();
        sbt_hit_.free();
        for (auto &buffer : accum_buffers_) {
            buffer.free();
        }
        params_buffer_.free();
        for (auto &event : event_raygen_done_) {
            if (event != nullptr) {
                CUDA_CHECK(cudaEventDestroy(event));
                event = nullptr;
            }
        }
        for (auto &event : event_tonemap_done_) {
            if (event != nullptr) {
                CUDA_CHECK(cudaEventDestroy(event));
                event = nullptr;
            }
        }
    }

    void Renderer::submit_cuda(const optix::Context &cuda_context,
                               const uint32_t width,
                               const uint32_t height,
                               const uint32_t frame_index) {
        // Dual-stream overlap: compute_stream runs raygen while display_stream
        // runs tonemap + semaphore signal in parallel. Ping-pong buffers guarantee
        // raygen and tonemap access different buffers within the same frame; CUDA
        // events enforce the cross-frame dependencies (each buffer must finish
        // being written before tonemap reads it, and finish being read before
        // raygen overwrites it).
        const uint32_t slot = frame_counter_ % 2;
        const uint32_t prev_slot = 1 - slot;

        // --- compute_stream: wait prev tonemap → upload + raygen → record ---
        // Wait until the previous frame's tonemap finished reading the buffer
        // that this frame's raygen is about to overwrite.
        CUDA_CHECK(cudaStreamWaitEvent(cuda_context.compute_stream, event_tonemap_done_[prev_slot]));

        const LaunchParams params{
            .accumulation_buffer = accum_buffers_[1 - accum_index_].data(),
            .width = width,
            .height = height,
            .frame_index = frame_counter_,
        };
        params_buffer_.upload(&params, 1, cuda_context.compute_stream);

        const OptixShaderBindingTable sbt{
            .raygenRecord = sbt_raygen_.device_ptr(),
            .exceptionRecord = 0,
            .missRecordBase = sbt_miss_.device_ptr(),
            .missRecordStrideInBytes = sizeof(SbtRecord),
            .missRecordCount = 1,
            .hitgroupRecordBase = sbt_hit_.device_ptr(),
            .hitgroupRecordStrideInBytes = sizeof(SbtRecord),
            .hitgroupRecordCount = 1,
            .callablesRecordBase = 0,
            .callablesRecordStrideInBytes = 0,
            .callablesRecordCount = 0,
        };

        // traversable=0 is valid: raygen does not call optixTrace, so no
        // acceleration structure is traversed.
        OPTIX_CHECK(optixLaunch(pipeline_.handle,
                                cuda_context.compute_stream,
                                params_buffer_.device_ptr(),
                                sizeof(LaunchParams),
                                &sbt,
                                width, height, 1));

        CUDA_CHECK(cudaEventRecord(event_raygen_done_[slot], cuda_context.compute_stream));

        // --- display_stream: wait reverse sem + wait prev raygen → tonemap → record → signal ---
        // Wait for the previous frame's blit to finish reading display_surface
        // before this frame's tonemap overwrites it (write-after-read). A single
        // semaphore suffices: the forward chain (tonemap → forward signal → blit →
        // reverse signal) structurally prevents double-signaling.
        // Enqueued before the raygen event wait: the reverse semaphore typically
        // signals earlier (blit is lighter than raygen), so checking it first lets
        // the GPU scheduler resolve the faster wait and prepare for the next one.
        // ReSharper disable once CppLocalVariableMayBeConst
        cudaExternalSemaphore_t reverse_sem = cuda_context.reverse_external_semaphore;
        constexpr cudaExternalSemaphoreWaitParams reverse_wait_params{};
        CUDA_CHECK(cudaWaitExternalSemaphoresAsync(&reverse_sem, &reverse_wait_params, 1, cuda_context.display_stream));

        // Wait until the previous frame's raygen finished writing the buffer
        // that this frame's tonemap is about to read.
        CUDA_CHECK(cudaStreamWaitEvent(cuda_context.display_stream, event_raygen_done_[prev_slot]));

        launch_tonemap(accum_buffers_[accum_index_].data(),
                       cuda_context.display_surface,
                       width, height,
                       cuda_context.display_stream);

        CUDA_CHECK(cudaEventRecord(event_tonemap_done_[slot], cuda_context.display_stream));

        // Signal after tonemap completes on display_stream. Binary OPAQUE_WIN32
        // needs no fence value — signal_params stays zeroed.
        // ReSharper disable once CppLocalVariableMayBeConst
        cudaExternalSemaphore_t sem = cuda_context.external_semaphores[frame_index];
        constexpr cudaExternalSemaphoreSignalParams signal_params{};
        CUDA_CHECK(cudaSignalExternalSemaphoresAsync(&sem, &signal_params, 1, cuda_context.display_stream));

        accum_index_ = 1 - accum_index_;
        ++frame_counter_;
    }

    void Renderer::record_vulkan(const RenderInput &input) {
        // ReSharper disable once CppLocalVariableMayBeConst
        VkCommandBuffer cmd = input.cmd;
        // ReSharper disable once CppLocalVariableMayBeConst
        VkImage swapchain_image = input.swapchain.images[input.image_index];
        const VkExtent2D extent = input.swapchain.extent;

        // --- Layout transitions before the blit ---
        // Display buffer acquire: CUDA just wrote it via a surface object (external
        // access). The queue family ownership transfer from EXTERNAL makes the CUDA
        // writes visible to Vulkan. GENERAL is the layout compatible with external
        // access; the transition to TRANSFER_SRC_OPTIMAL prepares for the blit read.
        // The external semaphore wait in end_frame provides the execution dependency.
        const VkImageMemoryBarrier2 display_acquire{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL,
            .dstQueueFamilyIndex = input.graphics_queue_family,
            .image = input.display_buffer.image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        // Swapchain: UNDEFINED -> TRANSFER_DST_OPTIMAL. No prior GPU work touched the
        // acquired image (acquire hands it over via the wait semaphore in end_frame),
        // so the src stage is TOP_OF_PIPE.
        const VkImageMemoryBarrier2 swapchain_to_dst{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchain_image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        const VkImageMemoryBarrier2 pre_blit_barriers[2]{display_acquire, swapchain_to_dst};
        // ReSharper disable once CppVariableCanBeMadeConstexpr
        const VkDependencyInfo pre_blit_dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 2,
            .pImageMemoryBarriers = pre_blit_barriers,
        };
        vkCmdPipelineBarrier2(cmd, &pre_blit_dep);

        // --- Blit: display buffer (R8G8B8A8_UNORM) -> swapchain (B8G8R8A8_SRGB) ---
        // Hardware handles UNORM->float, RGBA->BGRA channel swap, and sRGB encoding.
        const VkImageBlit2 region{
            .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .srcSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .srcOffsets = {{0, 0, 0}, {static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), 1}},
            .dstSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .dstOffsets = {{0, 0, 0}, {static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), 1}},
        };

        const VkBlitImageInfo2 blit_info{
            .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .srcImage = input.display_buffer.image,
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage = swapchain_image,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = 1,
            .pRegions = &region,
            .filter = VK_FILTER_LINEAR,
        };
        vkCmdBlitImage2(cmd, &blit_info);

        // --- Post-blit barriers (display buffer release + swapchain transition) ---
        // Display buffer release: hand ownership back to EXTERNAL so the next
        // frame's CUDA tonemap can write it. Transitions back to GENERAL (the
        // layout compatible with external access). The reverse semaphore signal
        // in end_frame provides the execution dependency toward CUDA.
        const VkImageMemoryBarrier2 display_release{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .srcQueueFamilyIndex = input.graphics_queue_family,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL,
            .image = input.display_buffer.image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        // Swapchain: TRANSFER_DST_OPTIMAL -> COLOR_ATTACHMENT_OPTIMAL for ImGui.
        const VkImageMemoryBarrier2 to_attachment{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchain_image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        const VkImageMemoryBarrier2 post_blit_barriers[2]{display_release, to_attachment};
        const VkDependencyInfo post_blit_dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 2,
            .pImageMemoryBarriers = post_blit_barriers,
        };
        vkCmdPipelineBarrier2(cmd, &post_blit_dep);

        // --- ImGui overlay on top of the blitted image (loadOp=LOAD keeps it) ---
        const VkRenderingAttachmentInfo color_attachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = input.swapchain.image_views[input.image_index],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };

        const VkRenderingInfo rendering_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea =
            {
                .offset = {0, 0},
                .extent = extent,
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment,
        };

        vkCmdBeginRendering(cmd, &rendering_info);
        input.imgui.render(cmd);
        vkCmdEndRendering(cmd);

        // --- Swapchain: COLOR_ATTACHMENT_OPTIMAL -> PRESENT_SRC_KHR ---
        const VkImageMemoryBarrier2 to_present{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchain_image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        const VkDependencyInfo to_present_dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &to_present,
        };
        vkCmdPipelineBarrier2(cmd, &to_present_dep);
    }
} // namespace qualquer::renderer
