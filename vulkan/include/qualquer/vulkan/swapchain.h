#pragma once

/**
 * @file swapchain.h
 * @brief Vulkan swapchain: presentation surface, images, and image views.
 */

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace qualquer::vulkan {
    class Context;

    /**
     * @brief Swapchain present mode selection.
     *
     * Controls how rendered images are delivered to the display.
     * FIFO is always available; the others fall back to FIFO when unsupported.
     */
    enum class PresentMode : uint8_t {
        Fifo, ///< VSync — wait for vertical blank (VK_PRESENT_MODE_FIFO_KHR, guaranteed).
        Mailbox, ///< Triple-buffered, no tearing, low latency (VK_PRESENT_MODE_MAILBOX_KHR).
        Immediate, ///< No sync, allows tearing, uncapped framerate (VK_PRESENT_MODE_IMMEDIATE_KHR).
    };

    /**
     * @brief Manages a Vulkan swapchain and its associated image views.
     *
     * Owns the VkSwapchainKHR and creates one VkImageView per swapchain image.
     * The VkImage handles are owned by the swapchain itself (not manually destroyed).
     *
     * Per the project ownership principle, this class holds no handle owned by
     * Context. Device/physical-device/surface are obtained from the Context
     * reference passed to each operation.
     *
     * Lifetime is managed explicitly via init() and destroy(). Recreation on
     * resize or present-mode change is handled in a later step.
     */
    class Swapchain {
    public:
        /**
         * @brief Creates the swapchain and its image views.
         * @param context Vulkan context providing device, physical device, and surface.
         * @param window  GLFW window used to query framebuffer size for extent.
         * @param mode    Requested present mode (default Mailbox).
         */
        void init(const Context &context, GLFWwindow *window, PresentMode mode = PresentMode::Mailbox);

        /**
         * @brief Destroys image views and the swapchain in reverse creation order.
         * @param context Vulkan context providing the owning device.
         */
        void destroy(const Context &context) const;

        /** @brief Current present mode. Reflected back to the requested value after fallback. */
        PresentMode present_mode = PresentMode::Mailbox;

        /**
         * @brief Present modes supported by the (physical_device, surface) pair.
         *
         * Queried once during init() and cached for the surface lifetime: present-mode
         * support is a property of the surface and does not change on swapchain recreate.
         * Single source for both present-mode selection and UI queries.
         */
        std::vector<VkPresentModeKHR> supported_modes;

        /** @brief Swapchain handle. */
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        /** @brief Swapchain image format (e.g. B8G8R8A8_SRGB). */
        VkFormat format = VK_FORMAT_UNDEFINED;

        /** @brief Swapchain image extent in pixels. */
        VkExtent2D extent = {0, 0};

        /** @brief Swapchain images (owned by the swapchain, not manually destroyed). */
        std::vector<VkImage> images;

        /** @brief One image view per swapchain image. */
        std::vector<VkImageView> image_views;

    private:
        /**
         * @brief Selects the best surface format.
         *
         * Prefers B8G8R8A8_SRGB with SRGB_NONLINEAR color space.
         * Falls back to the first available format with a warning.
         */
        static VkSurfaceFormatKHR choose_surface_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

        /**
         * @brief Selects the Vulkan present mode from a requested PresentMode.
         *
         * Reads the cached supported_modes. Returns the requested mode if supported,
         * otherwise falls back to FIFO (always available). FIFO is returned as-is.
         *
         * @param requested Requested present mode (input — never mutated by this call).
         *
         * Non-static: depends on the instance's cached support list.
         */
        VkPresentModeKHR choose_present_mode(PresentMode requested) const;

        /**
         * @brief Determines the swapchain extent from surface capabilities.
         *
         * Uses the surface's currentExtent if defined, otherwise queries
         * the GLFW framebuffer size and clamps to the supported range.
         */
        static VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);

        /**
         * @brief Core creation logic shared by init() (and later recreate()).
         *
         * Queries surface capabilities, selects format/present mode/extent,
         * creates the swapchain, retrieves images, and creates image views.
         *
         * @param context       Vulkan context providing device, physical device, and surface.
         * @param window        GLFW window used to query framebuffer size for extent.
         * @param old_swapchain Previous swapchain handle for driver recycling,
         *                      or VK_NULL_HANDLE for first creation.
         */
        void create_resources(const Context &context, GLFWwindow *window, VkSwapchainKHR old_swapchain);

        /** @brief Creates a VkImageView for each swapchain image. */
        void create_image_views(VkDevice device);
    };
} // namespace qualquer::vulkan
