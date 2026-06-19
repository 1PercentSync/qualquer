/**
 * @file context.cpp
 * @brief Vulkan context implementation.
 */

#include <qualquer/vulkan/context.h>

#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace qualquer::vulkan {

namespace {

#ifdef NDEBUG
/** @brief Validation layers are disabled in release builds. */
constexpr bool kEnableValidationLayers = false;
#else
/** @brief Validation layers are enabled in debug builds. */
constexpr bool kEnableValidationLayers = true;
#endif

/** @brief All severity levels — let the callback do spdlog-level filtering at runtime. */
constexpr VkDebugUtilsMessageSeverityFlagsEXT kAllSeverityFlags =
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

// Builds a tag string from the message type bitmask (e.g. "[Validation][Performance]")
std::string format_message_type(const VkDebugUtilsMessageTypeFlagsEXT type) {
    std::string tag;
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) { tag += "[Validation]"; }
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) { tag += "[Performance]"; }
    return tag;
}

// Routes validation layer messages to spdlog by severity
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    const VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    [[maybe_unused]] void* user_data) {
    const auto tag = format_message_type(type);
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        spdlog::error("{} {}", tag, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        spdlog::warn("{} {}", tag, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        spdlog::info("{} {}", tag, callback_data->pMessage);
    } else {
        spdlog::debug("{} {}", tag, callback_data->pMessage);
    }
    return VK_FALSE;
}

}  // namespace

void Context::init(GLFWwindow* window) {
    create_instance();
    create_debug_messenger();
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    spdlog::info("Window surface created");
}

void Context::destroy() {
    // Reverse of init(): surface → debug messenger → instance
    vkDestroySurfaceKHR(instance, surface, nullptr);

    if constexpr (kEnableValidationLayers) {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        func(instance, debug_messenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    spdlog::info("Vulkan context destroyed");
}

void Context::create_instance() {
    // Declare the Vulkan API version for the loader and validation layers
    const VkApplicationInfo app_info{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_4,
    };

    // GLFW reports the surface extensions needed by the current platform
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    if (kEnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Assemble instance: API version, extensions, and validation layer (debug only)
    const auto validation_layer = "VK_LAYER_KHRONOS_validation";

    const VkInstanceCreateInfo create_info{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = kEnableValidationLayers ? 1u : 0u,
        .ppEnabledLayerNames = kEnableValidationLayers ? &validation_layer : nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VK_CHECK(vkCreateInstance(&create_info, nullptr, &instance));

    spdlog::info("Vulkan instance created (API 1.4)");
}

void Context::create_debug_messenger() {
    if constexpr (!kEnableValidationLayers) { return; }

    const VkDebugUtilsMessengerCreateInfoEXT create_info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = kAllSeverityFlags,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
    };

    // Extension function — must be loaded manually via vkGetInstanceProcAddr
    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    VK_CHECK(func(instance, &create_info, nullptr, &debug_messenger));

    spdlog::info("Debug messenger created");
}

}  // namespace qualquer::vulkan
