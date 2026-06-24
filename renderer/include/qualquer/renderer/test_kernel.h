#pragma once

/**
 * @file test_kernel.h
 * @brief Display-buffer test kernel launch entry.
 */

#include <cstdint>

#include <cuda_runtime.h>

namespace qualquer::renderer {
    /**
     * @brief Launches the display-buffer test kernel.
     *
     * The caller owns the frame counter that drives the animation.
     * @param surface Surface object over the imported display buffer.
     * @param width   Display buffer width in pixels.
     * @param height  Display buffer height in pixels.
     * @param frame   Monotonic frame counter for animation.
     */
    void launch_test_kernel(cudaSurfaceObject_t surface, uint32_t width, uint32_t height, uint32_t frame);
} // namespace qualquer::renderer
