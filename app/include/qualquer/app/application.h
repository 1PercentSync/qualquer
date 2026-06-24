#pragma once

/**
 * @file application.h
 * @brief Main application class: window management, frame loop, init/destroy sequence.
 */

#include <array>
#include <string>

#include <qualquer/optix/context.h>
#include <qualquer/renderer/debug_ui.h>
#include <qualquer/renderer/imgui_backend.h>
#include <qualquer/renderer/renderer.h>
#include <qualquer/vulkan/context.h>
#include <qualquer/vulkan/interop.h>
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
        void destroy();

    private:
        /**
         * @brief Waits for the current frame slot's fence and resets it.
         *
         * Must be the first frame-loop step: ensures the slot's prior submit (and
         * its external-semaphore wait) completed before this frame reuses the slot's
         * binary semaphore.
         */
        void wait_frame_slot();

        /**
         * @brief Acquires the next swapchain image.
         * @return true if acquisition succeeded and the frame should proceed;
         *         false if the swapchain was out of date and has been recreated
         *         (the caller skips the rest of this frame and retries next iteration).
         */
        bool acquire_image();

        /** @brief Records this frame's Vulkan commands into the current command buffer. */
        void record();

        /**
         * @brief Submits the recorded commands and presents the swapchain image.
         *
         * Triggers swapchain recreation when the driver reports a stale swapchain
         * (present returning OUT_OF_DATE / SUBOPTIMAL) or when the polled framebuffer
         * size no longer matches the swapchain extent.
         */
        void end_frame();

        /**
         * @brief Recreates the swapchain after a size change, driver-reported
         *        staleness, or a present-mode change.
         *
         * Delegates to Swapchain::recreate, which consumes the current
         * swapchain.present_mode (the single source of truth — unchanged on resize,
         * written by the combo box on a mode change) and reflects the effective mode
         * back into it.
         */
        void recreate_swapchain();

        /** @brief GLFW window handle. */
        GLFWwindow *window_ = nullptr;

        /**
         * @brief OptiX/CUDA context (device selection, UUID).
         *
         * Initialized before the Vulkan context: its selected device UUID feeds
         * vulkan::Context::init so both layers bind the same physical GPU.
         */
        optix::Context cuda_context_;

        /** @brief Vulkan context (instance, debug messenger). */
        vulkan::Context context_;

        /** @brief Vulkan swapchain (surface, images, image views). */
        vulkan::Swapchain swapchain_;

        /**
         * @brief Display buffer: CUDA-written image blitted to the swapchain image.
         *
         * Recreated with the swapchain since its extent tracks the swapchain extent.
         */
        vulkan::InteropImage display_buffer_;

        /**
         * @brief Per-frame external semaphores for CUDA signal -> Vulkan wait.
         *
         * One per frame-in-flight. Resolution-independent, so created once and not
         * recreated on swapchain resize.
         */
        std::array<vulkan::InteropSemaphore, vulkan::kMaxFramesInFlight> interop_semaphores_{};

        /** @brief ImGui integration (context, backends, UI rendering). */
        renderer::ImGuiBackend imgui_backend_;

        /** @brief Single-frame render content (CUDA submit + Vulkan recording). */
        renderer::Renderer renderer_;

        /** @brief Debug panel (frame stats, GPU info, present-mode/log-level controls). */
        renderer::DebugUI debug_ui_;

        /**
         * @brief Current error message for the debug panel's dismissable banner.
         *
         * Empty when there is no error to show. Non-fatal errors that should not
         * abort the program surface here; fatal ones still abort via VK_CHECK.
         */
        std::string error_message_;

        /**
         * @brief Index of the swapchain image acquired for the current frame.
         *
         * Produced by acquire_image and consumed by record/end_frame.
         * It is a per-frame transient of the begin→end frame flow, not an input set
         * by a caller, so it lives as a member rather than being threaded as a param.
         */
        uint32_t image_index_ = 0;
    };
} // namespace qualquer::app
