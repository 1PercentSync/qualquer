#pragma once

/**
 * @file pt_common.cuh
 * @brief Path tracing shared utilities (renderer layer).
 */

#include <cstdint>

#include <cuda_runtime.h>

#include <qualquer/renderer/math_utils.cuh>

namespace qualquer::renderer {

// ---- PathState --------------------------------------------------------------

/**
 * @brief Per-path state for the raygen bounce loop.
 *
 * Raygen-local in Phase 4 (megakernel). Becomes a global buffer element type
 * when migrating to multi-launch.
 */
struct PathState {
    /** @brief Current ray origin (world space). */
    float3 origin;

    /** @brief Current ray direction (world space, normalized). */
    float3 direction;

    /** @brief Accumulated path throughput (product of BRDF weights). */
    float3 throughput;

    /** @brief Accumulated radiance (sum of weighted contributions). */
    float3 radiance;

    /** @brief Linear pixel index (y * width + x). */
    uint32_t pixel_index;

    /**
     * @brief Path sequence index for Sobol / xxhash (frame_index * spp + s).
     *
     * Independent of Separate Sum sample_count, which is zeroed under DLSS.
     */
    uint32_t sample_index;

    /** @brief Current bounce index (0-based). */
    uint32_t bounce;

    /** @brief False when the path has terminated (miss or invalid sample). */
    bool alive;
};

// ---- Ray origin offset (Wächter & Binder, Ray Tracing Gems Ch.6) -----------

// Three constants from Wächter & Binder (Ray Tracing Gems Ch.6 offset_ray):
//   int_scale   — integer bit-level push magnitude
//   float_scale — linear push step for near-origin coordinates
//   origin      — threshold below which the linear push is used
constexpr float kRayOriginIntScale   = 256.0f;
constexpr float kRayOriginFloatScale = 1.0f / 65536.0f;
constexpr float kRayOriginThreshold  = 1.0f / 32.0f;

/**
 * @brief Offsets a ray origin from a surface to prevent self-intersection.
 *
 * Uses integer bit manipulation on the float representation to push the
 * origin away from the surface along the geometric normal. Robust across
 * all floating-point scales without a scene-dependent epsilon.
 *
 * @param p     World-space hit position.
 * @param n_geo Geometric normal (not shading normal; must face the ray origin side).
 * @return Offset position safe for spawning secondary rays.
 */
__forceinline__ __device__ float3 offset_ray_origin(const float3 p,
                                                     const float3 n_geo) {
    const auto of_ix = static_cast<int32_t>(kRayOriginIntScale * n_geo.x);
    const auto of_iy = static_cast<int32_t>(kRayOriginIntScale * n_geo.y);
    const auto of_iz = static_cast<int32_t>(kRayOriginIntScale * n_geo.z);

    // Integer offset: add to the float bit representation (acts as ULP-scale push).
    const float p_ix = __int_as_float(__float_as_int(p.x) + (p.x < 0.0f ? -of_ix : of_ix));
    const float p_iy = __int_as_float(__float_as_int(p.y) + (p.y < 0.0f ? -of_iy : of_iy));
    const float p_iz = __int_as_float(__float_as_int(p.z) + (p.z < 0.0f ? -of_iz : of_iz));

    // Near the origin the integer offset underflows; use a linear push instead.
    return make_float3(
        fabsf(p.x) < kRayOriginThreshold ? p.x + kRayOriginFloatScale * n_geo.x : p_ix,
        fabsf(p.y) < kRayOriginThreshold ? p.y + kRayOriginFloatScale * n_geo.y : p_iy,
        fabsf(p.z) < kRayOriginThreshold ? p.z + kRayOriginFloatScale * n_geo.z : p_iz);
}

// ---- Shading normal consistency --------------------------------------------

/**
 * @brief Clamps a shading normal to the geometric normal hemisphere.
 *
 * Normal mapping can push the shading normal below the geometric surface,
 * causing light leaks. This reflects the shading normal about the geometric
 * normal when the two disagree in hemisphere.
 *
 * @param n_shading Shading normal (post-normal-map, normalized).
 * @param n_geo     Geometric face normal (normalized, post-back-face-flip).
 * @return Corrected shading normal on the same side as n_geo.
 */
__forceinline__ __device__ float3 ensure_normal_consistency(const float3 n_shading,
                                                            const float3 n_geo) {
    if (dot(n_shading, n_geo) < 0.0f) {
        return reflect(n_shading, n_geo);
    }
    return n_shading;
}

// ---- MIS power heuristic ---------------------------------------------------

/**
 * @brief Power heuristic for multiple importance sampling (beta = 2).
 *
 * Uses the ratio form 1/(1 + (b/a)²) instead of a²/(a² + b²) to avoid
 * overflow when PDFs are large (e.g. emissive solid-angle PDF with
 * dist²/cos at extreme geometry). The ratio r = b/a stays bounded or
 * overflows to Inf, and 1/(1+Inf) = 0 is the correct answer (pdf_a
 * negligible).
 *
 * @param pdf_a PDF of the strategy being weighted.
 * @param pdf_b PDF of the competing strategy.
 * @return MIS weight for strategy A in [0, 1].
 */
__forceinline__ __device__ float mis_power_heuristic(const float pdf_a,
                                                     const float pdf_b) {
    if (pdf_a <= 0.0f) {
        return 0.0f;
    }
    const float r = pdf_b / pdf_a;
    // Both Inf → r = NaN → guard to 0.5 (equal weight, symmetric default).
    if (!isfinite(r)) {
        return 0.5f;
    }
    return 1.0f / (1.0f + r * r);
}

// ---- Shadow terminator correction (Chiang et al. 2019) ---------------------

/**
 * @brief Smooth geometry factor to eliminate hard shadow-terminator artifacts.
 *
 * Low-poly meshes with smooth shading normals create a mismatch between the
 * geometric and shading hemispheres. Near the geometric terminator (where
 * N_geo·L ≈ 0 but N_shading·L > 0), the shadow ray origin sits close to the
 * geometric surface and gets self-occluded, producing hard shadow edges.
 *
 * This factor smoothly attenuates the NEE contribution as the geometric
 * cosine drops below the shading cosine, eliminating the hard cutoff.
 *
 * @param N_geo     Geometric (face) normal (normalized).
 * @param N_shading Shading normal (post-normal-map, normalized).
 * @param L         Light direction (normalized, facing away from surface).
 * @return Correction factor in [0, 1]; multiply into the NEE contribution.
 */
__forceinline__ __device__ float shadow_terminator_factor(const float3 N_geo,
                                                          const float3 N_shading,
                                                          const float3 L) {
    const float NgdotL = dot(N_geo, L);
    const float NdotL  = dot(N_shading, L);

    // Geometric normal sees no light at all.
    if (NgdotL <= 0.0f) {
        return 0.0f;
    }
    // Geometric agrees with or exceeds shading — no correction needed.
    if (NgdotL >= NdotL) {
        return 1.0f;
    }

    // ensure_normal_consistency references N_face, guaranteeing
    // dot(N_face, N_shading) >= 0 by construction.
    const float NgdotNs = dot(N_geo, N_shading);

    // Chiang 2019 Eq. 1: min(1, ratio) before Hermite smoothing.
    const float G = fminf(NgdotL / (NdotL * NgdotNs), 1.0f);

    // Smooth polynomial: G + G² − G³ = G(1 + G(1 − G)).
    // Monotonic 0→1, derivative at G=1 is 0 (smooth arrival).
    return G + G * G - G * G * G;
}

// ---- Firefly clamp ----------------------------------------------------------

/**
 * @brief Scales per-sample radiance so average RGB luminance does not exceed
 *        @p max_clamp (vk_gltf_renderer-style luminance clamp).
 *
 * Hue is preserved by uniform scale. @p max_clamp <= 0 disables clamping.
 *
 * @param radiance  Path sample radiance.
 * @param max_clamp Luminance threshold; 0 or negative leaves radiance unchanged.
 * @return Clamped radiance.
 */
__forceinline__ __device__ float3 apply_firefly_clamp(const float3 radiance,
                                                     const float max_clamp) {
    if (max_clamp <= 0.0f) {
        return radiance;
    }
    // Equal-weight luminance matches vk_gltf_renderer (not Rec.709).
    const float lum = (radiance.x + radiance.y + radiance.z) * (1.0f / 3.0f);
    if (lum > max_clamp) {
        return radiance * (max_clamp / lum);
    }
    return radiance;
}

} // namespace qualquer::renderer
