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

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window_ = glfwCreateWindow(kInitialWidth, kInitialHeight, kWindowTitle, nullptr, nullptr);

        // Three-stage init: Vulkan pre-init enumerates presentable devices, CUDA
        // selects among them (presentability-constrained), Vulkan completes on
        // the CUDA-selected device. This binds both layers to the same physical
        // GPU and excludes compute-only devices (e.g. a TCC GPU) up front.
        const std::vector<std::array<std::uint8_t, 16>> presentable_uuids = context_.pre_init(window_);
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
        imgui_backend_.init(context_, swapchain_, window_);
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

            if (!begin_frame()) {
                continue;
            }
            imgui_backend_.begin_frame();

            // DeltaTime is only valid after begin_frame has advanced ImGui's IO.
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
            // mid-frame would invalidate the image acquired in begin_frame before it is
            // presented, so the frame runs to completion on the old swapchain first.

            render_frame();
            end_frame();

            if (actions.present_mode_changed) {
                // The combo wrote the new selection into swapchain_.present_mode during
                // draw; now that this frame has been presented on the old swapchain,
                // recreate consumes the new mode and reflects the effective one back.
                recreate_swapchain();
            }
        }
    }

    bool Application::begin_frame() {
        // Fences start signaled (Context::create_frame_data), so the first frame's
        // wait returns immediately.
        const auto &frame = context_.current_frame();
        VK_CHECK(vkWaitForFences(context_.device, 1, &frame.render_fence, VK_TRUE, UINT64_MAX));

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

    void Application::render_frame() {
        const auto &frame = context_.current_frame();
        // ReSharper disable once CppLocalVariableMayBeConst
        VkCommandBuffer cmd = frame.command_buffer;

        VK_CHECK(vkResetCommandBuffer(cmd, 0));

        constexpr VkCommandBufferBeginInfo begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));

        VkImage swapchain_image = swapchain_.images[image_index_];

        // No prior GPU work touched the acquired image (acquire hands it over via
        // the wait semaphore in end_frame), so the src stage is TOP_OF_PIPE.
        const VkImageMemoryBarrier2 to_attachment{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
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

        const VkRenderingAttachmentInfo color_attachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = swapchain_.image_views[image_index_],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue =
            {
                .color = {{0.0f, 0.0f, 0.0f, 1.0f}},
            },
        };

        const VkRenderingInfo rendering_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea =
            {
                .offset = {0, 0},
                .extent = swapchain_.extent,
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment,
        };

        vkCmdBeginRendering(cmd, &rendering_info);
        imgui_backend_.render(cmd);
        vkCmdEndRendering(cmd);

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

        VK_CHECK(vkEndCommandBuffer(cmd));
    }

    void Application::end_frame() {
        const auto &frame = context_.current_frame();

        const VkCommandBufferSubmitInfo cmd_submit_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = frame.command_buffer,
        };

        const VkSemaphoreSubmitInfo wait_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frame.image_available_semaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        };

        // Per-swapchain-image (not per-frame-slot): see Swapchain::render_finished_semaphores.
        const VkSemaphoreSubmitInfo signal_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = swapchain_.render_finished_semaphores[image_index_],
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        };

        const VkSubmitInfo2 submit_info{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &wait_info,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &cmd_submit_info,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &signal_info,
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
        // Swapchain::recreate waits the graphics queue idle first, which also guards
        // the display buffer's destruction — any GPU work referencing it has finished.
        // The CUDA side must release its imported surface before the Vulkan image is
        // destroyed (the surface wraps that image's memory). External semaphores are
        // resolution-independent and stay.
        cuda_context_.release_display_buffer();
        swapchain_.recreate(context_);
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

        imgui_backend_.destroy();
        // CUDA releases its imported surface/semaphores before the Vulkan resources
        // they wrap: the surface backs the display-buffer image memory, and the
        // imported semaphores back the Vulkan external semaphores.
        cuda_context_.destroy();
        for (auto &sem: interop_semaphores_) {
            sem.destroy(context_);
        }
        display_buffer_.destroy(context_);
        swapchain_.destroy(context_);
        context_.destroy();

        glfwDestroyWindow(window_);
        glfwTerminate();
    }
} // namespace qualquer::app
