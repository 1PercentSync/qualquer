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

        /** @brief Exposure in EV stops; converted to linear pow(2, ev) at submit time. */
        float exposure_ev = 0.0f;

        /**
         * @brief When false, raygen overwrites the buffer every frame instead
         *        of accumulating onto previous samples (sample_count forced to 0).
         */
        bool accumulation_enabled = true;
    };
} // namespace qualquer::renderer
