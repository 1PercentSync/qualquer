#pragma once

/**
 * @file nee.cuh
 * @brief Next Event Estimation helpers for environment and emissive NEE (renderer layer).
 */

#include <cstdint>

#include <cuda_runtime.h>
#include <optix_device.h>

#include <qualquer/renderer/brdf.cuh>
#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/math_utils.cuh>
#include <qualquer/renderer/pt_common.cuh>
#include <qualquer/renderer/rng.cuh>

// Defined in programs.cu; NEE evaluators read scene/light resources from it.
extern "C" {
extern __constant__ qualquer::renderer::LaunchParams params;
}

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
 * Returns world_dir, env_dir (for cubemap lookup), and solid-angle PDF
 * directly from the selected pixel's luminance, avoiding the reverse
 * mapping (atan2/asin) that env_pdf would need to rediscover the pixel.
 */
struct EnvSample {
    float3 world_dir; ///< Sampled direction in world space (normalized).
    float3 env_dir;   ///< Sampled direction in env space (for cubemap lookup).
    float  pdf;       ///< Solid-angle PDF of this sample.
};

/**
 * @brief Alias-table importance sampling of the environment map.
 *
 * @param env  Packed env light data (alias table, dimensions, rotation).
 * @param r1   Uniform random [0,1) — bin selection.
 * @param r2   Uniform random [0,1) — accept/reject.
 * @param r3   Uniform random [0,1) — horizontal sub-pixel jitter.
 * @param r4   Uniform random [0,1) — vertical sub-pixel jitter.
 * @return EnvSample with world_dir, env_dir, and PDF.
 */
