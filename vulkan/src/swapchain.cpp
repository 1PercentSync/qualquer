/**
 * @file swapchain.cpp
 * @brief Vulkan swapchain implementation.
 */

#include <qualquer/vulkan/swapchain.h>

#include <vector>

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

#include <qualquer/vulkan/context.h>

namespace qualquer::vulkan {
    void Swapchain::init(const Context &context, const PresentMode mode) {
        present_mode = mode;
        create_resources(context, VK_NULL_HANDLE);
    }

    // Fences only track vkQueueSubmit completion, not vkQueuePresentKHR. The
    // graphics-queue wait covers both submits and presents on this queue without
    // stalling unrelated queues (vkDeviceWaitIdle would over-serialize).
    void Swapchain::recreate(const Context &context, const PresentMode mode) {
        vkQueueWaitIdle(context.graphics_queue);

        // Destroy old resolution-dependent resources; keep the old swapchain
        // handle so the driver can recycle resources across the swap.
        for (const auto semaphore : render_finished_semaphores) {
            vkDestroySemaphore(context.device, semaphore, nullptr);
        }
        for (const auto view : image_views) {
            vkDestroyImageView(context.device, view, nullptr);
        }
        render_finished_semaphores.clear();
        image_views.clear();
        images.clear();

        // ReSharper disable once CppLocalVariableMayBeConst
        VkSwapchainKHR old_swapchain = swapchain;
        swapchain = VK_NULL_HANDLE;

        // Set the requested mode before create_resources so choose_present_mode
        // reads the new intent.
        present_mode = mode;
        create_resources(context, old_swapchain);

        // The old swapchain is now retired; destroy it after the new one is in place.
        vkDestroySwapchainKHR(context.device, old_swapchain, nullptr);
    }

    // Reverse creation order: image views first, then the swapchain (which also
    // releases the swapchain images, so those handles are never destroyed manually).
    void Swapchain::destroy(const Context &context) const {
        for (const auto semaphore : render_finished_semaphores) {
            vkDestroySemaphore(context.device, semaphore, nullptr);
        }
        for (const auto view : image_views) {
            vkDestroyImageView(context.device, view, nullptr);
        }
        vkDestroySwapchainKHR(context.device, swapchain, nullptr);

        spdlog::info("Swapchain destroyed");
    }

    // old_swapchain lets the driver recycle resources across recreate (VK_NULL_HANDLE on first creation).
    // ReSharper disable once CppParameterMayBeConst
    void Swapchain::create_resources(const Context &context, VkSwapchainKHR old_swapchain) {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physical_device, context.surface, &capabilities);

        // supported_modes also feeds UI queries, so it is a member rather than a local.
        {
            uint32_t mode_count = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                context.physical_device, context.surface, &mode_count, nullptr);
            supported_modes.resize(mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                context.physical_device, context.surface, &mode_count, supported_modes.data());
        }

        const VkSurfaceFormatKHR surface_format = choose_surface_format(context.physical_device, context.surface);
        const VkPresentModeKHR vk_present_mode = choose_present_mode(present_mode);

        // Record the mode actually set (requested mode may have fallen back to FIFO).
        if (vk_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = PresentMode::Mailbox;
        } else if (vk_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            present_mode = PresentMode::Immediate;
        } else {
            present_mode = PresentMode::Fifo;
        }

        format = surface_format.format;
        // Win32 guarantees currentExtent equals the window size (no sentinel), so used directly.
        extent = capabilities.currentExtent;

        // +1 over minImageCount for triple-buffering headroom; clamp to maxImageCount (0 = unlimited).
        uint32_t image_count = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
            image_count = capabilities.maxImageCount;
        }

        // TRANSFER_DST: target of the display-buffer blit; COLOR_ATTACHMENT: ImGui draws on top.
        const VkSwapchainCreateInfoKHR create_info{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = context.surface,
            .minImageCount = image_count,
            .imageFormat = surface_format.format,
            .imageColorSpace = surface_format.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = vk_present_mode,
            .clipped = VK_TRUE,
            .oldSwapchain = old_swapchain,
        };

        VK_CHECK(vkCreateSwapchainKHR(context.device, &create_info, nullptr, &swapchain));

        // Images are owned by the swapchain; we only borrow the handles.
        vkGetSwapchainImagesKHR(context.device, swapchain, &image_count, nullptr);
        images.resize(image_count);
        vkGetSwapchainImagesKHR(context.device, swapchain, &image_count, images.data());

        create_image_views(context.device);
        create_render_finished_semaphores(context.device);

        spdlog::info("Swapchain created ({}x{}, {} images)",
                     extent.width,
                     extent.height,
                     image_count);
    }

    // Requires B8G8R8A8_SRGB with SRGB_NONLINEAR. No fallback: a format mismatch
    // would force downstream assumptions to track the actual format, so we treat
    // an unsupported preferred format as an unrecoverable init error. This is the
    // common case on Windows (BGRA is the display-native layout).
    // ReSharper disable CppParameterMayBeConst
    VkSurfaceFormatKHR Swapchain::choose_surface_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
        // ReSharper restore CppParameterMayBeConst
        uint32_t count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, formats.data());

        for (const auto &fmt: formats) {
            if (fmt.format == VK_FORMAT_B8G8R8A8_SRGB && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return fmt;
            }
        }

        spdlog::error("Required surface format B8G8R8A8_SRGB / SRGB_NONLINEAR not supported by the surface");
        std::abort();
    }

    // Reads the member supported_modes (populated in create_resources). FIFO is
    // always available; the others fall back to FIFO when unsupported by the surface.
    VkPresentModeKHR Swapchain::choose_present_mode(const PresentMode requested) const {
        if (requested == PresentMode::Fifo) {
            spdlog::info("Present mode: FIFO (vsync)");
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        const VkPresentModeKHR target = (requested == PresentMode::Mailbox)
                                            ? VK_PRESENT_MODE_MAILBOX_KHR
                                            : VK_PRESENT_MODE_IMMEDIATE_KHR;
        const char *const target_name = (requested == PresentMode::Mailbox) ? "MAILBOX" : "IMMEDIATE";

        for (const auto m: supported_modes) {
            if (m == target) {
                spdlog::info("Present mode: {}", target_name);
                return m;
            }
        }

        spdlog::warn("Present mode: FIFO ({} not available)", target_name);
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    // Creates a 2D color image view for each swapchain image.
    // ReSharper disable once CppParameterMayBeConst
    void Swapchain::create_image_views(VkDevice device) {
        image_views.resize(images.size());

        for (size_t i = 0; i < images.size(); ++i) {
            const VkImageViewCreateInfo view_info{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = format,
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };

            VK_CHECK(vkCreateImageView(device, &view_info, nullptr, &image_views[i]));
        }
    }
    // Creates one render-finished semaphore per swapchain image.
    // Per-swapchain-image (not per-frame) because vkQueuePresentKHR's semaphore
    // wait is consumed asynchronously by the presentation engine, and the render
    // fence does not guarantee that consumption has completed.
    // ReSharper disable once CppParameterMayBeConst
    void Swapchain::create_render_finished_semaphores(VkDevice device) {
        render_finished_semaphores.resize(images.size());

        constexpr VkSemaphoreCreateInfo semaphore_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        for (auto &semaphore : render_finished_semaphores) {
            VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore));
        }
    }
} // namespace qualquer::vulkan
