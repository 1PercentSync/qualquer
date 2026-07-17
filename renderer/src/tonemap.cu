/**
 * @file tonemap.cu
 * @brief Tone mapping kernel: HDR accumulation buffer -> LDR display surface.
 */

#include <cstdint>

#include <cuda_fp16.h>
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

        // CUDA has no half4 surface write type: pack IEEE half bits into ushort4
        // (see CUDA programming guide / simpleSurfaceWrite half path).
        __device__ __forceinline__ ushort4 pack_half4(const float4 v) {
            return make_ushort4(
                __half_as_ushort(__float2half(v.x)),
                __half_as_ushort(__float2half(v.y)),
                __half_as_ushort(__float2half(v.z)),
                __half_as_ushort(__float2half(v.w)));
        }

        // 1D Catmull-Rom weights for the four taps at integer offsets -1..2
        // around the sample position, evaluated at fractional offset t in [0,1).
        // The weights sum to 1 exactly, so no normalization is needed.
        __device__ __forceinline__ void catmull_rom_weights(const float t, float w[4]) {
            const float t2 = t * t;
            const float t3 = t2 * t;
            w[0] = 0.5f * (-t3 + 2.0f * t2 - t);
            w[1] = 0.5f * (3.0f * t3 - 5.0f * t2 + 2.0f);
            w[2] = 0.5f * (-3.0f * t3 + 4.0f * t2 + t);
            w[3] = 0.5f * (t3 - t2);
        }

        // Catmull-Rom bicubic sample at source coordinate (sx, sy) using tex2D
        // point reads. Operates in the Separate-Sum domain — the caller
        // divides by the sample count afterwards. The 4x4 tap window is
        // clamped to the buffer edges via tex2D clamp addressing.
        __device__ float3 sample_catmull_rom(const cudaTextureObject_t color_tex,
                                             const float sx,
                                             const float sy) {
            const float bx = floorf(sx);
            const float by = floorf(sy);
            const int ix = static_cast<int>(bx);
            const int iy = static_cast<int>(by);

            float wx[4];
            float wy[4];
            catmull_rom_weights(sx - bx, wx);
            catmull_rom_weights(sy - by, wy);

            float3 sum = make_float3(0.0f, 0.0f, 0.0f);
            for (int j = 0; j < 4; ++j) {
                // tex2D with unnormalized coords + clamp addressing handles
                // edge clamping automatically — no manual min/max needed.
                const float ty = static_cast<float>(iy - 1 + j) + 0.5f;
                for (int i = 0; i < 4; ++i) {
                    const float tx = static_cast<float>(ix - 1 + i) + 0.5f;
                    const float4 texel = tex2D<float4>(color_tex, tx, ty);
                    sum = sum + make_float3(texel.x, texel.y, texel.z) * (wx[i] * wy[j]);
                }
            }
            return sum;
        }

        // Footprint box average over the source interval [x0,x1) x [y0,y1)
        // using tex2D point reads. Operates in the Separate-Sum domain. Each
        // covered texel contributes its overlap area with the footprint. The
        // SSAA resolve: no ringing, no undersampling at any ratio.
        __device__ float3 sample_box(const cudaTextureObject_t color_tex,
                                     const uint32_t render_width,
                                     const uint32_t render_height,
                                     const float x0,
                                     const float x1,
                                     const float y0,
                                     const float y1) {
            const int ix0 = max(static_cast<int>(floorf(x0)), 0);
            const int iy0 = max(static_cast<int>(floorf(y0)), 0);
            const int ix1 = min(static_cast<int>(ceilf(x1)), static_cast<int>(render_width));
            const int iy1 = min(static_cast<int>(ceilf(y1)), static_cast<int>(render_height));

            float3 sum = make_float3(0.0f, 0.0f, 0.0f);
            float weight_sum = 0.0f;
            for (int sy = iy0; sy < iy1; ++sy) {
                const float wy = fminf(y1, static_cast<float>(sy) + 1.0f) -
                                 fmaxf(y0, static_cast<float>(sy));
                for (int sx = ix0; sx < ix1; ++sx) {
                    const float wx = fminf(x1, static_cast<float>(sx) + 1.0f) -
                                     fmaxf(x0, static_cast<float>(sx));
                    const float w = wx * wy;
                    const float4 texel = tex2D<float4>(color_tex,
                                                       static_cast<float>(sx) + 0.5f,
                                                       static_cast<float>(sy) + 0.5f);
                    sum = sum + make_float3(texel.x, texel.y, texel.z) * w;
                    weight_sum += w;
                }
            }
            return sum / weight_sum;
        }

        // __float2uint_rn performs round-to-nearest-even and saturates out-of-range
        // inputs, avoiding the floating-point +0.5-then-truncate bias that a plain
        // integral cast would introduce on .5 boundaries.
        __global__ void tonemap_kernel(const cudaTextureObject_t color_tex,
                                        const cudaSurfaceObject_t display_surface,
                                        const uint32_t render_width,
                                        const uint32_t render_height,
                                        const uint32_t display_width,
                                        const uint32_t display_height,
                                        const float scale_x,
                                        const float scale_y,
                                        const uint32_t sample_count,
                                        const float exposure) {
            const uint32_t x = blockIdx.x * blockDim.x + threadIdx.x;
            const uint32_t y = blockIdx.y * blockDim.y + threadIdx.y;
            if (x >= display_width || y >= display_height) {
                return;
            }

            // sample_count == 0: the buffer holds uninitialised or stale data
            // (init/first-ever frame/just-reallocated after a render-resolution
            // change). Write black and return without touching the buffer;
            // raygen is concurrently filling the OTHER buffer with the first
            // valid sample, which tonemap will read next frame.
            // Display is R16G16B16A16_SFLOAT: write ushort4 half-bit packs, 8 B/texel.
            if (sample_count == 0) {
                const ushort4 black = pack_half4(make_float4(0.0f, 0.0f, 0.0f, 1.0f));
                surf2Dwrite(black,
                            display_surface,
                            static_cast<int>(x * sizeof(ushort4)),
                            static_cast<int>(y),
                            cudaBoundaryModeZero);
                return;
            }

            const float inv_count = 1.0f / static_cast<float>(sample_count);

            // Resampling runs in linear HDR (mean) space, before exposure and
            // tonemap. The branch is uniform across the launch (resolutions are
            // launch constants), so there is no warp divergence. tex2D with
            // unnormalized coords uses pixel-center convention (+0.5). Alpha is
            // a constant 1.0 written by raygen, so the resampling paths skip it.
            float3 mean;
            float alpha = 1.0f;
            if (render_width == display_width && render_height == display_height) {
                const float4 hdr = tex2D<float4>(color_tex,
                                                  static_cast<float>(x) + 0.5f,
                                                  static_cast<float>(y) + 0.5f);
                mean = make_float3(hdr.x, hdr.y, hdr.z) * inv_count;
                alpha = hdr.w;
            } else if (render_width > display_width || render_height > display_height) {
                // Downscale: box average over the display pixel's source
                // footprint. Also covers the near-parity corner where width
                // rounding leaves one axis at or slightly below 1:1 — the
                // footprint then spans 1-2 texels and degrades to a tent filter.
                const float x0 = static_cast<float>(x) * scale_x;
                const float y0 = static_cast<float>(y) * scale_y;
                mean = sample_box(color_tex, render_width, render_height,
                                  x0, x0 + scale_x, y0, y0 + scale_y) * inv_count;
            } else {
                // Upscale: Catmull-Rom around the display pixel center mapped
                // into source texel space (pixel-center convention: the -0.5
                // shift aligns texel centers). Negative lobes can undershoot on
                // HDR contrast edges, so the mean is clamped to >= 0.
                const float sx = (static_cast<float>(x) + 0.5f) * scale_x - 0.5f;
                const float sy = (static_cast<float>(y) + 0.5f) * scale_y - 0.5f;
                mean = sample_catmull_rom(color_tex, sx, sy) * inv_count;
                mean = make_float3(fmaxf(mean.x, 0.0f), fmaxf(mean.y, 0.0f), fmaxf(mean.z, 0.0f));
            }

            const float3 ldr = apply_tonemap(mean, exposure);

            // Linear LDR in [0,1]; sRGB encode happens on blit to the swapchain.
            // Half intermediate avoids 8-bit linear banding in shadows.
            const ushort4 pixel = pack_half4(make_float4(
                clamp01(ldr.x), clamp01(ldr.y), clamp01(ldr.z), clamp01(alpha)));

            surf2Dwrite(pixel,
                        display_surface,
                        static_cast<int>(x * sizeof(ushort4)),
                        static_cast<int>(y),
                        cudaBoundaryModeZero);
        }
    } // namespace

    void launch_tonemap(const cudaTextureObject_t color_tex,
                        const cudaSurfaceObject_t display_surface,
                        const uint32_t render_width,
                        const uint32_t render_height,
                        const uint32_t display_width,
                        const uint32_t display_height,
                        const uint32_t sample_count,
                        const float exposure,
                        cudaStream_t stream) {
        constexpr uint32_t kBlockDim = 16;
        constexpr dim3 block(kBlockDim, kBlockDim);
        const dim3 grid((display_width + kBlockDim - 1) / kBlockDim,
                        (display_height + kBlockDim - 1) / kBlockDim);

        const float scale_x = static_cast<float>(render_width)
                             / static_cast<float>(display_width);
        const float scale_y = static_cast<float>(render_height)
                             / static_cast<float>(display_height);

        tonemap_kernel<<<grid, block, 0, stream>>>(color_tex, display_surface,
                                                   render_width, render_height,
                                                   display_width, display_height,
                                                   scale_x, scale_y,
                                                   sample_count, exposure);

        CUDA_CHECK_KERNEL(cudaGetLastError());
    }
} // namespace qualquer::renderer