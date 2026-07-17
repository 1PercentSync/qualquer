#pragma once

/**
 * @file env_alias_table.h
 * @brief Environment map alias table construction for NEE importance sampling (app layer).
 */

#include <qualquer/renderer/launch_params.h>

#include <cstdint>
#include <vector>

namespace qualquer::app {

    /** @brief Maximum alias table width (each dimension independently capped). */
    constexpr uint32_t kMaxAliasWidth = 1024;

    /** @brief Maximum alias table height (each dimension independently capped). */
    constexpr uint32_t kMaxAliasHeight = 512;

    /**
     * @brief Result of building an environment alias table.
     *
     * Holds the CPU-side alias table entries and metadata needed by the
     * renderer to fill LaunchParams. GPU upload and lifetime management are
     * handled by the caller (SceneLoader).
     */
    struct EnvAliasTableResult {
        /** @brief One entry per alias-table cell (downsampled resolution). */
        std::vector<renderer::EnvAliasEntry> entries;

        /** @brief Sum of all cell weights (block luminance × sin_theta). */
        float total_luminance;

        /** @brief Alias table width (after downsampling). */
        uint32_t alias_width;

        /** @brief Alias table height (after downsampling). */
        uint32_t alias_height;
    };

    /**
     * @brief Builds a downsampled alias table from an equirectangular HDR image.
     *
     * Computes per-pixel luminance at full resolution, then box-filter
     * downsamples to the smallest power-of-2 factor that brings both
     * dimensions within kMaxAliasWidth × kMaxAliasHeight. Each alias-table
     * entry represents one downsampled block; its stored luminance (without
     * sin_theta) is the block average, used for env_pdf() on the device.
     * Sampling weights are block luminance × sin(theta_block_center).
     *
     * Uses Vose's O(N) alias-table algorithm.
     *
     * The power-of-2 constraint means non-power-of-2 input dimensions may
     * produce a suboptimal downsampling ratio (e.g. 3072×1536 ÷4 → 768×384
     * instead of the ideal 1024×512). This is a known acceptable limitation.
     *
     * @param rgb_data  Host pointer to RGB float32 equirect pixels (3 floats/pixel).
     * @param width     Source image width in pixels.
     * @param height    Source image height in pixels.
     * @return Alias table entries, total luminance, and table dimensions;
     *         empty entries on failure.
     */
    [[nodiscard]] EnvAliasTableResult build_env_alias_table(
        const float *rgb_data,
        uint32_t width,
        uint32_t height);

} // namespace qualquer::app
