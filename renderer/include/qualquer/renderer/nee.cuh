#pragma once

/**
 * @file nee.cuh
 * @brief Next Event Estimation helpers for environment and emissive NEE (renderer layer).
 */

#include <cstdint>

#include <cuda_runtime.h>
#include <optix_device.h>

#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/math_utils.cuh>

namespace qualquer::renderer {

// ---- Environment alias table sampling ---------------------------------------

/**
 * @brief Samples a world-space direction from the environment alias table.
 *
 * O(1) importance sampling proportional to luminance × sin(theta). Sub-pixel
 * jitter (r3, r4) within the selected pixel eliminates aliasing at pixel
 * boundaries. The alias table is built in env space (unrotated equirect);
 * the returned direction is transformed to world space by the inverse IBL
 * rotation.
 *
 * @param alias_table   Device alias table entries (one per equirect pixel).
 * @param entry_count   Total entries (width × height).
 * @param width         Equirect source width.
 * @param height        Equirect source height.
 * @param env_rotation  IBL Y-axis rotation in radians.
 * @param r1            Uniform random [0,1) — bin selection.
 * @param r2            Uniform random [0,1) — accept/reject.
 * @param r3            Uniform random [0,1) — horizontal sub-pixel jitter.
 * @param r4            Uniform random [0,1) — vertical sub-pixel jitter.
 * @return Sampled world-space direction (normalized).
 */
__forceinline__ __device__ float3 sample_env_alias_table(
        const EnvAliasEntry *alias_table,
        const uint32_t entry_count,
        const uint32_t width, const uint32_t height,
        const float env_rotation,
        const float r1, const float r2, const float r3, const float r4) {

    if (entry_count == 0) {
        return make_float3(0.0f, 1.0f, 0.0f);
    }

    const uint32_t idx = min(static_cast<uint32_t>(r1 * static_cast<float>(entry_count)),
                             entry_count - 1);
    const EnvAliasEntry &e = alias_table[idx];
    const uint32_t pixel = (r2 < e.prob) ? idx : e.alias;

    // Pixel → equirect UV (jittered within pixel).
    const uint32_t px = pixel % width;
    const uint32_t py = pixel / width;
    const float u = (static_cast<float>(px) + r3) / static_cast<float>(width);
    const float v = (static_cast<float>(py) + r4) / static_cast<float>(height);

    // Equirect UV → direction in env space (matches equirect_to_cubemap.cu convention).
    const float phi   = (u - 0.5f) * kTwoPi;
    const float theta = (0.5f - v) * kPi;
    const float cos_theta = __cosf(theta);

    const float3 env_dir = make_float3(cos_theta * __cosf(phi),
                                       __sinf(theta),
                                       cos_theta * __sinf(phi));

    // Inverse IBL rotation: env space → world space.
    float sin_r, cos_r;
    sincosf(env_rotation, &sin_r, &cos_r);
    return rotate_y_dir(env_dir, -sin_r, cos_r);
}

/**
 * @brief Solid-angle PDF of a direction under the environment alias table distribution.
 *
 * Rotates the world-space direction to env space (forward IBL rotation), converts
 * to equirect UV, looks up the stored per-pixel luminance, and converts to a
 * solid-angle PDF. Using the stored luminance (same values that built the alias
 * table) ensures exact PDF/sampling consistency.
 *
 * pdf = luminance × W × H / (total_luminance × 2π²)
 *
 * @param alias_table      Device alias table entries.
 * @param width            Equirect source width.
 * @param height           Equirect source height.
 * @param total_luminance  Sum of all pixel weights (luminance × sin_theta).
 * @param env_rotation     IBL Y-axis rotation in radians.
 * @param dir              World-space direction (normalized).
 * @return Solid-angle PDF (>= 1e-7).
 */
__forceinline__ __device__ float env_pdf(
        const EnvAliasEntry *alias_table,
        const uint32_t width, const uint32_t height,
        const float total_luminance,
        const float env_rotation,
        const float3 dir) {

    if (total_luminance <= 0.0f) {
        return 1e-7f;
    }

    // Forward IBL rotation: world space → env space.
    float sin_r, cos_r;
    sincosf(env_rotation, &sin_r, &cos_r);
    const float3 env_dir = rotate_y_dir(dir, sin_r, cos_r);

    // Direction → equirect UV (inverse of the sampling mapping).
    const float phi   = atan2f(env_dir.z, env_dir.x);
    const float theta = asinf(fminf(1.0f, fmaxf(-1.0f, env_dir.y)));
    const float u = phi / kTwoPi + 0.5f;
    const float v = 0.5f - theta * kInvPi;

    // UV → nearest pixel index.
    const uint32_t px = min(static_cast<uint32_t>(u * static_cast<float>(width)),  width  - 1);
    const uint32_t py = min(static_cast<uint32_t>(v * static_cast<float>(height)), height - 1);
    const uint32_t pixel = py * width + px;

    const float lum = alias_table[pixel].luminance;

    return fmaxf(lum * static_cast<float>(width) * static_cast<float>(height)
                 / (total_luminance * kTwoPi * kPi), 1e-7f);
}

// ---- Shadow ray tracing -----------------------------------------------------

/**
 * @brief Traces a shadow ray and returns visibility (1 = visible, 0 = occluded).
 *
 * Uses TERMINATE_ON_FIRST_HIT | DISABLE_CLOSESTHIT for maximum efficiency.
 * missIndex=1 invokes __miss__shadow which sets payload_0 = 1.
 *
 * @param traversable TLAS handle.
 * @param origin      Ray origin (offset from surface).
 * @param direction   Ray direction (normalized, toward light).
 * @param tmax        Maximum ray distance (1e16 for env, dist*(1-eps) for area).
 * @return 1 if the light is visible, 0 if occluded.
 */
__forceinline__ __device__ uint32_t trace_shadow_ray(
        const OptixTraversableHandle traversable,
        const float3 origin, const float3 direction,
        const float tmax) {

    uint32_t visible = 0;
    optixTrace(traversable,
               origin, direction,
               0.0f,   // tmin
               tmax,
               0.0f,   // rayTime
               0xFF,   // visibilityMask
               OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT
                   | OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT,
               0,      // SBT offset
               0,      // SBT stride
               1,      // missIndex (shadow miss)
               visible);
    return visible;
}

// ---- Emissive triangle sampling ---------------------------------------------

/**
 * @brief Uniform barycentric coordinates on a triangle (Turk 1990).
 *
 * @param r1 Uniform random [0,1).
 * @param r2 Uniform random [0,1).
 * @return Barycentric weights (u, v, w) with u + v + w = 1.
 */
__forceinline__ __device__ float3 triangle_barycentric(const float r1, const float r2) {
    const float sqrt_r1 = sqrtf(r1);
    const float u = 1.0f - sqrt_r1;
    const float v = r2 * sqrt_r1;
    return make_float3(u, v, 1.0f - u - v);
}

/**
 * @brief Selects an emissive triangle from the power-weighted alias table.
 *
 * @param alias_table Device alias table entries (one per emissive triangle).
 * @param count       Number of emissive triangles.
 * @param r1          Uniform random [0,1) — bin selection.
 * @param r2          Uniform random [0,1) — accept/reject.
 * @return Index of the selected emissive triangle.
 */
__forceinline__ __device__ uint32_t sample_emissive_alias_table(
        const AliasEntry *alias_table, const uint32_t count,
        const float r1, const float r2) {

    const uint32_t idx = min(static_cast<uint32_t>(r1 * static_cast<float>(count)),
                             count - 1);
    const AliasEntry &e = alias_table[idx];
    return (r2 < e.prob) ? idx : e.alias;
}

/**
 * @brief Solid-angle PDF of sampling a specific emissive triangle.
 *
 * The alias table selects triangle i with probability:
 *   P(i) = power_i / total_power = luminance(emission_i) × area_i / total_power
 * Combined with uniform point sampling (1 / area_i), the area-measure PDF is:
 *   pdf_area = luminance(emission_i) / total_power
 * Converting to solid-angle measure:
 *   pdf_omega = pdf_area × dist² / |cos_theta_light|
 *
 * @param emission_luminance luminance(emissive_factor) of the triangle.
 * @param dist               Distance from shading point to light sample point.
 * @param cos_theta_light    |dot(light_normal, dir_to_shading_point)|.
 * @param total_power        Total power sum across all emissive triangles.
 * @return Solid-angle PDF (>= 1e-7).
 */
__forceinline__ __device__ float emissive_light_pdf(
        const float emission_luminance, const float dist,
        const float cos_theta_light, const float total_power) {

    if (total_power <= 0.0f || cos_theta_light <= 0.0f) {
        return 1e-7f;
    }
    const float pdf_area = emission_luminance / total_power;
    return fmaxf(pdf_area * dist * dist / cos_theta_light, 1e-7f);
}

} // namespace qualquer::renderer
