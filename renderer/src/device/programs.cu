/**
 * @file programs.cu
 * @brief OptiX device programs (renderer layer).
 */

#include <cstdint>

#include <optix_device.h>

#include <qualquer/renderer/launch_params.h>

using qualquer::renderer::float4x4;

namespace {

/// Multiply a float4x4 (row-major) by a float4 column vector.
__forceinline__ __device__ float4 mul(const float4x4 &m, const float4 &v) {
    return make_float4(
        m.rows[0].x * v.x + m.rows[0].y * v.y + m.rows[0].z * v.z + m.rows[0].w * v.w,
        m.rows[1].x * v.x + m.rows[1].y * v.y + m.rows[1].z * v.z + m.rows[1].w * v.w,
        m.rows[2].x * v.x + m.rows[2].y * v.y + m.rows[2].z * v.z + m.rows[2].w * v.w,
        m.rows[3].x * v.x + m.rows[3].y * v.y + m.rows[3].z * v.z + m.rows[3].w * v.w);
}

} // namespace

// OptiX locates program groups by the entry function symbol name (e.g.
// "__raygen__rg"), so entry points must be extern "C" at global scope. The
// launch-params constant is bound by name to pipelineLaunchParamsVariableName.
extern "C" {
__constant__ qualquer::renderer::LaunchParams params;

/// Ray generation entry point: generates a primary ray via inverse camera
/// matrices, traces it against the TLAS, and writes the returned shading
/// color into the accumulation buffer.
__global__ void __raygen__rg() { // NOLINT(*-reserved-identifier)
    const uint3 idx = optixGetLaunchIndex();
    const uint32_t linear_index = idx.y * params.width + idx.x;

    // Pixel center → NDC ([-1,1], Y flipped so +Y = up)
    const float u = (static_cast<float>(idx.x) + 0.5f) / static_cast<float>(params.width);
    const float v = (static_cast<float>(idx.y) + 0.5f) / static_cast<float>(params.height);
    const float ndc_x =  u * 2.0f - 1.0f;
    const float ndc_y = -(v * 2.0f - 1.0f);

    // NDC → view space via inv_projection (z=1 near plane, w=1 homogeneous)
    const float4 clip_target = make_float4(ndc_x, ndc_y, 1.0f, 1.0f);
    float4 view_target = mul(params.inv_projection, clip_target);
    // Perspective divide
    const float inv_w = 1.0f / view_target.w;
    view_target = make_float4(view_target.x * inv_w,
                              view_target.y * inv_w,
                              view_target.z * inv_w,
                              1.0f);

    // View space → world space via inv_view
    const float4 origin_w = mul(params.inv_view, make_float4(0.0f, 0.0f, 0.0f, 1.0f));
    const float4 dir_w    = mul(params.inv_view, make_float4(view_target.x,
                                                              view_target.y,
                                                              view_target.z,
                                                              0.0f));
    const float dir_len = sqrtf(dir_w.x * dir_w.x + dir_w.y * dir_w.y + dir_w.z * dir_w.z);
    const float inv_len = 1.0f / dir_len;
    const float3 ray_origin    = make_float3(origin_w.x, origin_w.y, origin_w.z);
    const float3 ray_direction = make_float3(dir_w.x * inv_len,
                                             dir_w.y * inv_len,
                                             dir_w.z * inv_len);

    // Payload registers: p0-p2 = shading color (RGB)
    uint32_t p0 = 0;
    uint32_t p1 = 0;
    uint32_t p2 = 0;

    optixTrace(params.traversable,
               ray_origin,
               ray_direction,
               0.0f,           // tmin
               1e16f,          // tmax
               0.0f,           // rayTime
               0xFF,           // visibilityMask
               OPTIX_RAY_FLAG_NONE,
               0,              // SBT offset
               1,              // SBT stride
               0,              // miss SBT index
               p0, p1, p2);

    const float r = __uint_as_float(p0);
    const float g = __uint_as_float(p1);
    const float b = __uint_as_float(p2);

    // Overwrite accumulation buffer each frame (no temporal accumulation yet)
    params.accumulation_buffer[linear_index] = make_float4(r, g, b, 1.0f);
}

/// Miss entry point.
__global__ void __miss__ms() { // NOLINT(*-reserved-identifier)
}

/// Closest-hit entry point.
__global__ void __closesthit__ch() { // NOLINT(*-reserved-identifier)
}

/// Any-hit entry point.
__global__ void __anyhit__ah() { // NOLINT(*-reserved-identifier)
}

}  // extern "C"
