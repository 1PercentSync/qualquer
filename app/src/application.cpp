/**
 * @file application.cpp
 * @brief Application implementation.
 */

#include <qualquer/app/application.h>

#include <array>
#include <vector>

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace qualquer::app {
    namespace {
        constexpr auto kLogLevel = spdlog::level::info;

        constexpr int kInitialWidth = 1920;
        constexpr int kInitialHeight = 1080;
        constexpr auto kWindowTitle = "Qualquer";
    } // namespace

    void Application::init() {
        spdlog::set_level(kLogLevel);
        spdlog::flush_on(spdlog::level::critical);

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window_ = glfwCreateWindow(kInitialWidth, kInitialHeight, kWindowTitle, nullptr, nullptr);

        // Three-stage init: Vulkan pre-init enumerates presentable devices, CUDA
        // selects among them (presentability-constrained), Vulkan completes on
        // the CUDA-selected device. This binds both layers to the same physical
        // GPU and excludes compute-only devices (e.g. a TCC GPU) up front.
        const std::vector<std::array<std::uint8_t, 16> > presentable_uuids = context_.pre_init(window_);
        cuda_context_.init(presentable_uuids);
        context_.init(cuda_context_.device_uuid);

        swapchain_.init(context_, VK_PRESENT_MODE_MAILBOX_KHR);
        display_buffer_.init(context_,
                             VK_FORMAT_R8G8B8A8_UNORM,
                             swapchain_.extent,
                             VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        for (auto &sem: interop_semaphores_) {
            sem.init(context_);
        }
        cuda_context_.import_display_buffer(display_buffer_.win32_handle,
                                            swapchain_.extent.width,
                                            swapchain_.extent.height,
                                            display_buffer_.size);
        std::array<void *, optix::kMaxFramesInFlight> semaphore_handles{};
        for (uint32_t i = 0; i < optix::kMaxFramesInFlight; ++i) {
            semaphore_handles[i] = interop_semaphores_[i].win32_handle;
        }
        cuda_context_.import_semaphores(semaphore_handles);
        reverse_interop_semaphore_.init(context_);
        cuda_context_.import_reverse_semaphore(reverse_interop_semaphore_.win32_handle);
        // Pre-signal the reverse semaphore so the first frame's CUDA wait
        // passes immediately — there is no prior blit to wait for.
        const VkSemaphoreSubmitInfo presignal_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = reverse_interop_semaphore_.semaphore,
            .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        };
        const VkSubmitInfo2 presignal_submit{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &presignal_info,
        };
        VK_CHECK(vkQueueSubmit2(context_.graphics_queue, 1, &presignal_submit, VK_NULL_HANDLE));
        imgui_backend_.init(context_, swapchain_, window_);
        renderer_.init(cuda_context_,
                       swapchain_.extent.width,
                       swapchain_.extent.height,
                       "shaders/programs.optixir");
    }

    void Application::run() {
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();

            int fb_width = 0;
            int fb_height = 0;
            glfwGetFramebufferSize(window_, &fb_width, &fb_height);
            while ((fb_width == 0 || fb_height == 0) && !glfwWindowShouldClose(window_)) {
                glfwWaitEvents();
                glfwGetFramebufferSize(window_, &fb_width, &fb_height);
            }

            wait_frame_slot();
            // CUDA submit before acquire so the CUDA starts computing while the CPU
            // waits on acquire — submission order decides when the engine starts, hence
            // whether the acquire wait overlaps CUDA compute (async submit alone does not).
            renderer_.submit_cuda(cuda_context_,
                                  swapchain_.extent.width,
                                  swapchain_.extent.height,
                                  context_.frame_index);

            if (!acquire_image()) {
                continue;
            }
            imgui_backend_.begin_frame();

            // DeltaTime is only valid after imgui begin_frame has advanced ImGui's IO.
            renderer::DebugUIContext ui_ctx{
                .delta_time = ImGui::GetIO().DeltaTime,
                .context = context_,
                .swapchain = swapchain_,
                .error_message = error_message_,
            };
            const auto actions = debug_ui_.draw(ui_ctx);

            if (actions.error_dismissed) {
                error_message_.clear();
            }
            // present_mode_changed is acted on after end_frame (see below): recreating
            // mid-frame would invalidate the image acquired in acquire_image before it is
            // presented, so the frame runs to completion on the old swapchain first.

            record();
            end_frame();

            if (actions.present_mode_changed) {
                // The combo wrote the new selection into swapchain_.present_mode during
                // draw; now that this frame has been presented on the old swapchain,
                // recreate consumes the new mode and reflects the effective one back.
                recreate_swapchain();
            }
        }
    }

    void Application::wait_frame_slot() {
        // Fences start signaled (Context::create_frame_data), so the first frame's
        // wait returns immediately.
        const auto &frame = context_.current_frame();
        VK_CHECK(vkWaitForFences(context_.device, 1, &frame.render_fence, VK_TRUE, UINT64_MAX));
    }

    bool Application::acquire_image() {
        const auto &frame = context_.current_frame();

        const VkResult acquire_result = vkAcquireNextImageKHR(
            context_.device, swapchain_.swapchain,
            UINT64_MAX,
            frame.image_available_semaphore,
            VK_NULL_HANDLE,
            &image_index_);

        // OUT_OF_DATE: surface no longer compatible, recreate and skip this frame.
        // The fence has not been reset, so the next iteration's wait still guards the
        // last submit on this slot; recreate itself waits the queue idle first.
        if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
            // This frame's submit_cuda already signaled the forward semaphore.
            // Skipping submit2 would leave that signal unconsumed, so the next
            // frame's signal on the same binary semaphore would violate the
            // signal/wait pairing. The drain also signals the reverse semaphore:
            // with no blit this frame, nothing else would produce the signal the
            // next frame's CUDA tonemap wait consumes, so it would hang.
            // ReSharper disable once CppLocalVariableMayBeConst
            VkSemaphore forward_sem = interop_semaphores_[context_.frame_index].semaphore;
            const VkSemaphoreSubmitInfo drain_wait{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .semaphore = forward_sem,
                .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            };
            const VkSemaphoreSubmitInfo drain_signal{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .semaphore = reverse_interop_semaphore_.semaphore,
                .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            };
            const VkSubmitInfo2 drain_submit{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                .waitSemaphoreInfoCount = 1,
                .pWaitSemaphoreInfos = &drain_wait,
                .signalSemaphoreInfoCount = 1,
                .pSignalSemaphoreInfos = &drain_signal,
            };
            VK_CHECK(vkQueueSubmit2(context_.graphics_queue, 1, &drain_submit, VK_NULL_HANDLE));

            recreate_swapchain();
            return false;
        }
        // SUBOPTIMAL succeeds; handled at present time.
        if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
            spdlog::critical("vkAcquireNextImageKHR returned {}", static_cast<int>(acquire_result));
            std::abort();
        }

        // Reset so the next wait on this slot reflects this frame's submit, not the
        // previous frame's (fences do not auto-clear once signaled).
        VK_CHECK(vkResetFences(context_.device, 1, &frame.render_fence));
        return true;
    }

    void Application::record() {
        const auto &frame = context_.current_frame();
        // ReSharper disable once CppLocalVariableMayBeConst
        VkCommandBuffer cmd = frame.command_buffer;

        VK_CHECK(vkResetCommandBuffer(cmd, 0));

        constexpr VkCommandBufferBeginInfo begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));

        const renderer::RenderInput input{
            .cmd = cmd,
            .cuda_context = cuda_context_,
            .display_buffer = display_buffer_,
            .swapchain = swapchain_,
            .image_index = image_index_,
            .graphics_queue_family = context_.graphics_queue_family,
            .imgui = imgui_backend_,
        };
        qualquer::renderer::Renderer::record_vulkan(input);

        VK_CHECK(vkEndCommandBuffer(cmd));
    }

    void Application::end_frame() {
        const auto &frame = context_.current_frame();

        const VkCommandBufferSubmitInfo cmd_submit_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = frame.command_buffer,
        };

        const VkSemaphoreSubmitInfo wait_infos[2]{
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                // Presentation engine signals this after finishing its read of the
                // swapchain image. The first Vulkan stage that touches the image is
                // the blit (TRANSFER_DST transition + write), so the wait must
                // cover BLIT — anything earlier would let the blit race with the
                // presentation engine's read.
                .semaphore = frame.image_available_semaphore,
                .stageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            },
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                // External semaphore signaled by CUDA after writing the display buffer.
                // Waits the CUDA write before Vulkan reads the buffer (blit in
                // record_vulkan). TRANSFER is the exact first-read stage.
                .semaphore = interop_semaphores_[context_.frame_index].semaphore,
                .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            },
        };

        const VkSemaphoreSubmitInfo signal_infos[2]{
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                // Per-swapchain-image: see Swapchain::render_finished_semaphores.
                .semaphore = swapchain_.render_finished_semaphores[image_index_],
                .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            },
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                // Reverse interop: signaled when all transfer work (the blit) is done,
                // allowing the next frame's CUDA tonemap to overwrite display_surface.
                .semaphore = reverse_interop_semaphore_.semaphore,
                .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            },
        };

        const VkSubmitInfo2 submit_info{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 2,
            .pWaitSemaphoreInfos = wait_infos,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &cmd_submit_info,
            .signalSemaphoreInfoCount = 2,
            .pSignalSemaphoreInfos = signal_infos,
        };

        VK_CHECK(vkQueueSubmit2(context_.graphics_queue, 1, &submit_info, frame.render_fence));

        const VkPresentInfoKHR present_info{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &swapchain_.render_finished_semaphores[image_index_],
            .swapchainCount = 1,
            .pSwapchains = &swapchain_.swapchain,
            .pImageIndices = &image_index_,
        };

        // Recreate on driver-reported staleness or when the polled framebuffer size
        // diverges from the swapchain extent (Windows may not report a size change
        // via acquire/present in the same frame).
        if (const VkResult present_result = vkQueuePresentKHR(context_.graphics_queue, &present_info);
            present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) {
            recreate_swapchain();
        } else if (present_result != VK_SUCCESS) {
            spdlog::critical("vkQueuePresentKHR returned {}", static_cast<int>(present_result));
            std::abort();
        } else {
            int fb_width = 0;
            int fb_height = 0;
            glfwGetFramebufferSize(window_, &fb_width, &fb_height);
            if (static_cast<uint32_t>(fb_width) != swapchain_.extent.width ||
                static_cast<uint32_t>(fb_height) != swapchain_.extent.height) {
                recreate_swapchain();
            }
        }

        context_.advance_frame();
    }

    void Application::recreate_swapchain() {
        // Drain both CUDA streams before releasing resources they use.
        // compute_stream: raygen may still be reading/writing accum buffers.
        // display_stream: tonemap may still be writing display_surface.
        // Neither stream is covered by vkQueueWaitIdle (inside recreate) —
        // compute_stream has no semaphore link to the Vulkan queue, and the
        // forward semaphore linking display_stream is consumed on the GPU
        // timeline which hasn't necessarily been reached yet.
        CUDA_CHECK(cudaStreamSynchronize(cuda_context_.compute_stream));
        CUDA_CHECK(cudaStreamSynchronize(cuda_context_.display_stream));
        // The CUDA side must release its imported surface before the Vulkan image is
        // destroyed (the surface wraps that image's memory). External semaphores are
        // resolution-independent and stay.
        cuda_context_.release_display_buffer();
        swapchain_.recreate(context_);
        renderer_.resize(cuda_context_,
                         swapchain_.extent.width,
                         swapchain_.extent.height);
        display_buffer_.destroy(context_);
        display_buffer_.init(context_,
                             VK_FORMAT_R8G8B8A8_UNORM,
                             swapchain_.extent,
                             VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        cuda_context_.import_display_buffer(display_buffer_.win32_handle,
                                            swapchain_.extent.width,
                                            swapchain_.extent.height,
                                            display_buffer_.size);
    }

    void Application::destroy() {
        vkQueueWaitIdle(context_.graphics_queue);
        // vkQueueWaitIdle covers display_stream (linked via the forward semaphore)
        // but not compute_stream — drain it before freeing the resources it uses.
        CUDA_CHECK(cudaStreamSynchronize(cuda_context_.compute_stream));

        imgui_backend_.destroy();
        // Renderer's OptiX pipeline and CUDA buffers are torn down before the CUDA
        // context they were created against.
        renderer_.destroy();
        // CUDA releases its imported surface/semaphores before the Vulkan resources
        // they wrap: the surface backs the display-buffer image memory, and the
        // imported semaphores back the Vulkan external semaphores.
        cuda_context_.destroy();
        for (auto &sem: interop_semaphores_) {
            sem.destroy(context_);
        }
        reverse_interop_semaphore_.destroy(context_);
        display_buffer_.destroy(context_);
        swapchain_.destroy(context_);
        context_.destroy();

        glfwDestroyWindow(window_);
        glfwTerminate();
    }
} // namespace qualquer::app
