#pragma once

/**
 * @file vk_check.h
 * @brief VK_CHECK macro.
 */

#include <cstdlib>

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
            spdlog::default_logger()->flush();                           \
            std::abort();                                                \
        }                                                                \
    } while (0)
