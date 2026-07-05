#pragma once

/**
 * @file tonemap.cuh
 * @brief HDR to LDR tone mapping helpers (renderer layer).
 */

#include <cuda_runtime.h>
#include <cmath> // IDE resolver needs explicit fminf/fmaxf decl; nvcc maps to device intrinsics regardless

#include <qualquer/renderer/math_utils.cuh>

namespace qualquer::renderer {

/**
 * @brief Khronos PBR Neutral tone mapping curve.
 *
 * Ported from the Khronos sample implementation (pbrNeutral.glsl). Designed for
 * PBR rendering: colors below the compression threshold pass through 1:1, so
 * base colors render faithfully; only highlights are compressed and slightly
 * desaturated, avoiding hue shifts. Piecewise in three regions:
 *   - toe (min channel < 0.08): quadratic lift to restore shadow detail
 *   - linear 1:1: identity for mid tones
 *   - compression (peak > startCompression): rational highlight roll-off plus
 *     desaturation toward the new peak
 *
 * @param color Linear HDR color, non-negative, Rec. 709.
 * @return Linear LDR color in [0, 1], Rec. 709.
 */
__forceinline__ __device__ float3 pbr_neutral_tonemap(float3 color) {
    // startCompression = 0.8 - F90 (F90 = 0.04 for IoR 1.5); desaturation controls
    // how fast highlights tend toward white.
    constexpr float kStartCompression = 0.76f; // 0.8 - 0.04
    constexpr float kDesaturation = 0.15f;

    const float x = fminf(color.x, fminf(color.y, color.z));
    const float offset = x < 0.08f ? x - 6.25f * x * x : 0.04f;
    color = color - offset;

    const float peak = fmaxf(color.x, fmaxf(color.y, color.z));
    if (peak < kStartCompression) {
        return color;
    }

    // Rational highlight compression: newPeak rises toward 1 as peak grows.
    constexpr float d = 1.0f - kStartCompression; // 0.24
    const float new_peak = 1.0f - d * d / (peak + d - kStartCompression);
    color = color * (new_peak / peak);

    // Desaturate toward the compressed peak as highlights blow out.
    const float g = 1.0f - 1.0f / (kDesaturation * (peak - new_peak) + 1.0f);
    return color * (1.0f - g) + make_float3(new_peak, new_peak, new_peak) * g;
}

/**
 * @brief Applies exposure then the Khronos PBR Neutral tone mapper.
 *
 * Exposure is a linear color multiplier (the app converts EV to this multiplier
 * via pow(2, ev) before upload). It is applied before tonemapping so the tone
 * curve compresses the exposed highlights.
 *
 * @param color    Linear HDR color (non-negative, Rec. 709).
 * @param exposure Linear multiplier applied to color before tonemapping.
 * @return Linear LDR color in [0, 1].
 */
__forceinline__ __device__ float3 apply_tonemap(float3 color, const float exposure) {
    color = color * exposure;
    return pbr_neutral_tonemap(color);
}

} // namespace qualquer::renderer
