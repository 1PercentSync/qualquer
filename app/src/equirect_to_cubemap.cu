/**
 * @file equirect_to_cubemap.cu
 * @brief Equirectangular-to-cubemap CUDA kernel (app layer).
 */

#include <bit>
#include <cstdint>
#include <vector>

#include <cuda_fp16.h>
#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check_kernel.h>
#include <qualquer/renderer/math_utils.cuh>
#include <qualquer/app/equirect_to_cubemap.h>

namespace qualquer::app {
    namespace {
        constexpr float kHalfMax = 65504.0f;

        /**
         * Computes the world-space direction for a cubemap texel.
         *
         * Face ordering follows OpenGL/CUDA cubemap convention:
         * 0=+X, 1=-X, 2=+Y, 3=-Y, 4=+Z, 5=-Z.
         */
        __device__ __forceinline__ float3 cube_dir(const uint32_t face,
                                                   const float u, const float v) {
            // Map [0,1] UV to [-1,1] st.
            const float s = u * 2.0f - 1.0f;
            const float t = v * 2.0f - 1.0f;

            switch (face) {
                case 0: return make_float3( 1.0f,    -t,    -s);  // +X
                case 1: return make_float3(-1.0f,    -t,     s);  // -X
                case 2: return make_float3(    s,  1.0f,     t);  // +Y
                case 3: return make_float3(    s, -1.0f,    -t);  // -Y
                case 4: return make_float3(    s,    -t,  1.0f);  // +Z
                case 5: return make_float3(   -s,    -t, -1.0f);  // -Z
                default: return make_float3(0.0f, 0.0f, 0.0f);
            }
        }

        /**
         * Bilinear sample from the equirect RGB f32 source.
         *
         * Wraps horizontally (equirect seam), clamps vertically (poles).
         */
        __device__ __forceinline__ float3 sample_equirect(const float *src,
                                                          const uint32_t w,
                                                          const uint32_t h,
                                                          float u, float v) {
            // Wrap u to [0,1), clamp v to [0,1].
            u = u - floorf(u);
            v = fminf(1.0f, fmaxf(0.0f, v));

            const float px = u * static_cast<float>(w) - 0.5f;
            const float py = v * static_cast<float>(h) - 0.5f;

            const int x0 = static_cast<int>(floorf(px));
            const int y0 = static_cast<int>(floorf(py));
            const float fx = px - static_cast<float>(x0);
            const float fy = py - static_cast<float>(y0);

            // Wrap x horizontally, clamp y vertically.
            auto fetch = [&](int x, int y) -> float3 {
                x = ((x % static_cast<int>(w)) + static_cast<int>(w)) % static_cast<int>(w);
                y = min(max(y, 0), static_cast<int>(h) - 1);
                const uint32_t idx = static_cast<uint32_t>(y) * w + static_cast<uint32_t>(x);
                return make_float3(src[idx * 3], src[idx * 3 + 1], src[idx * 3 + 2]);
            };

            const float3 c00 = fetch(x0,     y0);
            const float3 c10 = fetch(x0 + 1, y0);
            const float3 c01 = fetch(x0,     y0 + 1);
            const float3 c11 = fetch(x0 + 1, y0 + 1);

            const float w00 = (1.0f - fx) * (1.0f - fy);
            const float w10 = fx * (1.0f - fy);
            const float w01 = (1.0f - fx) * fy;
            const float w11 = fx * fy;

            return make_float3(
                c00.x * w00 + c10.x * w10 + c01.x * w01 + c11.x * w11,
                c00.y * w00 + c10.y * w10 + c01.y * w01 + c11.y * w11,
                c00.z * w00 + c10.z * w10 + c01.z * w01 + c11.z * w11
            );
        }

