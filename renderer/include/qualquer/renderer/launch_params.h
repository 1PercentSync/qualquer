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
 * @brief Per-frame parameters uploaded to the OptiX device constant buffer.
 *
 * Shared between host (renderer.cpp) and device (programs.cu): the same
 * definition is included on both sides and must stay layout-compatible.
 */
struct LaunchParams {
    /** @brief Device pointer to the accumulation buffer (HDR RGBA32F, written by raygen). */
    float4 *accumulation_buffer;

    /** @brief Framebuffer width in pixels. */
    uint32_t width;

    /** @brief Framebuffer height in pixels. */
    uint32_t height;

    /** @brief Monotonic accumulation counter driving temporal noise variation. */
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
};

} // namespace qualquer::renderer
