#pragma once

/**
 * @file context.h
 * @brief Vulkan context module and VK_CHECK macro.
 */

#include <string>

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

/**
 * @brief Checks a VkResult and aborts on failure with diagnostic output.
 *
 * Logs the failed expression, VkResult code, and source location via spdlog.
 * Vulkan API errors during development are programming errors
 * and do not need runtime recovery.
 */
#define VK_CHECK(x)                                                      \
    do {                                                                 \
        VkResult vk_check_result_ = (x);                                 \
        if (vk_check_result_ != VK_SUCCESS) {                            \
            spdlog::critical("VK_CHECK failed: {} returned {} at {}:{}", \
                             #x,                                         \
                             static_cast<int>(vk_check_result_),         \
                             __FILE__,                                   \
                             __LINE__);                                  \
            std::abort();                                                \
        }                                                                \
    } while (0)

struct GLFWwindow;

namespace qualquer::vulkan {

/**
 * @brief Core Vulkan context owning instance-level Vulkan objects.
 *
 * Lifetime is managed explicitly via init() and destroy().
 * Additional Vulkan objects (device, allocator) will be
 * added in subsequent steps.
 */
class Context {
public:
    /**
     * @brief Initializes the Vulkan context.
     * @param window GLFW window used to create the presentation surface.
     */
    void init(GLFWwindow* window);

    /**
     * @brief Destroys all Vulkan objects in reverse creation order.
     */
    void destroy();

    /** @brief Vulkan instance. */
    VkInstance instance = VK_NULL_HANDLE;

    /** @brief Debug messenger for validation layer callbacks. */
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    /** @brief Window surface for swapchain presentation. */
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    /** @brief Selected physical device (GPU). */
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;

    /** @brief Human-readable GPU name, populated during init. */
    std::string gpu_name;

    /** @brief Queue family index supporting both graphics and present. */
    uint32_t graphics_queue_family = 0;

    /** @brief Logical device. */
    VkDevice device = VK_NULL_HANDLE;

    /** @brief Graphics queue (also used for presentation). */
    VkQueue graphics_queue = VK_NULL_HANDLE;

private:
    /** @brief Creates VkInstance with validation layers and debug_utils extension. */
    void create_instance();

    /** @brief Sets up the debug messenger callback for validation messages. */
    void create_debug_messenger();

    /** @brief Selects a suitable physical device, preferring discrete GPUs. */
    void pick_physical_device();

    /**
     * @brief Finds and records the queue family supporting both graphics and present.
     *
     * Must be called after pick_physical_device(). Aborts if none found.
     */
    void find_graphics_queue_family();

    /**
     * @brief Creates the logical device with swapchain extension and Vulkan 1.4 core features.
     */
    void create_device();
};

}  // namespace qualquer::vulkan
