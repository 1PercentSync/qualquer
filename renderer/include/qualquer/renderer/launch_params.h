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
 * 12 bytes per entry. Stores per-cell sampling weight (luminance × sin(colatitude))
 * so env_pdf() evaluates the solid-angle PDF without recomputing the cell's latitude.
 */
struct EnvAliasEntry {
    /** @brief Acceptance probability of the drawn index, in [0,1]. */
    float prob;

    /** @brief Alias index used when the draw is rejected. */
    uint32_t alias;

    /** @brief Per-cell sampling weight: luminance × sin(colatitude_center). */
    float luminance;
};

static_assert(sizeof(EnvAliasEntry) == 12);

/**
 * @brief World-space emissive triangle for NEE sampling.
 *
 * 96 bytes (16-byte aligned for vertex vector loads across array elements).
 * Stores precomputed edges, face normal, and material properties so the
 * device NEE evaluator avoids recomputing edges/cross/normalize and
 * eliminates a random Material array lookup per sample.
 */
struct EmissiveTriangle {
    /** @brief First vertex position (world space). */
    float3 v0;
    /** @brief X component of the precomputed face normal (unit length). */
    float normal_x;

    /** @brief Edge vector v1 - v0 (world space). */
    float3 edge1;
    /** @brief Y component of the precomputed face normal. */
    float normal_y;

    /** @brief Edge vector v2 - v0 (world space). */
    float3 edge2;
    /** @brief Z component of the precomputed face normal. */
    float normal_z;

    /** @brief Emission color (material emissive_factor constant, not a texture sample). */
    float3 emission;
    /** @brief Index into the texture-object array for the emissive map. */
    uint32_t emissive_tex;

    /** @brief First vertex texture coordinates. */
    float2 uv0;
    /** @brief Second vertex texture coordinates. */
    float2 uv1;

    /** @brief Third vertex texture coordinates. */
    float2 uv2;
    /** @brief 1 if the material is double-sided, 0 otherwise. */
    uint32_t double_sided;
    uint32_t _pad; ///< Pads to 16-byte aligned struct size (96 = 16 × 6).
};

static_assert(sizeof(EmissiveTriangle) == 96);

/**
 * @brief Environment-map light resources for miss sampling and env NEE.
 *
 * Host/device POD (same nature as AliasEntry / EmissiveTriangle). Scene-owned
 * fields (cubemap, alias table) come from SceneLoader via SceneRenderInput;
 * rotation_sin/cos are launch-time values filled by the renderer from
 * RenderSettings::env_rotation so miss and NEE both read orientation from
 * the same packed env block.
 */
struct EnvLightData {
    /** @brief Cubemap texture sampled on miss and for environment NEE (0 = unloaded). */
    cudaTextureObject_t cubemap;

    /** @brief Device alias table (downsampled resolution; null when unloaded). */
    const EnvAliasEntry *alias_table;

    /** @brief Number of entries in alias_table. */
    uint32_t alias_count;

    /** @brief Alias-table width (downsampled; may be smaller than the source equirect). */
    uint32_t alias_width;

    /** @brief Alias-table height (downsampled; may be smaller than the source equirect). */
    uint32_t alias_height;

    /** @brief Total luminance across the environment map (alias-table normalization). */
    float total_luminance;

    /**
     * @brief Sine of the IBL Y-axis rotation angle.
     *
     * Host-precomputed once per launch from RenderSettings::env_rotation so
     * device code can rotate ray directions for cubemap lookup and NEE without
     * a per-hit sincosf (whose Payne-Hanek reduction pulls in double math).
     * SceneLoader leaves this at 0; the renderer overwrites it when packing
     * LaunchParams.
     */
    float rotation_sin;

    /** @brief Cosine of the IBL Y-axis rotation angle (see rotation_sin). */
    float rotation_cos;
};

static_assert(sizeof(EnvLightData) == 40);

/**
 * @brief Emissive-triangle light resources for NEE.
 *
 * Host/device POD. Owned by SceneLoader; borrowed by SceneRenderInput and
 * embedded in LaunchParams so emissive resources are packed once instead of
 * mirrored field-by-field.
 */
struct EmissiveLightData {
    /** @brief Device array of emissive triangles (null when none). */
    const EmissiveTriangle *triangles;

    /** @brief Alias table over emissive triangles (one entry per triangle; null when none). */
    const AliasEntry *alias_table;

    /** @brief Number of emissive triangles. */
    uint32_t count;

    /** @brief Total radiant power across all emissive triangles. */
    float total_power;
};

