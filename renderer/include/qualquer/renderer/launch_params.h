#pragma once

/**
 * @file launch_params.h
 * @brief OptiX launch parameters (renderer layer).
 */

#include <cstdint>

#include <cuda_runtime.h>
#include <optix_types.h>

namespace qualquer::renderer {

struct GPUGeometryInfo;
struct Material;

/**
 * @brief Simple 4x4 float matrix for host-device data transfer.
 *
 * CUDA has no built-in 4x4 matrix type. Host code converts from glm::mat4
 * when filling LaunchParams; device code reads rows directly.
 */
struct float4x4 {
    /** @brief Row-major storage: rows[i] is the i-th row of the matrix. */
    float4 rows[4];
};

/**
 * @brief Alias-table entry for emissive-triangle NEE (Vose's algorithm).
 *
 * 8 bytes per entry; one entry per emissive triangle.
 */
struct AliasEntry {
    /** @brief Acceptance probability of the drawn index, in [0,1]. */
    float prob;

    /** @brief Alias index used when the draw is rejected. */
    uint32_t alias;
};

static_assert(sizeof(AliasEntry) == 8);

/**
 * @brief Alias-table entry for environment-map NEE.
 *
 * 12 bytes per entry. Stores per-pixel luminance so env_pdf() evaluates the
 * sampled direction's PDF without a separate luminance lookup texture.
 */
struct EnvAliasEntry {
    /** @brief Acceptance probability of the drawn index, in [0,1]. */
    float prob;

    /** @brief Alias index used when the draw is rejected. */
    uint32_t alias;

    /** @brief Per-pixel luminance, excluding the sin(theta) projection weight. */
    float luminance;
};

static_assert(sizeof(EnvAliasEntry) == 12);

/**
 * @brief World-space emissive triangle for NEE sampling.
 *
 * 96 bytes (16-byte aligned for vertex vector loads across array elements).
 */
struct EmissiveTriangle {
    /** @brief First vertex position (world space). */
    float3 v0;
    float _pad0; ///< 16-byte alignment padding.
    /** @brief Second vertex position (world space). */
    float3 v1;
    float _pad1; ///< 16-byte alignment padding.
    /** @brief Third vertex position (world space). */
    float3 v2;
    float _pad2; ///< 16-byte alignment padding.

    /** @brief Emission color (material emissive_factor constant, not a texture sample). */
    float3 emission;

    /** @brief Material index; NEE samples the emissive texture via the interpolated UV. */
    uint32_t material_index;

    /** @brief First vertex texture coordinates. */
    float2 uv0;
    /** @brief Second vertex texture coordinates. */
    float2 uv1;
    /** @brief Third vertex texture coordinates. */
    float2 uv2;

    uint32_t _pad3; ///< Pads to 16-byte aligned struct size (96 = 16 × 6).
    uint32_t _pad4;
};

static_assert(sizeof(EmissiveTriangle) == 96);

/**
 * @brief Per-frame parameters uploaded to the OptiX device constant buffer.
 *
 * Shared between host (renderer.cpp) and device (programs.cu): the same
 * definition is included on both sides and must stay layout-compatible.
 */
struct LaunchParams {
    /** @brief Device pointer to the accumulation buffer written by raygen (new Separate-Sum total). */
    float4 *accumulation_buffer;

    /**
     * @brief Device pointer to the previous accumulation total (Separate-Sum read side).
     *
     * raygen reads this, adds the current frame's contribution, and writes the
     * new total to accumulation_buffer. Points to the opposite ping-pong slot.
     */
    const float4 *accumulation_buffer_read;

    /** @brief Render resolution width in pixels (raygen launch X dimension). */
    uint32_t width;

    /** @brief Render resolution height in pixels (raygen launch Y dimension). */
    uint32_t height;

    /** @brief Monotonic frame counter; uploaded for device-side temporal variation (e.g. RNG seed). */
    uint32_t frame_index;

    /** @brief TLAS handle for optixTrace traversal. */
    OptixTraversableHandle traversable;

    /** @brief Device pointer to the per-geometry RT query data array. */
    const GPUGeometryInfo *geometry_infos;

    /** @brief Device pointer to the material array. */
    const Material *materials;

    /** @brief Device pointer to the cudaTextureObject_t array. */
    const cudaTextureObject_t *texture_objects;

    /** @brief Inverse view matrix (camera world position and orientation). */
    float4x4 inv_view;

    /** @brief Inverse projection matrix (NDC to view-space unprojection). */
    float4x4 inv_projection;

    // ---- Path tracing ----

    /** @brief Maximum bounce depth per path. */
    uint32_t max_bounces;

    /** @brief Samples traced per frame (raygen sample-loop iteration count). */
    uint32_t samples_per_frame;

    /**
     * @brief Samples accumulated in the read buffer (chain count).
     *
     * 0 signals raygen to overwrite the write buffer directly (first sample
     * after reset/init) instead of accumulating from the read buffer.
     */
    uint32_t sample_count;

    /**
     * @brief Sine of the IBL Y-axis rotation angle.
     *
     * The rotation angle is a launch constant, so the host precomputes the
     * sin/cos pair once per frame. Device code rotates ray directions for
     * cubemap lookup and NEE without a per-hit sincosf (whose Payne-Hanek
     * argument reduction drags in double-precision math).
     */
    float env_rotation_sin;

    /** @brief Cosine of the IBL Y-axis rotation angle (see env_rotation_sin). */
    float env_rotation_cos;

    // ---- Environment light ----

    /** @brief Cubemap texture sampled on miss and for environment NEE. */
    cudaTextureObject_t env_cubemap;

    /** @brief Device alias table over the equirect (one entry per pixel). */
    const EnvAliasEntry *env_alias_table;

    /** @brief Number of entries in env_alias_table. */
    uint32_t env_alias_count;

    /** @brief Alias-table width (equirect width). */
    uint32_t env_alias_width;

    /** @brief Alias-table height (equirect height). */
    uint32_t env_alias_height;

    /** @brief Total luminance across the environment map (alias-table normalization). */
    float env_total_luminance;

    // ---- Emissive triangles ----

    /** @brief Device array of emissive triangles. */
    const EmissiveTriangle *emissive_triangles;

    /** @brief Alias table over emissive triangles (one entry per triangle). */
    const AliasEntry *emissive_alias_table;

    /** @brief Number of emissive triangles. */
    uint32_t emissive_count;

    /** @brief Total radiant power across all emissive triangles. */
    float emissive_total_power;

    // ---- Sobol RNG ----

    /**
     * @brief 128-dimension Sobol direction numbers in __constant__ memory.
     *
     * Embedded in LaunchParams so optixLaunch places them in __constant__ memory,
     * enabling L1 constant cache broadcast for warp-uniform reads (~8 cycles vs
     * ~200+ cycles through L2 for global memory pointers).
     *
     * Layout: sobol_directions[dim * 32 + bit], dim in [0,127], bit in [0,31].
     * Left-justified 32-bit values from Joe & Kuo. Initialized once from
     * kSobolDirectionData; unchanged across frames.
     */
    uint32_t sobol_directions[4096];
};

} // namespace qualquer::renderer
