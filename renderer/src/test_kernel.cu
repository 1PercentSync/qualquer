/**
 * @file test_kernel.cu
 * @brief Display-buffer test kernel: animated UV gradient.
 */

#include <cstdint>

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::renderer {
    namespace {
        // Animated UV gradient: color channels driven by normalized pixel position
        // and a frame counter, producing a moving pattern that verifies the CUDA
        // write path and per-frame animation end-to-end.
        __global__ void test_kernel(const cudaSurfaceObject_t surface,
                                    const uint32_t width,
                                    const uint32_t height,
                                    const uint32_t frame) {
            const uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
            const uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
            if (x >= width || y >= height) {
                return;
            }

            const float u = static_cast<float>(x) / static_cast<float>(width);
            const float v = static_cast<float>(y) / static_cast<float>(height);
            const float t = static_cast<float>(frame) * 0.01f;

            const float r = 0.5f + 0.5f * sinf(u * 6.2831853f + t);
            const float g = 0.5f + 0.5f * sinf(v * 6.2831853f + t * 1.3f);
            const float b = u;

            const uchar4 pixel{
                static_cast<uint8_t>(r * 255.0f),
                static_cast<uint8_t>(g * 255.0f),
                static_cast<uint8_t>(b * 255.0f),
                255u,
            };

            surf2Dwrite(pixel,
                        surface,
                        static_cast<int>(x * sizeof(uchar4)),
                        static_cast<int>(y),
                        cudaBoundaryModeZero);
        }
    } // namespace

    void launch_test_kernel(const cudaSurfaceObject_t surface,
                            const uint32_t width,
                            const uint32_t height,
                            const uint32_t frame) {
        constexpr uint32_t kBlockDim = 16;
        constexpr dim3 block(kBlockDim, kBlockDim);
        const dim3 grid((width + kBlockDim - 1) / kBlockDim, (height + kBlockDim - 1) / kBlockDim);

        test_kernel<<<grid, block>>>(surface, width, height, frame);

        CUDA_CHECK(cudaGetLastError());
    }
} // namespace qualquer::renderer
