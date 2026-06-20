#pragma once

/**
 * @file imgui_backend.h
 * @brief ImGui integration module (renderer layer).
 */

#include <vulkan/vulkan.h>

namespace qualquer::vulkan {
    class Context;
    class Swapchain;
} // namespace qualquer::vulkan

struct GLFWwindow;

namespace qualquer::renderer {
    /**
     * @brief Owns the ImGui lifecycle: context, platform (GLFW) backend, and Vulkan renderer backend.
     *
     * Initializes ImGui with Vulkan Dynamic Rendering (no VkRenderPass) and the GLFW
     * platform backend, rendering directly onto the swapchain image as a UI overlay.
     *
     * Per the project ownership principle, this class holds no Vulkan handle owned by
     * vulkan::Context (device/instance/queue are read from the Context reference passed
     * to init()). The Vulkan renderer backend and GLFW platform backend manage their
     * own internal state via ImGui's shutdown functions; the descriptor pool is created
     * internally by the Vulkan backend (DescriptorPoolSize > 0), so this class owns no
     * VkDescriptorPool either.
     *
     * Lifetime is managed explicitly via init() and destroy().
     * destroy() must be called after the graphics queue is idle (no in-flight GPU work
     * references ImGui resources).
     */
    class ImGuiBackend {
    public:
        /**
         * @brief Initializes the ImGui context, GLFW backend, and Vulkan renderer backend.
         *
         * Creates the ImGui context, applies DPI scaling from the window, installs the
         * GLFW platform backend, and configures the Vulkan renderer backend for Dynamic
         * Rendering against the swapchain's color format.
         *
         * @param context   Vulkan context providing instance, physical device, logical
         *                  device, graphics queue, and queue family.
         * @param swapchain Swapchain providing the color format and image count.
         * @param window    GLFW window driving input and DPI.
         */
        void init(const vulkan::Context &context, const vulkan::Swapchain &swapchain, GLFWwindow *window);

        /**
         * @brief Shuts down the ImGui backends and destroys the ImGui context.
         *
         * Must be called only when the graphics queue is idle. The internal descriptor
         * pool (if any) is released by the Vulkan backend's shutdown.
         */
        void destroy();

        /**
         * @brief Begins a new ImGui frame.
         *
         * Calls NewFrame on the Vulkan and GLFW backends, then ImGui::NewFrame().
         * Must be called once per frame, before any ImGui widget calls and before the
         * rendering pass begins.
         */
        void begin_frame();

        /**
         * @brief Finalizes ImGui and records its draw commands.
         *
         * Calls ImGui::Render() and records the resulting draw data into the given
         * command buffer. Must be called inside an active dynamic rendering pass
         * (between vkCmdBeginRendering and vkCmdEndRendering).
         *
         * @param cmd Command buffer to record ImGui draw commands into.
         */
        void render(VkCommandBuffer cmd);
    };
} // namespace qualquer::renderer
