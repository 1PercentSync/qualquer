/**
 * @file application.cpp
 * @brief Application implementation.
 */

#include <qualquer/app/application.h>

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

        context_.init(window_);
        swapchain_.init(context_);
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

            begin_frame();
            render_frame();
            end_frame();
        }
    }

    void Application::begin_frame() {
        // Block until the GPU has finished using this frame slot's resources.
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

        // SUBOPTIMAL is acceptable: acquisition succeeded, present-side handling
        // will trigger recreation later. OUT_OF_DATE requires recreation before any
        // work is recorded — not yet implemented, so it is unrecoverable here.
        if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
            spdlog::critical("vkAcquireNextImageKHR: swapchain out of date "
                "(recreate not yet implemented)");
            std::abort();
        }
        if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
            spdlog::critical("vkAcquireNextImageKHR returned {}", static_cast<int>(acquire_result));
            std::abort();
        }

        // The fence is reused by this frame's submit; reset it so the next wait on
        // this slot reflects this frame's completion rather than the previous one.
        VK_CHECK(vkResetFences(context_.device, 1, &frame.render_fence));
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

        // Recording: empty in this step. Layout transition and clear are added
        // in the following Step 7 items.
        VK_CHECK(vkEndCommandBuffer(cmd));
    }

    void Application::end_frame() {
        const auto &frame = context_.current_frame();

        const VkCommandBufferSubmitInfo cmd_submit_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = frame.command_buffer,
        };

        // Wait on image availability before writing to the swapchain image, so
        // the GPU does not touch the image until acquisition is complete.
        const VkSemaphoreSubmitInfo wait_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frame.image_available_semaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        };

        // Signal per-swapchain-image render-finished semaphore (indexed by the
        // acquired image, not the frame slot) for presentation.
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

        // Present result handling (OUT_OF_DATE/SUBOPTIMAL/resize) is a later Step 7 item.
        VK_CHECK(vkQueuePresentKHR(context_.graphics_queue, &present_info));

        context_.advance_frame();
    }

    void Application::destroy() const {
        swapchain_.destroy(context_);
        context_.destroy();

        glfwDestroyWindow(window_);
        glfwTerminate();
    }
} // namespace qualquer::app
