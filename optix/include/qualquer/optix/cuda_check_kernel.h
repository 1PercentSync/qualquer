#pragma once

/**
 * @file cuda_check_kernel.h
 * @brief CUDA error checking macro for nvcc-compiled translation units (optix layer).
 *
 * nvcc compilation of .cu files with spdlog/fmt headers triggers MSVC C4819
 * (code page cannot represent non-ASCII characters in NVIDIA headers). This
 * header provides the same abort-on-error behavior as cuda_check.h but uses
 * fprintf(stderr) instead of spdlog, making it safe for all .cu files.
 */

#include <cstdio>
#include <cstdlib>

#include <cuda_runtime.h>

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
            std::abort();                                                                 \
        }                                                                                 \
    } while (0)