__forceinline__ __device__ EnvSample sample_env_alias_table(
        const EnvLightData &env,
        const float r1, const float r2, const float r3, const float r4) {

    if (env.alias_count == 0) {
        return {make_float3(0.0f, 1.0f, 0.0f),
                make_float3(0.0f, 1.0f, 0.0f), 0.0f};
    }

    const uint32_t idx = min(static_cast<uint32_t>(r1 * static_cast<float>(env.alias_count)),
                             env.alias_count - 1);
    const EnvAliasEntry &e = env.alias_table[idx];
    const uint32_t pixel = (r2 < e.prob) ? idx : e.alias;

    // Pixel → equirect UV (jittered within pixel).
    const uint32_t px = pixel % env.alias_width;
    const uint32_t py = pixel / env.alias_width;
    const float u = (static_cast<float>(px) + r3) / static_cast<float>(env.alias_width);
    const float v = (static_cast<float>(py) + r4) / static_cast<float>(env.alias_height);

    // Equirect UV → direction in env space (matches equirect_to_cubemap.cu convention).
    const float phi   = (u - 0.5f) * kTwoPi;
    const float theta = (0.5f - v) * kPi;
    float sin_theta, cos_theta;
    __sincosf(theta, &sin_theta, &cos_theta);
    float sin_phi, cos_phi;
    __sincosf(phi, &sin_phi, &cos_phi);

    const float3 env_dir = make_float3(cos_theta * cos_phi,
                                       sin_theta,
                                       cos_theta * sin_phi);

    // PDF from the selected pixel's stored luminance (same value that built
    // the alias table). Avoids the atan2/asin inverse mapping of env_pdf.
    const float lum = env.alias_table[pixel].luminance;
    const float pdf = lum * static_cast<float>(env.alias_width)
                          * static_cast<float>(env.alias_height)
                      / (env.total_luminance * kTwoPi * kPi);

    // Inverse IBL rotation: env space → world space.
    const float3 world_dir = rotate_y_dir(env_dir, -env.rotation_sin, env.rotation_cos);
    return {world_dir, env_dir, pdf};
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
 * @param env  Packed env light data (alias table, dimensions, total luminance, rotation).
 * @param dir  World-space direction (normalized).
 * @return Solid-angle PDF (>= 0). Zero when the pixel has zero luminance
 *         or the environment is absent.
 */
__forceinline__ __device__ float env_pdf(
        const EnvLightData &env,
        const float3 dir) {

    if (env.total_luminance <= 0.0f) {
        return 0.0f;
    }

    // Forward IBL rotation: world space → env space.
    const float3 env_dir = rotate_y_dir(dir, env.rotation_sin, env.rotation_cos);

    // Direction → equirect UV (inverse of the sampling mapping).
    const float phi   = atan2f(env_dir.z, env_dir.x);
    const float theta = asinf(fminf(1.0f, fmaxf(-1.0f, env_dir.y)));
    const float u = phi / kTwoPi + 0.5f;
    const float v = 0.5f - theta * kInvPi;

    // UV → nearest pixel index.
    const uint32_t px = min(static_cast<uint32_t>(u * static_cast<float>(env.alias_width)),
                            env.alias_width - 1);
    const uint32_t py = min(static_cast<uint32_t>(v * static_cast<float>(env.alias_height)),
                            env.alias_height - 1);
    const uint32_t pixel = py * env.alias_width + px;

    const float lum = env.alias_table[pixel].luminance;

    return lum * static_cast<float>(env.alias_width) * static_cast<float>(env.alias_height)
           / (env.total_luminance * kTwoPi * kPi);
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
 * @return Solid-angle PDF. Returns +Inf when cos_theta_light <= 0
 *         (edge-on triangle subtends near-zero solid angle → PDF → ∞).
 */
__forceinline__ __device__ float emissive_light_pdf(
        const float emission_luminance, const float dist,
        const float cos_theta_light, const float total_power) {

    if (total_power <= 0.0f) {
        return 0.0f;
    }
    if (cos_theta_light <= 0.0f) {
        return kPosInf;
    }
    const float pdf_area = emission_luminance / total_power;
    return pdf_area * dist * dist / cos_theta_light;
}

// ---- Full NEE evaluations (env / emissive) ----------------------------------
//
// Env and emissive stay as two functions: their resource parameter sets differ
// enough that a single entry would force a dishonest or bloated signature.
// Both read scene resources from the programs.cu LaunchParams constant and take
// only shading-point inputs as explicit arguments.

/**
 * @brief Environment NEE: alias-sample a direction, shadow-test, BRDF eval, MIS.
 *
 * @param bp           Shading-point BRDF parameters (from init_brdf_params).
 * @param offset_pos   Ray origin offset from the hit surface.
 * @param N_face       Geometric face normal (for shadow-terminator correction).
 * @param N_shading    Shading normal (must match bp.N).
 * @param pixel_index  Linear pixel index for RNG.
 * @param sample_index Cumulative sample index for RNG.
 * @param dim_base     Per-bounce RNG dimension base (from bounce_dim_base).
 * @return NEE radiance contribution (not yet multiplied by path throughput).
 */
__forceinline__ __device__ float3 evaluate_env_nee(
        const BrdfParams &bp,
        const float3 offset_pos,
        const float3 N_face,
        const float3 N_shading,
        const uint32_t pixel_index,
        const uint32_t sample_index,
        const uint32_t dim_base) {

    if (params.env.cubemap == 0 || params.env.alias_table == nullptr
        || params.env.total_luminance <= 0.0f) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    const float env_r1 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEnvNee);
    const float env_r2 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEnvNee + 1);
    const float env_r3 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEnvNee + 2);
    const float env_r4 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEnvNee + 3);

    const EnvSample es = sample_env_alias_table(
        params.env, env_r1, env_r2, env_r3, env_r4);
    const float3 L = es.world_dir;

    if (es.pdf <= 0.0f || !isfinite(es.pdf)) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    // Geometric hemisphere reject: shadow_terminator_factor returns 0 when
    // NgdotL <= 0, so reject before the shadow ray and BRDF work.
    if (dot(N_face, L) <= 0.0f) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    const float NdotL = dot(N_shading, L);
    if (NdotL <= 0.0f) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    const uint32_t visible = trace_shadow_ray(
        params.traversable, offset_pos, L, 1e16f);
    if (!visible) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    // Cubemap lookup uses env_dir directly (no second rotation).
    const auto env_texel = texCubemap<float4>(
        params.env.cubemap, es.env_dir.x, es.env_dir.y, es.env_dir.z);
    const float3 env_color = make_float3(env_texel.x, env_texel.y, env_texel.z);

    const BrdfEvalResult bep = brdf_eval_and_pdf(bp, L, NdotL);

    const float mis_w = mis_power_heuristic(es.pdf, bep.pdf);
    const float st_factor = shadow_terminator_factor(N_face, N_shading, L);

    return env_color * bep.value * NdotL * mis_w * st_factor / es.pdf;
}

/**
 * @brief Emissive-triangle NEE: alias-sample a triangle, shadow-test, BRDF eval, MIS.
 *
 * @param bp           Shading-point BRDF parameters (from init_brdf_params).
 * @param offset_pos   Ray origin offset from the hit surface.
 * @param N_face       Geometric face normal (for shadow-terminator correction).
 * @param N_shading    Shading normal (must match bp.N).
 * @param pixel_index  Linear pixel index for RNG.
 * @param sample_index Cumulative sample index for RNG.
 * @param dim_base     Per-bounce RNG dimension base (from bounce_dim_base).
 * @return NEE radiance contribution (not yet multiplied by path throughput).
 */
