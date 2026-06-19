/**
 * @file context.cpp
 * @brief Vulkan context implementation.
 */

#include <qualquer/vulkan/context.h>

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

}  // namespace

void Context::init(GLFWwindow* window) {
    create_instance();
    create_debug_messenger();
}

void Context::destroy() {
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
}

}  // namespace qualquer::vulkan
