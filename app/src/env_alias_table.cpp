/**
 * @file env_alias_table.cpp
 * @brief Environment map alias table construction implementation (app layer).
 */

#include <qualquer/app/env_alias_table.h>

#include <spdlog/spdlog.h>

#include <cmath>
#include <limits>
#include <numbers>
#include <vector>

namespace qualquer::app {

    EnvAliasTableResult build_env_alias_table(
        const float *rgb_data,
        const uint32_t width,
        const uint32_t height) {
        if (!rgb_data || width == 0 || height == 0) {
            spdlog::error("build_env_alias_table: invalid input (null data or zero dimensions)");
            return {};
        }

        // --- Compute per-pixel luminance at full resolution ---
        const uint32_t pixel_count = width * height;
        std::vector<float> full_luminances(pixel_count);

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                const uint32_t pixel = y * width + x;
                const uint32_t src = pixel * 3;
                full_luminances[pixel] = 0.2126f * rgb_data[src + 0]
                                       + 0.7152f * rgb_data[src + 1]
                                       + 0.0722f * rgb_data[src + 2];
            }
        }

        // --- Determine power-of-2 downsampling factor ---
        uint32_t factor = 1;
        while (width / (factor * 2) > kMaxAliasWidth
               || height / (factor * 2) > kMaxAliasHeight) {
            factor *= 2;
        }
        // One more doubling if still exceeds target at current factor.
        if (width / factor > kMaxAliasWidth || height / factor > kMaxAliasHeight) {
            factor *= 2;
        }

        const uint32_t ds_w = width / factor;
        const uint32_t ds_h = height / factor;
        const uint32_t entry_count = ds_w * ds_h;

        if (entry_count == 0) {
            spdlog::error("build_env_alias_table: downsampled dimensions are zero "
                          "(input {}x{}, factor {})", width, height, factor);
            return {};
        }

        // --- Box-filter downsample luminances and compute sampling weights ---
        std::vector<float> block_luminances(entry_count);
        std::vector<float> weights(entry_count);
        const auto f_ds_h = static_cast<float>(ds_h);
        const auto inv_block_area = 1.0f / static_cast<float>(factor * factor);
        double weight_sum = 0.0;

        for (uint32_t by = 0; by < ds_h; ++by) {
            // sin(theta) at block center for equirectangular solid-angle correction.
            // Block center maps to the midpoint of the source rows it covers.
            const float theta = std::numbers::pi_v<float>
                                * (static_cast<float>(by) + 0.5f) / f_ds_h;
            const float sin_theta = std::sin(theta);

            for (uint32_t bx = 0; bx < ds_w; ++bx) {
                // Average luminance over the factor × factor source block.
                float lum_sum = 0.0f;
                const uint32_t src_x0 = bx * factor;
                const uint32_t src_y0 = by * factor;
                for (uint32_t dy = 0; dy < factor; ++dy) {
                    for (uint32_t dx = 0; dx < factor; ++dx) {
                        lum_sum += full_luminances[(src_y0 + dy) * width + (src_x0 + dx)];
                    }
                }
                const float block_lum = lum_sum * inv_block_area;

                const uint32_t idx = by * ds_w + bx;
                block_luminances[idx] = block_lum;

                const float w = block_lum * sin_theta;
                weights[idx] = w;
                weight_sum += static_cast<double>(w);
            }
        }

        // Scale luminances so total fits in float. env_pdf divides
        // lum[i] by total — uniform scaling cancels, PDF unchanged.
        float lum_scale = 1.0f;
        if (!std::isfinite(static_cast<float>(weight_sum))) {
            lum_scale = static_cast<float>(
                static_cast<double>(std::numeric_limits<float>::max()) * 0.5 / weight_sum);
            spdlog::warn("Env alias table: weight_sum overflows float (double={:.6e}), "
                         "scaling luminances by {:.6e}", weight_sum, lum_scale);
            for (uint32_t i = 0; i < entry_count; ++i) {
                block_luminances[i] *= lum_scale;
            }
        }
        const auto total_luminance = static_cast<float>(weight_sum * static_cast<double>(lum_scale));

        // --- Vose's alias table algorithm O(N) ---
        std::vector<renderer::EnvAliasEntry> table(entry_count);

        // Normalize weights so the average equals 1.0.
        const double avg = weight_sum / static_cast<double>(entry_count);
        std::vector<float> normalized(entry_count);
        for (uint32_t i = 0; i < entry_count; ++i) {
            normalized[i] = (avg > 0.0)
                                ? static_cast<float>(static_cast<double>(weights[i]) / avg)
                                : 1.0f;
        }

        // Partition into small (< 1) and large (>= 1) work lists.
        std::vector<uint32_t> small;
        std::vector<uint32_t> large;
        small.reserve(entry_count);
        large.reserve(entry_count);
        for (uint32_t i = 0; i < entry_count; ++i) {
            if (normalized[i] < 1.0f) {
                small.push_back(i);
            } else {
                large.push_back(i);
            }
        }

        // Pair small and large entries.
        while (!small.empty() && !large.empty()) {
            const uint32_t s = small.back();
            small.pop_back();
            const uint32_t l = large.back();
            large.pop_back();

            table[s].prob = normalized[s];
            table[s].alias = l;

            normalized[l] = (normalized[l] + normalized[s]) - 1.0f;

            if (normalized[l] < 1.0f) {
                small.push_back(l);
            } else {
                large.push_back(l);
            }
        }

        // Remaining entries get probability 1.0 (numerical cleanup).
        while (!large.empty()) {
            const uint32_t l = large.back();
            large.pop_back();
            table[l].prob = 1.0f;
            table[l].alias = l;
        }
        while (!small.empty()) {
            const uint32_t s = small.back();
            small.pop_back();
            table[s].prob = 1.0f;
            table[s].alias = s;
        }

        // Fill per-entry luminance (without sin_theta) for env_pdf().
        for (uint32_t i = 0; i < entry_count; ++i) {
            table[i].luminance = block_luminances[i];
        }

        if (factor > 1) {
            spdlog::info("Env alias table built: {}x{} -> {}x{} (factor {}, {} entries, "
                         "{:.1f} MB, total_lum={:.2f})",
                         width, height, ds_w, ds_h, factor, entry_count,
                         static_cast<double>(entry_count) * sizeof(renderer::EnvAliasEntry)
                             / (1024.0 * 1024.0),
                         total_luminance);
        } else {
            spdlog::info("Env alias table built: {}x{} ({} entries, {:.1f} MB, total_lum={:.2f})",
                         ds_w, ds_h, entry_count,
                         static_cast<double>(entry_count) * sizeof(renderer::EnvAliasEntry)
                             / (1024.0 * 1024.0),
                         total_luminance);
        }

        return {
            .entries = std::move(table),
            .total_luminance = total_luminance,
            .alias_width = ds_w,
            .alias_height = ds_h,
        };
    }

} // namespace qualquer::app
