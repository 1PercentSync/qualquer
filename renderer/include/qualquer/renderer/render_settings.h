#pragma once

/**
 * @file render_settings.h
 * @brief Runtime path-tracing parameters adjustable from the UI (renderer layer).
 */

#include <cstdint>

namespace qualquer::renderer {
    /**
     * @brief User-adjustable path-tracing parameters.
     *
     * Owned by Application as a runtime-only live state (not persisted to
     * config.json). Passed to the renderer each frame via SceneRenderInput;
     * the renderer detects changes and resets accumulation automatically.
     *
     * exposure_ev is stored in EV stops for the UI slider; Application
     * converts to a linear multiplier (pow(2, ev)) before passing to
     * the renderer, which multiplies color directly.
     */
    struct RenderSettings {
        /** @brief Maximum bounce depth per path (1..32). */
        uint32_t max_bounces = 16;

        /** @brief Samples traced per frame (1..64). */
        uint32_t samples_per_frame = 1;

        /**
         * @brief Render resolution height in pixels; the width derives from the
         *        display aspect ratio (compute_render_width).
         *
         * Initialized by Application to the native swapchain height at startup,
         * then decoupled from the swapchain: window resizes do not change it.
         */
        uint32_t render_height = 1080;

        /** @brief Exposure in EV stops; converted to linear pow(2, ev) at submit time. */
        float exposure_ev = 0.0f;

        /**
         * @brief When false, accumulation is paused: samples_per_frame is set
         *        to 0 so raygen preserves the current buffer, display freezes.
         */
        bool accumulation_enabled = true;

        /** @brief IBL Y-axis rotation in radians, accumulated from left-drag input. */
        float env_rotation = 0.0f;

        /** @brief DLSS-RR enabled. When true and the feature is active, raygen
         *  outputs single-frame noisy HDR and DLSS-RR handles temporal
         *  accumulation + denoising + upscaling. When false, Separate Sum
         *  accumulation is used (Phase 4 fallback). */
        bool dlss_enabled = false;
    };

    /**
     * @brief Derives the render width matching the display aspect ratio.
     *
     * Integer rounding of render_height * display_width / display_height;
     * yields exactly display_width when render_height == display_height, so
     * the default (native) setting is a 1:1 mapping.
     *
     * @param render_height  Render resolution height in pixels.
     * @param display_width  Display (swapchain) width in pixels.
     * @param display_height Display (swapchain) height in pixels (must be > 0).
     * @return Render width in pixels (at least 1).
     */
    inline uint32_t compute_render_width(const uint32_t render_height,
                                         const uint32_t display_width,
                                         const uint32_t display_height) {
        const uint64_t rounded = (static_cast<uint64_t>(render_height) * display_width +
                                  display_height / 2) / display_height;
        return rounded < 1 ? 1u : static_cast<uint32_t>(rounded);
    }
} // namespace qualquer::renderer
