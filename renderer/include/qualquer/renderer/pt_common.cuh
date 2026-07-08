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

    /** @brief Cumulative sample index for RNG dimension scrambling. */
    uint32_t sample_index;

    /** @brief Current bounce index (0-based). */
    uint32_t bounce;

    /** @brief False when the path has terminated (miss or invalid sample). */
    bool alive;
};

// ---- Ray origin offset (Wächter & Binder, Ray Tracing Gems Ch.6) -----------

// Integer-scale push for large |p| values; linear push for near-origin values.
constexpr float kRayOriginIntScale   = 256.0f;
constexpr float kRayOriginFloatScale = 1.0f / 65536.0f;

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
        fabsf(p.x) < kRayOriginFloatScale ? p.x + kRayOriginFloatScale * n_geo.x : p_ix,
        fabsf(p.y) < kRayOriginFloatScale ? p.y + kRayOriginFloatScale * n_geo.y : p_iy,
        fabsf(p.z) < kRayOriginFloatScale ? p.z + kRayOriginFloatScale * n_geo.z : p_iz);
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
 * @param n_geo     Geometric normal (interpolated vertex normal, normalized,
 *                  post-back-face-flip).
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
 * @param pdf_a PDF of the strategy being weighted.
 * @param pdf_b PDF of the competing strategy.
 * @return MIS weight for strategy A in [0, 1].
 */
__forceinline__ __device__ float mis_power_heuristic(const float pdf_a,
                                                     const float pdf_b) {
    const float a2 = pdf_a * pdf_a;
    return a2 / (a2 + pdf_b * pdf_b);
}

} // namespace qualquer::renderer