static_assert(sizeof(EmissiveLightData) == 24);

/**
 * @brief Per-frame parameters uploaded to the OptiX device constant buffer.
 *
 * Shared between host (renderer.cpp) and device (programs.cu): the same
 * definition is included on both sides and must stay layout-compatible.
 */
struct LaunchParams {
    /**
     * @brief Surface object for raygen color output (write via surf2Dwrite).
     *
     * Points to the write slot of the ping-pong CudaArrayBuffer pair.
     * DLSS ON: raygen writes single-frame noisy HDR.
     * DLSS OFF: raygen writes Separate-Sum total (old + new).
     */
    cudaSurfaceObject_t color_output;

    /**
     * @brief Texture object for reading the previous accumulation total.
     *
     * Points to the read slot of the ping-pong CudaArrayBuffer pair.
     * DLSS OFF: raygen reads via tex2D for Separate-Sum accumulation.
     * DLSS ON: not used (raygen writes single-frame output without reading).
     */
    cudaTextureObject_t color_input;

    /** @brief Render resolution width in pixels (raygen launch X dimension). */
    uint32_t width;

    /** @brief Render resolution height in pixels (raygen launch Y dimension). */
    uint32_t height;

    /**
     * @brief Base Sobol sequence index for this frame's first sample.
     *
     * Raygen computes per-sample index as sequence_base + s. Advances by
     * samples_per_frame after each frame that produces samples; reset to 0
     * on accumulation reset. Monotonically increasing regardless of spp changes.
     */
    uint32_t sequence_base;

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
     * Only used when dlss_enabled == 0 (Separate Sum fallback).
     * Not a Sobol path sequence index — that is sequence_base + s.
     */
    uint32_t sample_count;

    /**
     * @brief DLSS-RR pipeline mode selector.
     *
     * 1: raygen writes single-frame noisy HDR (no accumulation, no read),
     *    and uses jitter_x/jitter_y instead of per-pixel Sobol dim 0-1.
     * 0: raygen performs Separate Sum accumulation (reads color_input,
     *    writes accumulated total to color_output), per-pixel Sobol jitter.
     */
    uint32_t dlss_enabled;

    /**
     * @brief Global per-frame subpixel jitter (DLSS ON only).
     *
     * Host-computed Sobol dim 0-1 without per-pixel Cranley-Patterson
     * rotation, so all pixels share the same jitter offset. DLSS-RR needs
     * a single InJitterOffset per frame for temporal super-resolution.
     * Range [0,1) in pixel space. Ignored when dlss_enabled == 0.
     */
    float jitter_x;
    float jitter_y;

    /**
     * @brief Firefly clamp threshold (average RGB luminance); 0 disables.
     *
     * Applied to each finished path sample: if luminance exceeds the
     * threshold, radiance is scaled by threshold / luminance.
     */
    float max_clamp;

    // ---- Scene light resources (packed; see EnvLightData / EmissiveLightData) ----

    /**
     * @brief Environment-map light resources (cubemap + alias table + rotation).
     *
     * IBL orientation lives here as rotation_sin/cos so miss and NEE both
     * consume the same packed env block.
     */
    EnvLightData env;

    /** @brief Emissive-triangle light resources (triangles + alias table). */
    EmissiveLightData emissive;

    // ---- Aux G-buffer surfaces (closesthit/raygen write via surf2Dwrite) ----

    /** @brief View-space Z depth (R32F surface). */
    cudaSurfaceObject_t aux_depth;

    /** @brief Screen-space motion vectors (RG32F surface). */
    cudaSurfaceObject_t aux_motion_vectors;

    /** @brief Raw base_color diffuse albedo (RGBA32F surface). */
    cudaSurfaceObject_t aux_diffuse_albedo;

    /** @brief Specular reflectance albedo (RGBA32F surface). */
    cudaSurfaceObject_t aux_specular_albedo;

    /** @brief World-space shading normal (RGBA32F surface, .w unused). */
    cudaSurfaceObject_t aux_normals;

    /** @brief Linear roughness (R32F surface). */
    cudaSurfaceObject_t aux_roughness;

    // ---- Motion vector matrices ----

    /**
     * @brief Current-frame unjittered view-projection matrix (row-major).
     *
     * Used by raygen to project world-space hit positions into current screen
     * space for motion vector computation.
     */
    float4x4 view_projection;

    /**
     * @brief Previous-frame unjittered view-projection matrix (row-major).
     *
     * Used by raygen to project world-space hit positions into previous screen
     * space. The difference yields the per-pixel motion vector.
     */
    float4x4 prev_view_projection;

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
