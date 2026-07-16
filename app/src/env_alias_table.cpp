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

        const uint32_t entry_count = width * height;

        // --- Compute per-pixel luminance and sampling weights ---
        std::vector<float> weights(entry_count);
        std::vector<float> luminances(entry_count);
        const auto fh = static_cast<float>(height);
        double weight_sum = 0.0;

        for (uint32_t y = 0; y < height; ++y) {
            // sin(theta) at pixel center for equirectangular solid-angle correction.
            // theta = pi * (y + 0.5) / height maps [0, height) to (0, pi).
            const float theta = std::numbers::pi_v<float>
                                * (static_cast<float>(y) + 0.5f) / fh;
            const float sin_theta = std::sin(theta);

            for (uint32_t x = 0; x < width; ++x) {
                const uint32_t pixel = y * width + x;
                const uint32_t src = pixel * 3;
                const float r = rgb_data[src + 0];
                const float g = rgb_data[src + 1];
                const float b = rgb_data[src + 2];

                const float lum = 0.2126f * r + 0.7152f * g + 0.0722f * b;
                const float weight = lum * sin_theta;

                luminances[pixel] = lum;
                weights[pixel] = weight;
                weight_sum += static_cast<double>(weight);
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
                luminances[i] *= lum_scale;
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
            table[i].luminance = luminances[i];
        }

        spdlog::info("Env alias table built: {}x{} ({} entries, {:.1f} MB, total_lum={:.2f})",
                     width, height, entry_count,
                     static_cast<double>(entry_count) * sizeof(renderer::EnvAliasEntry)
                         / (1024.0 * 1024.0),
                     total_luminance);

        return {
            .entries = std::move(table),
            .total_luminance = total_luminance,
        };
    }

} // namespace qualquer::app
