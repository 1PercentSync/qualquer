/**
 * @file test_kernel.cu
 * @brief Display-buffer test kernel: animated UV gradient.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <cuda_runtime.h>

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

        // Animated UV gradient with a frame counter, producing a moving pattern that
        // verifies the CUDA write path and per-frame animation end-to-end.
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
                            const uint32_t frame,
                            const cudaStream_t stream) {
        constexpr uint32_t kBlockDim = 16;
        constexpr dim3 block(kBlockDim, kBlockDim);
        const dim3 grid((width + kBlockDim - 1) / kBlockDim, (height + kBlockDim - 1) / kBlockDim);

        test_kernel<<<grid, block, 0, stream>>>(surface, width, height, frame);

        CUDA_CHECK_KERNEL(cudaGetLastError());
    }
} // namespace qualquer::renderer
