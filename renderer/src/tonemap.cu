/**
 * @file tonemap.cu
 * @brief Tone mapping kernel: HDR accumulation buffer -> LDR display surface.
 */

#include <cstdint>
#include <cstdio>

#include <cuda_runtime.h>

#include <qualquer/renderer/tonemap.h>

namespace qualquer::renderer {
    namespace {
        // Checks the last CUDA error and aborts on failure with diagnostic output.
        // Inlined here rather than sharing optix::cuda_check because that header pulls
        // in spdlog/fmt, whose UTF-8 requirements complicate nvcc compilation. The
        // output format matches CUDA_CHECK so failures read identically either side;
        // stderr is unbuffered so the line is guaranteed to print before abort.
#define CUDA_CHECK_KERNEL(x)                                                              \
            do {                                                                          \
                cudaError_t cuda_check_result_ = (x);                                     \
                if (cuda_check_result_ != cudaSuccess) {                                  \
                    std::fprintf(stderr,                                                 \
                                 "CUDA_CHECK failed: {} returned {} at {}:{}\n",         \
                                 #x,                                                      \
                                 cudaGetErrorString(cuda_check_result_),                  \
                                 __FILE__,                                                \
                                 __LINE__);                                               \
                    std::abort();                                                         \
                }                                                                         \
            } while (0)

        // Hard clamp to [0,1] is the placeholder transform; ACES + exposure replace
        // it once the PT path lights up (progressive implementation). fminf/fmaxf,
        // not a clamp() template, to avoid pulling <algorithm> into device code.
        __device__ __forceinline__ float clamp01(const float v) {
            return fminf(1.0f, fmaxf(0.0f, v));
        }

        // __float2uint_rn performs round-to-nearest-even and saturates out-of-range
        // inputs, avoiding the floating-point +0.5-then-truncate bias that a plain
        // integral cast would introduce on .5 boundaries.
        __global__ void tonemap_kernel(const float4 *accumulation_buffer,
                                        const cudaSurfaceObject_t display_surface,
                                        const uint32_t width,
                                        const uint32_t height) {
            const uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
            const uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
            if (x >= width || y >= height) {
                return;
            }

            const float4 hdr = accumulation_buffer[y * width + x];

            const uchar4 pixel{
                static_cast<uint8_t>(__float2uint_rn(clamp01(hdr.x) * 255.0f)),
                static_cast<uint8_t>(__float2uint_rn(clamp01(hdr.y) * 255.0f)),
                static_cast<uint8_t>(__float2uint_rn(clamp01(hdr.z) * 255.0f)),
                static_cast<uint8_t>(__float2uint_rn(clamp01(hdr.w) * 255.0f)),
            };

            surf2Dwrite(pixel,
                        display_surface,
                        static_cast<int>(x * sizeof(uchar4)),
                        static_cast<int>(y),
                        cudaBoundaryModeZero);
        }
    } // namespace

    void launch_tonemap(const float4 *accumulation_buffer,
                        const cudaSurfaceObject_t display_surface,
                        const uint32_t width,
                        const uint32_t height,
                        cudaStream_t stream) {
        constexpr uint32_t kBlockDim = 16;
        constexpr dim3 block(kBlockDim, kBlockDim);
        const dim3 grid((width + kBlockDim - 1) / kBlockDim, (height + kBlockDim - 1) / kBlockDim);

        tonemap_kernel<<<grid, block, 0, stream>>>(accumulation_buffer, display_surface, width, height);

        CUDA_CHECK_KERNEL(cudaGetLastError());
    }
} // namespace qualquer::renderer