        __global__ void equirect_to_cubemap_kernel(const float *__restrict__ src,
                                                   uint16_t *__restrict__ dst,
                                                   const uint32_t equirect_w,
                                                   const uint32_t equirect_h,
                                                   const uint32_t face_size) {
            const uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
            const uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
            const uint32_t face = blockIdx.z;

            if (x >= face_size || y >= face_size) {
                return;
            }

            // Texel center in [0,1].
            const float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(face_size);
            const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(face_size);

            const float3 dir = cube_dir(face, u, v);

            // Direction to equirect UV.
            const float len = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
            const float inv_len = 1.0f / len;
            const float dx = dir.x * inv_len;
            const float dy = dir.y * inv_len;
            const float dz = dir.z * inv_len;

            const float phi = atan2f(dz, dx);                         // [-PI, PI]
            const float theta = asinf(fminf(1.0f, fmaxf(-1.0f, dy))); // [-PI/2, PI/2]

            const float eq_u = phi / kTwoPi + 0.5f;
            const float eq_v = 0.5f - theta * kInvPi;

            const float3 color = sample_equirect(src, equirect_w, equirect_h, eq_u, eq_v);

            // Clamp to float16 max and pack as RGBA half4.
            const uint32_t face_pixels = face_size * face_size;
            const uint32_t pixel_idx = face * face_pixels + y * face_size + x;
            const uint32_t out_base = pixel_idx * 4;

            dst[out_base + 0] = __half_as_ushort(__float2half(fminf(color.x, kHalfMax)));
            dst[out_base + 1] = __half_as_ushort(__float2half(fminf(color.y, kHalfMax)));
            dst[out_base + 2] = __half_as_ushort(__float2half(fminf(color.z, kHalfMax)));
            dst[out_base + 3] = __half_as_ushort(__float2half(1.0f));
        }
    } // namespace

    std::vector<uint16_t> equirect_to_cubemap(const float *rgb_data,
                                              const uint32_t equirect_width,
                                              const uint32_t equirect_height,
                                              uint32_t &out_face_size) {
        // Derive face size from equirect width (360° → 90° per face).
        constexpr uint32_t kMaxCubemapSize = 4096;
        const uint32_t face_size = std::min(std::bit_ceil(equirect_width / 4), kMaxCubemapSize);
        out_face_size = face_size;

        const uint64_t src_floats = static_cast<uint64_t>(equirect_width) * equirect_height * 3;
        const uint64_t dst_uint16s = static_cast<uint64_t>(face_size) * face_size * 6 * 4;

        // Upload equirect source to device.
        float *d_src = nullptr;
        CUDA_CHECK_KERNEL(cudaMalloc(&d_src, src_floats * sizeof(float)));
        CUDA_CHECK_KERNEL(cudaMemcpy(d_src, rgb_data, src_floats * sizeof(float), cudaMemcpyHostToDevice));

        // Allocate device output (6 faces × face_size² × 4 halves).
        uint16_t *d_dst = nullptr;
        CUDA_CHECK_KERNEL(cudaMalloc(&d_dst, dst_uint16s * sizeof(uint16_t)));

        // Dispatch.
        constexpr uint32_t kBlockDim = 16;
        constexpr dim3 block(kBlockDim, kBlockDim);
        const dim3 grid((face_size + kBlockDim - 1) / kBlockDim,
                        (face_size + kBlockDim - 1) / kBlockDim,
                        6);

        equirect_to_cubemap_kernel<<<grid, block>>>(d_src, d_dst,
                                                    equirect_width, equirect_height,
                                                    face_size);
        CUDA_CHECK_KERNEL(cudaGetLastError());
        CUDA_CHECK_KERNEL(cudaDeviceSynchronize());

        // Download result to host.
        std::vector<uint16_t> result(dst_uint16s);
        CUDA_CHECK_KERNEL(cudaMemcpy(result.data(), d_dst, dst_uint16s * sizeof(uint16_t), cudaMemcpyDeviceToHost));

        CUDA_CHECK_KERNEL(cudaFree(d_src));
        CUDA_CHECK_KERNEL(cudaFree(d_dst));

        return result;
    }
} // namespace qualquer::app
