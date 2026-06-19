#pragma once

/**
 * @file application.h
 * @brief Main application class: window management, frame loop, init/destroy sequence.
 */

#include <qualquer/vulkan/context.h>

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
        void run() const;

        /** @brief Destroys all resources in reverse init order. */
        void destroy();

    private:
        /** @brief GLFW window handle. */
        GLFWwindow *window_ = nullptr;

        /** @brief Vulkan context (instance, debug messenger). */
        vulkan::Context context_;
    };
} // namespace qualquer::app
