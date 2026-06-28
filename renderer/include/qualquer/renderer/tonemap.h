#pragma once

/**
 * @file tonemap.h
 * @brief Tone mapping kernel launch entry (renderer layer).
 */

#include <cstdint>

#include <cuda_runtime.h>

namespace qualquer::renderer {
    /**
     * @brief HDR accumulation buffer to LDR display buffer tone mapping.
     *
     * Reads the float4 RGBA32F accumulation buffer and writes the tone-mapped
     * uchar4 result into the imported display surface (R8G8B8A8_UNORM). Hosts the
     * HDR→LDR transform as an independent CUDA kernel, distinct from the OptiX
     * device programs; Vulkan only blits the resulting display buffer to the
     * swapchain.
     *
     * Which ping-pong accumulation buffer is read is decided by the caller —
     * this function is agnostic to the ping-pong indexing.
     * @param accumulation_buffer Device pointer to the float4 HDR buffer (read).
     * @param display_surface     Surface object over the imported LDR display buffer (write).
     * @param width               Framebuffer width in pixels.
     * @param height              Framebuffer height in pixels.
     * @param stream              CUDA stream to launch on.
     */
    void launch_tonemap(const float4 *accumulation_buffer,
                        cudaSurfaceObject_t display_surface,
                        uint32_t width,
                        uint32_t height,
                        cudaStream_t stream);
} // namespace qualquer::renderer