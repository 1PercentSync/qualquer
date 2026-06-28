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
#include <qualquer/renderer/test_kernel.h>
#include <qualquer/vulkan/context.h>
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
        // dependency), so they are passed to the pipeline as opaque values.
        pipeline_.init(cuda_context.device_context,
                       optixir_path,
                       sizeof(LaunchParams),
                       "params");

        // One SBT record per program group, header-only. Packing reuses one host
        // record since only the opaque header (written by the API) differs between
        // groups; the record body carries no user data.
        SbtRecord record{};
        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.raygen_program, &record));
        sbt_raygen_.alloc(1);
        sbt_raygen_.upload(&record, 1, cuda_context.stream);

        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.miss_program, &record));
        sbt_miss_.alloc(1);
        sbt_miss_.upload(&record, 1, cuda_context.stream);

        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.hitgroup_program, &record));
        sbt_hit_.alloc(1);
        sbt_hit_.upload(&record, 1, cuda_context.stream);

        // Two accumulation buffers for ping-pong HDR accumulation, cleared so the
        // first frame reads a defined zero background before any write.
        const std::size_t pixel_count = static_cast<std::size_t>(width) * height;
        for (auto &buffer : accum_buffers_) {
            buffer.alloc(pixel_count);
            buffer.clear(cuda_context.stream);
        }

        params_buffer_.alloc(1);
        accum_index_ = 0;

        spdlog::info("Renderer initialized ({}x{}, {} SBT records)",
                     width,
                     height,
                     3);
    }

    void Renderer::resize(const optix::Context &cuda_context,
                          const uint32_t width,
                          const uint32_t height) {
        // A size change invalidates prior HDR accumulation, so both buffers are
        // reallocated to the new pixel count and cleared, and the ping-pong index
        // restarts from zero. Pipeline, SBT records, and the params buffer are
        // resolution-independent and stay.
        const std::size_t pixel_count = static_cast<std::size_t>(width) * height;
        for (auto &buffer : accum_buffers_) {
            buffer.resize(pixel_count);
            buffer.clear(cuda_context.stream);
        }
        accum_index_ = 0;

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
    }

    void Renderer::submit_cuda(const optix::Context &cuda_context,
                               const uint32_t width,
                               const uint32_t height,
                               const uint32_t frame_index) {
        launch_test_kernel(cuda_context.display_surface,
                           width,
                           height,
                           frame_counter_,
                           cuda_context.stream);

        // Signal the frame's external semaphore on the same stream as the kernel, so
        // the signal is posted after the kernel completes (stream submission order).
        // Binary OPAQUE_WIN32 needs no fence value — params stays zeroed.
        cudaExternalSemaphore_t sem = cuda_context.external_semaphores[frame_index];
        cudaExternalSemaphoreSignalParams params{};
        CUDA_CHECK(cudaSignalExternalSemaphoresAsync(&sem, &params, 1, cuda_context.stream));

        ++frame_counter_;
    }

    void Renderer::record_vulkan(const RenderInput &input) {
        // ReSharper disable once CppLocalVariableMayBeConst
        VkCommandBuffer cmd = input.cmd;
        // ReSharper disable once CppLocalVariableMayBeConst
        VkImage swapchain_image = input.swapchain.images[input.image_index];
        const VkExtent2D extent = input.swapchain.extent;

        // --- Layout transitions before the blit ---
        // Display buffer: UNDEFINED -> TRANSFER_SRC_OPTIMAL. CUDA just wrote it; the
        // external semaphore wait in end_frame made those writes visible, so the src
        // stage is NONE (no prior Vulkan stage produced this content).
        const VkImageMemoryBarrier2 display_to_src{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
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

        const VkImageMemoryBarrier2 pre_blit_barriers[2]{display_to_src, swapchain_to_dst};
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

        // --- Swapchain: TRANSFER_DST_OPTIMAL -> COLOR_ATTACHMENT_OPTIMAL for ImGui ---
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

        const VkDependencyInfo to_attachment_dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &to_attachment,
        };
        vkCmdPipelineBarrier2(cmd, &to_attachment_dep);

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
