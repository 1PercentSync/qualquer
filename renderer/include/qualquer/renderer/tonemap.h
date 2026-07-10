#pragma once

/**
 * @file tonemap.h
 * @brief Tone mapping kernel launch entry (renderer layer).
 */

#include <cstdint>

#include <cuda_runtime.h>

namespace qualquer::renderer {
    /**
     * @brief HDR color buffer to LDR display buffer tone mapping with
     *        render-to-display resampling.
     *
     * Reads the float4 RGBA32F color buffer (CUDA array, texture object) at
     * render resolution and writes the tone-mapped uchar4 result into the
     * imported display surface (R8G8B8A8_UNORM) at display resolution.
     *
     * Resampling runs in linear HDR (mean) space before exposure and tonemap:
     * equal resolutions read 1:1 via tex2D; upscaling uses Catmull-Rom bicubic
     * (clamped to >= 0); downscaling uses a footprint box average (SSAA resolve).
     *
     * The color buffer holds a Separate-Sum total: this kernel divides by
     * sample_count to recover the mean, then applies exposure (linear multiplier)
     * and the Khronos PBR Neutral tone mapper before writing LDR pixels.
     *
     * @param color_tex           Texture object for the float4 HDR color buffer
     *                            (render_width x render_height, point-sample read).
     * @param display_surface     Surface object over the imported LDR display
     *                            buffer (display_width x display_height, write).
     * @param render_width        Color buffer width in pixels.
     * @param render_height       Color buffer height in pixels.
     * @param display_width       Display buffer width in pixels.
     * @param display_height      Display buffer height in pixels.
     * @param sample_count        Total samples accumulated (Separate-Sum divisor).
     * @param exposure            Linear color multiplier applied before tonemap.
     * @param stream              CUDA stream to launch on.
     */
    void launch_tonemap(cudaTextureObject_t color_tex,
                        cudaSurfaceObject_t display_surface,
                        uint32_t render_width,
                        uint32_t render_height,
                        uint32_t display_width,
                        uint32_t display_height,
                        uint32_t sample_count,
                        float exposure,
                        cudaStream_t stream);
} // namespace qualquer::renderer