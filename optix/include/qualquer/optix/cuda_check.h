#pragma once

/**
 * @file cuda_check.h
 * @brief CUDA_CHECK macro.
 */

#include <cstdlib>

#include <cuda_runtime.h>
#include <spdlog/spdlog.h>

/**
 * @brief Checks a cudaError_t and aborts on failure with diagnostic output.
 *
 * Mirrors VK_CHECK's fail-fast policy: CUDA runtime errors during init and
 * interop import are unrecoverable configuration failures.
 */
#define CUDA_CHECK(x)                                                             \
    do {                                                                          \
        cudaError_t cuda_check_result_ = (x);                                     \
        if (cuda_check_result_ != cudaSuccess) {                                  \
            spdlog::critical("CUDA_CHECK failed: {} returned {} at {}:{}",        \
                             #x,                                                  \
                             cudaGetErrorString(cuda_check_result_),              \
                             __FILE__,                                            \
                             __LINE__);                                           \
            std::abort();                                                         \
        }                                                                         \
    } while (0)
