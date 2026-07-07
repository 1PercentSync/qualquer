/**
 * @file tonemap.cu
 * @brief Tone mapping kernel: HDR accumulation buffer -> LDR display surface.
 */

#include <cstdint>

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check_kernel.h>
#include <qualquer/renderer/tonemap.h>
#include <qualquer/renderer/tonemap.cuh>

namespace qualquer::renderer {
    namespace {
        // PBR Neutral output is already in [0,1]; alpha is not a color and is
        // clamped directly. fminf/fmaxf, not a clamp() template, to avoid pulling
        // <algorithm> into device code.
        __device__ __forceinline__ float clamp01(const float v) {
            return fminf(1.0f, fmaxf(0.0f, v));
        }

        // __float2uint_rn performs round-to-nearest-even and saturates out-of-range
        // inputs, avoiding the floating-point +0.5-then-truncate bias that a plain
        // integral cast would introduce on .5 boundaries.
        __global__ void tonemap_kernel(const float4 *accumulation_buffer,
                                        const cudaSurfaceObject_t display_surface,
                                        const uint32_t width,
                                        const uint32_t height,
                                        const uint32_t sample_count,
                                        const float exposure) {
            const uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
            const uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
            if (x >= width || y >= height) {
                return;
            }

            // sample_count == 0: the buffer holds uninitialised or stale data
            // (init/resize/first-ever frame). Write black and return; raygen is
            // concurrently filling the OTHER buffer with the first valid sample,
            // which tonemap will read next frame.
            if (sample_count == 0) {
                const uchar4 black{0, 0, 0, 255};
                surf2Dwrite(black,
                            display_surface,
                            static_cast<int>(x * sizeof(uchar4)),
                            static_cast<int>(y),
                            cudaBoundaryModeZero);
                return;
            }

            const float4 hdr = accumulation_buffer[y * width + x];

            const float inv_count = 1.0f / static_cast<float>(sample_count);
            const float3 mean = make_float3(hdr.x, hdr.y, hdr.z) * inv_count;

            const float3 ldr = apply_tonemap(mean, exposure);

            const uchar4 pixel{
                static_cast<uint8_t>(__float2uint_rn(clamp01(ldr.x) * 255.0f)),
                static_cast<uint8_t>(__float2uint_rn(clamp01(ldr.y) * 255.0f)),
                static_cast<uint8_t>(__float2uint_rn(clamp01(ldr.z) * 255.0f)),
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
                        const uint32_t sample_count,
                        const float exposure,
                        cudaStream_t stream) {
        constexpr uint32_t kBlockDim = 16;
        constexpr dim3 block(kBlockDim, kBlockDim);
        const dim3 grid((width + kBlockDim - 1) / kBlockDim, (height + kBlockDim - 1) / kBlockDim);

        tonemap_kernel<<<grid, block, 0, stream>>>(accumulation_buffer, display_surface, width, height, sample_count, exposure);

        CUDA_CHECK_KERNEL(cudaGetLastError());
    }
} // namespace qualquer::renderer