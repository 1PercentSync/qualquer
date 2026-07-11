#pragma once

/**
 * @file optix_check.h
 * @brief OPTIX_CHECK macro.
 */

#include <cstdlib>

#include <optix.h>
#include <spdlog/spdlog.h>

/**
 * @brief Checks an OptixResult and aborts on failure with diagnostic output.
 *
 * Mirrors CUDA_CHECK / VK_CHECK: OptiX API errors are unrecoverable configuration failures.
 */
#define OPTIX_CHECK(x)                                                            \
    do {                                                                          \
        OptixResult optix_check_result_ = (x);                                    \
        if (optix_check_result_ != OPTIX_SUCCESS) {                               \
            spdlog::critical("OPTIX_CHECK failed: {} returned {} ({}) at {}:{}",  \
                             #x,                                                  \
                             optixGetErrorName(optix_check_result_),              \
                             optixGetErrorString(optix_check_result_),            \
                             __FILE__,                                            \
                             __LINE__);                                           \
            spdlog::default_logger()->flush();                                    \
            std::abort();                                                         \
        }                                                                         \
    } while (0)
