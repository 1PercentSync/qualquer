/**
 * @file application.cpp
 * @brief Application implementation.
 */

#include <qualquer/app/application.h>

#include <spdlog/spdlog.h>

namespace qualquer::app {

namespace {

constexpr auto kLogLevel = spdlog::level::info;

}  // namespace

void Application::init() {
    spdlog::set_level(kLogLevel);
}

void Application::run() {
}

void Application::destroy() {
}

}  // namespace qualquer::app
