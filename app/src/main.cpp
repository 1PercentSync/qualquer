/**
 * @file main.cpp
 * @brief Application entry point.
 */

#include <spdlog/spdlog.h>

namespace {

constexpr auto kLogLevel = spdlog::level::info;

}  // namespace

int main() {
    spdlog::set_level(kLogLevel);
    return 0;
}
