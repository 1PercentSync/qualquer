#pragma once

/**
 * @file swapchain.h
 * @brief Vulkan swapchain: presentation surface, images, and image views.
 */

#include <vector>

#include <vulkan/vulkan.h>

namespace qualquer::vulkan {
    class Context;

    /**
     * @brief Short display label for a Vulkan present mode (e.g. "FIFO", "Mailbox").
     *
     * Single source for the enum-to-text mapping. Returns "Unknown" for modes the
     * renderer does not surface (FIFO_RELAXED, shared-present modes).
     */
    const char *to_label(VkPresentModeKHR mode);

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
     * Lifetime is managed explicitly via init() and destroy(). Recreation after
     * surface changes, driver-reported staleness, or present-mode changes is done
     * via recreate().
     */
    class Swapchain {
    public:
        /**
         * @brief Creates the swapchain and its image views.
         * @param context Vulkan context providing device, physical device, and surface.
         * @param mode    Requested present mode (caller-supplied; MAILBOX is the
         *                conventional default the application passes at startup).
         */
        void init(const Context &context, VkPresentModeKHR mode);

        /**
         * @brief Recreates the swapchain after surface size change, driver-reported
         *        staleness, or a present-mode change.
         *
         * Waits for the graphics queue to idle (fences do not track present completion),
         * destroys old image views and per-image semaphores, then rebuilds the swapchain
         * while passing the old handle to the driver for resource recycling. Uses the
         * current present_mode as the requested mode; present_mode is then updated to the
         * actually-effective mode (which may fall back to FIFO when unsupported). A mode
         * change is initiated by writing present_mode before calling this.
         * @param context Vulkan context providing device, physical device, queue, and surface.
         */
        void recreate(const Context &context);

        /**
         * @brief Destroys image views and the swapchain in reverse creation order.
         * @param context Vulkan context providing the owning device.
         */
        void destroy(const Context &context) const;

        /** @brief Current present mode. Reflected back to the effective value after fallback. */
        VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR;

        /**
         * @brief Present modes supported by the (physical_device, surface) pair.
         *
         * Populated in create_resources() and exposed for UI queries. Re-queried on
         * every creation (init / recreate), but present-mode support is a property of
         * the surface, so the value is stable for a given surface.
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

        /**
         * @brief One render-finished semaphore per swapchain image.
         *
         * Indexed by the acquired image index (not the frame index).
         * The presentation engine holds the semaphore until the image is
         * actually displayed, so per-frame semaphores are insufficient
         * when the swapchain has more images than frames in flight.
         */
        std::vector<VkSemaphore> render_finished_semaphores;

    private:
        /**
         * @brief Requires the B8G8R8A8_SRGB + SRGB_NONLINEAR surface format.
         *
         * No fallback: aborts if the surface does not offer the preferred format,
         * since a mismatch would force downstream code to track the actual format.
         */
        static VkSurfaceFormatKHR choose_surface_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

        /**
         * @brief Resolves a requested Vulkan present mode to the effective one.
         *
         * Returns the requested mode if it is in supported_modes, otherwise FIFO
         * (always available).
         *
         * @param requested Requested present mode (input — never mutated by this call).
         *
         * Non-static: depends on the instance's supported_modes member.
         */
        [[nodiscard]] VkPresentModeKHR choose_present_mode(VkPresentModeKHR requested) const;

        /**
         * @brief Core creation logic shared by init() and recreate().
         *
         * Queries surface capabilities, selects format/present mode, creates the
         * swapchain, retrieves images, and creates image views. Extent is taken
         * directly from currentExtent (Win32 guarantees it equals the window size).
         *
         * @param context       Vulkan context providing device, physical device, and surface.
         * @param old_swapchain Previous swapchain handle for driver recycling,
         *                      or VK_NULL_HANDLE for first creation.
         */
        void create_resources(const Context &context, VkSwapchainKHR old_swapchain);

        /** @brief Creates a VkImageView for each swapchain image. */
        void create_image_views(VkDevice device);

        /** @brief Creates one render-finished semaphore per swapchain image. */
        void create_render_finished_semaphores(VkDevice device);
    };
} // namespace qualquer::vulkan
