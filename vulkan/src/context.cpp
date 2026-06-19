/**
 * @file context.cpp
 * @brief Vulkan context implementation.
 */

#include <qualquer/vulkan/context.h>

#include <vector>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace qualquer::vulkan {

void Context::init(GLFWwindow* window) {
    create_instance();
    create_debug_messenger();
}

void Context::destroy() {
}

void Context::create_instance() {
}

void Context::create_debug_messenger() {
}

}  // namespace qualquer::vulkan
