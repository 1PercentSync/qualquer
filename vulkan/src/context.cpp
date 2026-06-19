/**
 * @file context.cpp
 * @brief Vulkan context implementation.
 */

#include <qualquer/vulkan/context.h>

#include <algorithm>
#include <cstring>
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

// Checks whether the device has a queue family supporting both graphics and present
bool has_graphics_present_queue(VkPhysicalDevice dev, VkSurfaceKHR surface) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, families.data());

    for (uint32_t i = 0; i < count; ++i) {
        if (!(families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) { continue; }

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &present_support);
        if (present_support) { return true; }
    }
    return false;
}

// Checks whether the device supports the VK_KHR_swapchain extension
bool has_swapchain_extension(VkPhysicalDevice dev) {
    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, extensions.data());

    for (const auto& ext : extensions) {
        if (std::strcmp(ext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Rates a physical device's suitability. Returns 0 if unsuitable.
 * Scoring: discrete GPU +1000, +1 per GB of device-local VRAM.
 */
int rate_device(VkPhysicalDevice dev, VkSurfaceKHR surface) {
    if (!has_graphics_present_queue(dev, surface)) { return 0; }
    if (!has_swapchain_extension(dev)) { return 0; }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(dev, &props);
    if (props.apiVersion < VK_API_VERSION_1_4) { return 0; }

    int score = 1;
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(dev, &mem_props);
    VkDeviceSize max_heap = 0;
    for (uint32_t i = 0; i < mem_props.memoryHeapCount; ++i) {
        if (mem_props.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            max_heap = std::max(max_heap, mem_props.memoryHeaps[i].size);
        }
    }
    score += static_cast<int>(max_heap / (1024 * 1024 * 1024));
    return score;
}

}  // namespace

void Context::init(GLFWwindow* window) {
    create_instance();
    create_debug_messenger();
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    spdlog::info("Window surface created");
    pick_physical_device();
    find_graphics_queue_family();
    create_device();
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

void Context::pick_physical_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (device_count == 0) {
        spdlog::error("No Vulkan-capable GPU found");
        std::abort();
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    int best_score = 0;
    for (VkPhysicalDevice dev : devices) {
        const int score = rate_device(dev, surface);
        if (score > best_score) {
            best_score = score;
            physical_device = dev;
        }
    }

    if (physical_device == VK_NULL_HANDLE) {
        spdlog::error("No suitable GPU found (need Vulkan 1.4 + swapchain + graphics/present queue)");
        std::abort();
    }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physical_device, &props);
    gpu_name = props.deviceName;

    spdlog::info("Selected GPU: {} (score: {})", gpu_name, best_score);
}

void Context::find_graphics_queue_family() {
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, nullptr);
    std::vector<VkQueueFamilyProperties> families(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, families.data());

    for (uint32_t i = 0; i < family_count; ++i) {
        if (!(families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) { continue; }

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
        if (present_support) {
            graphics_queue_family = i;
            spdlog::info("Queue family {} supports graphics + present", graphics_queue_family);
            return;
        }
    }

    spdlog::error("No queue family supports both graphics and present");
    std::abort();
}

void Context::create_device() {
    constexpr float queue_priority = 1.0f;
    const VkDeviceQueueCreateInfo queue_info{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphics_queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    VkPhysicalDeviceVulkan13Features features_13{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
    };

    const char* const enabled_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    const VkDeviceCreateInfo device_info{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features_13,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_info,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = enabled_extensions,
    };

    VK_CHECK(vkCreateDevice(physical_device, &device_info, nullptr, &device));

    spdlog::info("Logical device created (queue family: {})", graphics_queue_family);
}

}  // namespace qualquer::vulkan
