#pragma once

/**
 * @file application.h
 * @brief Main application class: window management, frame loop, init/destroy sequence.
 */

#include <qualquer/vulkan/context.h>
#include <qualquer/vulkan/swapchain.h>

struct GLFWwindow;

namespace qualquer::app {
    /**
     * @brief Top-level application managing subsystems and frame loop.
     *
     * Lifetime is managed via init() and destroy().
     */
    class Application {
    public:
        /** @brief Initializes all subsystems. */
        void init();

        /** @brief Runs the main frame loop until the window is closed. */
        void run();

        /** @brief Destroys all resources in reverse init order. */
        void destroy() const;

    private:
        /**
         * @brief Waits for the current frame slot's fence and acquires a swapchain image.
         *
         * Handles the frame-loop head: vkWaitForFences, vkAcquireNextImageKHR, and
         * vkResetFences once acquisition succeeds. A stale swapchain (OUT_OF_DATE) is
         * not yet handled — it aborts until swapchain recreation is implemented.
         */
        void begin_frame();

        /** @brief Records this frame's commands into the current frame's command buffer. */
        void render_frame();

        /** @brief Submits the recorded commands and presents the swapchain image. */
        void end_frame();

        /** @brief GLFW window handle. */
        GLFWwindow *window_ = nullptr;

        /** @brief Vulkan context (instance, debug messenger). */
        vulkan::Context context_;

        /** @brief Vulkan swapchain (surface, images, image views). */
        vulkan::Swapchain swapchain_;

        /**
         * @brief Index of the swapchain image acquired for the current frame.
         *
         * Produced by begin_frame (acquire) and consumed by render_frame/end_frame.
         * It is a per-frame transient of the begin→end frame flow, not an input set
         * by a caller, so it lives as a member rather than being threaded as a param.
         */
        uint32_t image_index_ = 0;
    };
} // namespace qualquer::app
