/**
 * @file application.cpp
 * @brief Application implementation.
 */

#include <qualquer/app/application.h>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace qualquer::app {

namespace {

constexpr auto kLogLevel = spdlog::level::info;

constexpr int kInitialWidth = 1920;
constexpr int kInitialHeight = 1080;
constexpr auto kWindowTitle = "Qualquer";

}  // namespace

void Application::init() {
    spdlog::set_level(kLogLevel);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_ = glfwCreateWindow(kInitialWidth, kInitialHeight, kWindowTitle, nullptr, nullptr);
}

void Application::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize(window_, &fb_width, &fb_height);
        while ((fb_width == 0 || fb_height == 0) && !glfwWindowShouldClose(window_)) {
            glfwWaitEvents();
            glfwGetFramebufferSize(window_, &fb_width, &fb_height);
        }
    }
}

void Application::destroy() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

}  // namespace qualquer::app
