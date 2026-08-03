#pragma once

/**
 * @file cuda_check_kernel.h
 * @brief CUDA_CHECK_KERNEL macro (optix layer).
 */

#include <cstdio>
#include <cstdlib>

#include <cuda_runtime.h>

/**
 * @brief Checks a cudaError_t and aborts on failure with diagnostic output.
 *
 * Equivalent to CUDA_CHECK but uses fprintf(stderr) instead of spdlog.
 * nvcc compilation of .cu files with spdlog/fmt headers triggers MSVC C4819
 * (code page cannot represent non-ASCII characters in NVIDIA headers);
 * this macro avoids that dependency.
 */
#define CUDA_CHECK_KERNEL(x)                                                              \
    do {                                                                                  \
        cudaError_t cuda_check_result_ = (x);                                             \
        if (cuda_check_result_ != cudaSuccess) {                                          \
            std::fprintf(stderr,                                                          \
                         "CUDA_CHECK failed: %s returned %s at %s:%d\n",                  \
                         #x,                                                              \
                         cudaGetErrorString(cuda_check_result_),                           \
                         __FILE__,                                                        \
                         __LINE__);                                                       \
            std::fflush(stderr);                                                          \
            std::abort();                                                                 \
        }                                                                                 \
    } while (0)
