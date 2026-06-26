#pragma once

/**
 * @file launch_params.h
 * @brief OptiX launch parameters (renderer layer).
 */

#include <cstdint>

#include <cuda_runtime.h>

namespace qualquer::renderer {

/**
 * @brief Per-frame parameters uploaded to the OptiX device constant buffer.
 *
 * Shared between host (renderer.cpp) and device (programs.cu): the same
 * definition is included on both sides and must stay layout-compatible.
 */
struct LaunchParams {
    /** @brief Device pointer to the accumulation buffer (HDR RGBA32F, written by raygen). */
    float4 *accumulation_buffer;

    /** @brief Framebuffer width in pixels. */
    uint32_t width;

    /** @brief Framebuffer height in pixels. */
    uint32_t height;

    /** @brief Monotonic accumulation counter driving temporal noise variation. */
    uint32_t frame_index;
};

} // namespace qualquer::renderer
