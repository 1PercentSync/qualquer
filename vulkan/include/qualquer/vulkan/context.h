#pragma once

/**
 * @file context.h
 * @brief Vulkan context: instance, debug messenger, and VK_CHECK macro.
 */

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
 * @brief Core Vulkan context owning instance and debug messenger.
 *
 * Lifetime is managed explicitly via init() and destroy().
 * Additional Vulkan objects (surface, device, allocator) will be
 * added in subsequent steps.
 */
class Context {
public:
    /**
     * @brief Initializes the Vulkan context.
     * @param window GLFW window (used for surface creation in a later step).
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

private:
    /** @brief Creates VkInstance with validation layers and debug_utils extension. */
    void create_instance();

    /** @brief Sets up the debug messenger callback for validation messages. */
    void create_debug_messenger();
};

}  // namespace qualquer::vulkan
