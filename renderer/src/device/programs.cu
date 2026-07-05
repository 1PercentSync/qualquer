/**
 * @file programs.cu
 * @brief OptiX device programs (renderer layer).
 */

#include <cstdint>

#include <optix_device.h>

#include <qualquer/renderer/launch_params.h>

// Device-side definitions for forward-declared types (see material.h / vertex.h).
// CUDA-native types replace glm to avoid the glm include dependency: the
// standalone nvcc custom command does not inherit vcpkg system include paths.
// static_asserts lock the layout against the host-side originals.
namespace qualquer::renderer {

struct GPUGeometryInfo {
    CUdeviceptr vertex_buffer_address;
    CUdeviceptr index_buffer_address;
    uint32_t material_buffer_offset;
    // ReSharper disable once CppDeclaratorNeverUsed
    uint32_t padding;
};
static_assert(sizeof(GPUGeometryInfo) == 24);

struct alignas(16) Material {
    float4 base_color_factor;
    float4 emissive_factor;
    float metallic_factor;
    float roughness_factor;
    float normal_scale;
    float occlusion_strength;
    uint32_t base_color_tex;
    uint32_t emissive_tex;
    uint32_t metallic_roughness_tex;
    uint32_t normal_tex;
    uint32_t occlusion_tex;
    float alpha_cutoff;
    uint32_t alpha_mode;
    uint32_t double_sided;
};
static_assert(sizeof(Material) == 80);

} // namespace qualquer::renderer

#include <qualquer/renderer/math_utils.cuh>

// OptiX locates program groups by the entry function symbol name (e.g.
// "__raygen__rg"), so entry points must be extern "C" at global scope. The
// launch-params constant is bound by name to pipelineLaunchParamsVariableName.
extern "C" {
__constant__ qualquer::renderer::LaunchParams params;

/// Ray generation: primary ray via inverse camera matrices → optixTrace →
/// payload → accumulation buffer.
__global__ void __raygen__rg() { // NOLINT(*-reserved-identifier)
    const uint3 idx = optixGetLaunchIndex();
    const uint32_t linear_index = idx.y * params.width + idx.x;

    // Empty scene (no geometry loaded → traversable == 0): optixTrace on a null
    // traversable is undefined behavior, so write the miss background color
    // directly and skip tracing. Matches __miss__ms output, keeping the frame
    // consistent with a fully-miss scene.
    if (params.traversable == 0) {
        params.accumulation_buffer[linear_index] = make_float4(0.1f, 0.1f, 0.1f, 1.0f);
        return;
    }

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
    const float3 ray_origin    = make_float3(origin_w.x, origin_w.y, origin_w.z);
    const float3 ray_direction = normalize(make_float3(dir_w.x, dir_w.y, dir_w.z));

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

    params.accumulation_buffer[linear_index] = make_float4(r, g, b, 1.0f);
}

/// Emits a constant background color on miss.
__global__ void __miss__ms() { // NOLINT(*-reserved-identifier)
    optixSetPayload_0(__float_as_uint(0.1f));
    optixSetPayload_1(__float_as_uint(0.1f));
    optixSetPayload_2(__float_as_uint(0.1f));
}

/// Closest hit: geometry lookup → vertex interpolation → PBR texture sampling
/// → normal mapping → ambient shading (baseColor × occlusion) → payload.
__global__ void __closesthit__ch() { // NOLINT(*-reserved-identifier)
    // ---- Geometry info + material lookup ----
    const uint32_t geo_index = optixGetInstanceId() + optixGetSbtGASIndex();
    const auto &geo = params.geometry_infos[geo_index];
    const auto &mat = params.materials[geo.material_buffer_offset];

    // ---- Triangle vertex fetch ----
    const uint32_t prim_idx = optixGetPrimitiveIndex();
    const auto *indices = reinterpret_cast<const uint32_t *>(geo.index_buffer_address);
    const uint32_t i0 = indices[3 * prim_idx];
    const uint32_t i1 = indices[3 * prim_idx + 1];
    const uint32_t i2 = indices[3 * prim_idx + 2];

    const auto *verts = reinterpret_cast<const DeviceVertex *>(geo.vertex_buffer_address);
    const DeviceVertex &v0 = verts[i0];
    const DeviceVertex &v1 = verts[i1];
    const DeviceVertex &v2 = verts[i2];

    // ---- Barycentric interpolation (object space) ----
    const float2 bary = optixGetTriangleBarycentrics();
    const float w0 = 1.0f - bary.x - bary.y;
    const float w1 = bary.x;
    const float w2 = bary.y;

    const float3 obj_normal = w0 * v0.normal + w1 * v1.normal + w2 * v2.normal;
    const float2 uv         = w0 * v0.uv0    + w1 * v1.uv0    + w2 * v2.uv0;
    const float4 tangent    = w0 * v0.tangent + w1 * v1.tangent + w2 * v2.tangent;

    // Face normal from triangle edges (object space, not normalized)
    const float3 face_normal_obj = cross(v1.position - v0.position,
                                         v2.position - v0.position);

    // ---- World-space transforms ----
    float3 N_interp = normalize(optixTransformNormalFromObjectToWorldSpace(obj_normal));
    float3 N_face   = normalize(optixTransformNormalFromObjectToWorldSpace(face_normal_obj));
    const float3 T_world  = normalize(optixTransformVectorFromObjectToWorldSpace(
        make_float3(tangent.x, tangent.y, tangent.z)));

    // ---- Back-face flip ----
    if (const float3 ray_dir = optixGetWorldRayDirection(); dot(N_face, ray_dir) > 0.0f) {
        N_face   = -N_face;
        N_interp = -N_interp;
    }

    // ---- PBR texture sampling ----
    const cudaTextureObject_t *tex = params.texture_objects;

    const auto bc_texel = tex2D<float4>(tex[mat.base_color_tex], uv.x, uv.y);
    const float3 base_color = make_float3(bc_texel.x * mat.base_color_factor.x,
                                          bc_texel.y * mat.base_color_factor.y,
                                          bc_texel.z * mat.base_color_factor.z);

    // Metallic-roughness and emissive sampled for pipeline validation
    const auto mr_texel = tex2D<float4>(tex[mat.metallic_roughness_tex], uv.x, uv.y);
    const auto em_texel = tex2D<float4>(tex[mat.emissive_tex], uv.x, uv.y);
    (void)mr_texel;
    (void)em_texel;

    // Occlusion: glTF spec lerp — color * (1 + strength * (ao - 1))
    const auto ao_texel = tex2D<float4>(tex[mat.occlusion_tex], uv.x, uv.y);
    const float occlusion = 1.0f + mat.occlusion_strength * (ao_texel.x - 1.0f);

    // ---- Normal mapping (TBN + BC5 normal map) ----
    const auto nm_texel = tex2D<float4>(tex[mat.normal_tex], uv.x, uv.y);
    const float3 N_shading = get_shading_normal(
        N_interp, T_world, tangent.w,
        make_float2(nm_texel.x, nm_texel.y), mat.normal_scale);
    (void)N_shading;

    // ---- Ambient shading: baseColor × occlusion ----
    const float3 color = base_color * occlusion;

    optixSetPayload_0(__float_as_uint(color.x));
    optixSetPayload_1(__float_as_uint(color.y));
    optixSetPayload_2(__float_as_uint(color.z));
}

/// Intentionally empty — opaque geometry disables any-hit.
__global__ void __anyhit__ah() { // NOLINT(*-reserved-identifier)
}

}  // extern "C"