__forceinline__ __device__ float3 evaluate_emissive_nee(
        const BrdfParams &bp,
        const float3 offset_pos,
        const float3 N_face,
        const float3 N_shading,
        const uint32_t pixel_index,
        const uint32_t sample_index,
        const uint32_t dim_base) {

    if (params.emissive.count == 0 || params.emissive.triangles == nullptr) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    const float emi_r1 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEmissiveNee);
    const float emi_r2 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEmissiveNee + 1);
    const float emi_r3 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEmissiveNee + 2);
    const float emi_r4 = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index,
                                   dim_base + kBounceOffsetEmissiveNee + 3);

    const uint32_t tri_idx = sample_emissive_alias_table(
        params.emissive.alias_table, params.emissive.count, emi_r1, emi_r2);
    const EmissiveTriangle &tri = params.emissive.triangles[tri_idx];

    // Barycentric point on triangle: v0 + edge1 * b1 + edge2 * b2.
    // triangle_barycentric returns (b0, b1, b2) with b0 = 1 - b1 - b2;
    // the v0 term absorbs b0 via v0*b0 + (v0+e1)*b1 + (v0+e2)*b2 = v0 + e1*b1 + e2*b2.
    const float3 light_bary = triangle_barycentric(emi_r3, emi_r4);
    const float3 light_pos = tri.v0 + tri.edge1 * light_bary.y
                           + tri.edge2 * light_bary.z;

    const float3 to_light = light_pos - offset_pos;
    const float dist2 = dot(to_light, to_light);

    // Guard against zero or underflowed distance (overlapping geometry,
    // precision collapse). 1/0 → Inf → L = (0,0,0)*Inf = NaN.
    if (dist2 <= 0.0f) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    const float dist = sqrtf(dist2);
    const float3 L = to_light * (1.0f / dist);

    const float3 light_normal = make_float3(tri.normal_x, tri.normal_y, tri.normal_z);
    float cos_theta_light = dot(light_normal, -L);

    // Double-sided: take absolute cosine so both faces are treated as
    // front-facing (light emits from both sides of the triangle).
    if (tri.double_sided == 1u) {
        cos_theta_light = fabsf(cos_theta_light);
    }
    if (cos_theta_light <= 0.0f) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    // Geometric hemisphere reject: shadow_terminator_factor returns 0 when
    // NgdotL <= 0, so reject before the shadow ray and BRDF work.
    if (dot(N_face, L) <= 0.0f) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    const float NdotL_emi = dot(N_shading, L);
    if (NdotL_emi <= 0.0f) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    // tMax shortened to avoid hitting the target triangle itself.
    const uint32_t visible = trace_shadow_ray(
        params.traversable, offset_pos, L, dist * (1.0f - 1e-4f));
    if (!visible) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    // UV interpolation: uv0 + (uv1-uv0)*b1 + (uv2-uv0)*b2 (same edge form as position).
    const float2 light_uv = make_float2(
        tri.uv0.x + (tri.uv1.x - tri.uv0.x) * light_bary.y
                   + (tri.uv2.x - tri.uv0.x) * light_bary.z,
        tri.uv0.y + (tri.uv1.y - tri.uv0.y) * light_bary.y
                   + (tri.uv2.y - tri.uv0.y) * light_bary.z);
    const auto le_texel = tex2D<float4>(
        params.texture_objects[tri.emissive_tex], light_uv.x, light_uv.y);
    const float3 Le = make_float3(
        le_texel.x * tri.emission.x,
        le_texel.y * tri.emission.y,
        le_texel.z * tri.emission.z);

    const BrdfEvalResult bep_emi = brdf_eval_and_pdf(bp, L, NdotL_emi);

    const float emission_lum = luminance(tri.emission);
    const float light_pdf_emi = emissive_light_pdf(
        emission_lum, dist, cos_theta_light, params.emissive.total_power);
    if (light_pdf_emi <= 0.0f || !isfinite(light_pdf_emi)) {
        return make_float3(0.0f, 0.0f, 0.0f);
    }

    const float mis_w_emi = mis_power_heuristic(light_pdf_emi, bep_emi.pdf);
    const float st_factor_emi = shadow_terminator_factor(N_face, N_shading, L);

    return Le * bep_emi.value * NdotL_emi * mis_w_emi * st_factor_emi
        / light_pdf_emi;
}

} // namespace qualquer::renderer
