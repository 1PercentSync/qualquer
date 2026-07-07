#pragma once

/**
 * @file env_alias_table.h
 * @brief Environment map alias table construction for NEE importance sampling (app layer).
 */

#include <qualquer/renderer/launch_params.h>

#include <cstdint>
#include <vector>

namespace qualquer::app {

    /**
     * @brief Result of building an environment alias table.
     *
     * Holds the CPU-side alias table entries and the aggregate luminance needed
     * by the renderer to fill LaunchParams. GPU upload and lifetime management
     * are handled by the caller (SceneLoader).
     */
    struct EnvAliasTableResult {
        /** @brief One entry per equirect pixel (full resolution). */
        std::vector<renderer::EnvAliasEntry> entries;

        /** @brief Sum of all pixel weights (luminance × sin_theta). */
        float total_luminance;
    };

    /**
     * @brief Builds a full-resolution alias table from an equirectangular HDR image.
     *
     * Each pixel becomes one alias-table entry. Sampling weights are
     * luminance × sin(theta), correcting for the equirectangular projection's
     * non-uniform solid angle per pixel. Per-entry luminance (without the
     * sin_theta factor) is stored for env_pdf() evaluation on the device.
     *
     * Uses Vose's O(N) alias-table algorithm.
     *
     * @param rgb_data  Host pointer to RGB float32 equirect pixels (3 floats/pixel).
     * @param width     Source image width in pixels.
     * @param height    Source image height in pixels.
     * @return Alias table entries and total luminance, or empty on failure.
     */
    [[nodiscard]] EnvAliasTableResult build_env_alias_table(
        const float *rgb_data,
        uint32_t width,
        uint32_t height);

} // namespace qualquer::app
