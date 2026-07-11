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
        uint32_t base_color_tex;
        uint32_t emissive_tex;
        uint32_t metallic_roughness_tex;
        uint32_t normal_tex;
        float alpha_cutoff;
        uint32_t alpha_mode;
        uint32_t double_sided;
    };

    static_assert(sizeof(Material) == 80);
} // namespace qualquer::renderer

#include <qualquer/renderer/math_utils.cuh>
#include <qualquer/renderer/brdf.cuh>
#include <qualquer/renderer/rng.cuh>
#include <qualquer/renderer/pt_common.cuh>
#include <qualquer/renderer/payload_helpers.cuh>
#include <qualquer/renderer/tonemap.cuh>
#include <qualquer/renderer/nee.cuh>

// OptiX locates program groups by the entry function symbol name (e.g.
// "__raygen__rg"), so entry points must be extern "C" at global scope. The
// launch-params constant is bound by name to pipelineLaunchParamsVariableName.
extern "C" {
__constant__ qualquer::renderer::LaunchParams params;

/// Traces one path from a primary ray, returning per-sample radiance.
/// When capture_primary is true (first sample), writes primary hit/miss
/// info into the output parameters for MV and sky aux-default computation.
__forceinline__ __device__ float3 trace_sample(
    const float3 cam_origin,
    const float3 primary_dir,
    const uint32_t pixel_index,
    const uint32_t sample_index,
    const bool capture_primary,
    float3 &primary_hit_pos,
    float3 &primary_sky_color,
    bool &primary_is_hit
) {
    using namespace qualquer::renderer;

    PathState path{};
    path.origin = cam_origin;
    path.direction = primary_dir;
    path.throughput = make_float3(1.0f, 1.0f, 1.0f);
    path.radiance = make_float3(0.0f, 0.0f, 0.0f);
    path.pixel_index = pixel_index;
    path.sample_index = sample_index;
    path.bounce = 0;
    path.alive = true;

    // Payload registers persist across bounces: closesthit's env_mis_weight
    // (p13) survives into the next iteration's miss read.
    uint32_t p0 = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
    uint32_t p6 = 0, p7 = 0, p8 = 0, p9 = 0, p10 = 0, p11 = 0;
    uint32_t p12 = 0, p13 = __float_as_uint(1.0f), p14 = 0;
    uint32_t p15 = 0, p16 = 0, p17 = 0;

    while (path.alive && path.bounce < params.max_bounces) {
        // ---- Russian Roulette (bounce >= 2) ----
        if (path.bounce >= 2) {
            const uint32_t rr_dim = bounce_dim_base(path.bounce) + kBounceOffsetRR;
            const float rr_rand = sobol_rng(params.sobol_directions, pixel_index,
                                           sample_index, params.frame_index, rr_dim);
            const float rr_prob = fminf(0.95f,
                fmaxf(0.05f, fmaxf(path.throughput.x,
                                   fmaxf(path.throughput.y, path.throughput.z))));
            if (rr_rand >= rr_prob) {
                path.alive = false;
                break;
            }
            path.throughput = path.throughput / rr_prob;
        }

        p15 = sample_index;
        p17 = path.bounce;

        optixTraverse(params.traversable,
                      path.origin,
                      path.direction,
                      0.0f, // tmin
                      1e16f, // tmax
                      0.0f, // rayTime
                      0xFF, // visibilityMask
                      OPTIX_RAY_FLAG_NONE,
                      0, // SBT offset
                      0, // SBT stride (all geometries share one hitgroup record)
                      0, // miss SBT index (env)
                      p0, p1, p2, p3, p4, p5,
                      p6, p7, p8, p9, p10, p11,
                      p12, p13, p14, p15, p16, p17);
        // SER: reorder by material for texture cache coherence. With a
        // single hitgroup record the default hint only separates hit vs
        // miss; the masked material hint additionally groups threads by
        // material. 10 bits cover 1024 materials exactly; beyond that the
        // mask aliases low index bits (bucket-sharing materials still
        // group, quality degrades gracefully). Wider hints measurably pay
        // more sort cost; narrower ones cap the exact-grouping capacity.
        uint32_t reorder_hint = 0;
        if (optixHitObjectIsHit()) {
            const uint32_t geo_idx = optixHitObjectGetInstanceId()
                                   + optixHitObjectGetSbtGASIndex();
            reorder_hint = params.geometry_infos[geo_idx].material_buffer_offset;
        }
        optixReorder(reorder_hint & 0x3FFu, 10);
        optixInvoke(p0, p1, p2, p3, p4, p5,
                    p6, p7, p8, p9, p10, p11,
                    p12, p13, p14, p15, p16, p17);

        const PayloadData d = payload_unpack(
            p0, p1, p2, p3, p4, p5,
            p6, p7, p8, p9, p10, p11,
            p12, p13, p14, p15, p16, p17);

        // Capture first-bounce result for MV + sky aux defaults (first sample only).
        if (capture_primary && path.bounce == 0) {
            if (d.hit_distance >= 0.0f) {
                primary_is_hit = true;
                primary_hit_pos = cam_origin + primary_dir * d.hit_distance;
            } else {
                primary_is_hit = false;
                primary_sky_color = d.color;
            }
        }

        // Accumulate radiance contribution from this bounce.
        float3 contribution = path.throughput * d.color;
        if (d.hit_distance < 0.0f) {
            contribution = contribution * d.env_mis_weight;
        }
        path.radiance = path.radiance + contribution;

        if (d.hit_distance < 0.0f) {
            path.alive = false;
            break;
        }

        path.throughput = path.throughput * d.throughput_update;

        if (fmaxf(path.throughput.x, fmaxf(path.throughput.y, path.throughput.z)) < 1e-6f) {
            path.alive = false;
            break;
        }

        path.origin = d.next_origin;
        path.direction = d.next_direction;
        path.bounce += 1;
    }

    return path.radiance;
}

/// Computes the primary ray direction from subpixel jitter offsets.
__forceinline__ __device__ float3 compute_primary_dir(
    const uint32_t px, const uint32_t py,
    const float jx, const float jy
) {
    const float u = (static_cast<float>(px) + jx) / static_cast<float>(params.width);
    const float v = (static_cast<float>(py) + jy) / static_cast<float>(params.height);
    const float ndc_x = u * 2.0f - 1.0f;
    const float ndc_y = -(v * 2.0f - 1.0f);

    const float4 clip_target = make_float4(ndc_x, ndc_y, 1.0f, 1.0f);
    float4 view_target = mul(params.inv_projection, clip_target);
    const float inv_w = 1.0f / view_target.w;
    view_target = make_float4(view_target.x * inv_w,
                              view_target.y * inv_w,
                              view_target.z * inv_w,
                              1.0f);
    const float4 dir_w = mul(params.inv_view, make_float4(view_target.x,
                                                          view_target.y,
                                                          view_target.z,
                                                          0.0f));
    return normalize(make_float3(dir_w.x, dir_w.y, dir_w.z));
}

/// Ray generation: samples_per_frame paths per pixel with subpixel jitter,
/// accumulated in registers and written once as a DLSS mean or fallback sum.
__global__ void __raygen__rg() { // NOLINT(*-reserved-identifier)
    using namespace qualquer::renderer;

    const uint3 idx = optixGetLaunchIndex();
    const uint32_t pixel_index = idx.y * params.width + idx.x;

    // Empty scene (no geometry loaded → traversable == 0): optixTrace on a
    // null traversable is undefined behavior. Write black directly — reading
    // the read buffer would access uninitialized memory on the first frame
    // after allocation. Host sets the write-slot count to 0 to match.
    if (params.traversable == 0) {
        surf2Dwrite(make_float4(0.0f, 0.0f, 0.0f, 1.0f),
                    params.color_output,
                    static_cast<int>(idx.x * sizeof(float4)),
                    static_cast<int>(idx.y));
        return;
    }

    // Camera origin (constant across all samples within this frame).
    const float4 origin_w = mul(params.inv_view, make_float4(0.0f, 0.0f, 0.0f, 1.0f));
    const float3 cam_origin = make_float3(origin_w.x, origin_w.y, origin_w.z);

    // Register-local accumulation across all samples in this frame.
    float3 frame_radiance = make_float3(0.0f, 0.0f, 0.0f);

    // First-bounce capture for MV + sky aux defaults (sample 0 only).
    float3 primary_hit_pos = make_float3(0.0f, 0.0f, 0.0f);
    float3 primary_sky_color = make_float3(0.0f, 0.0f, 0.0f);
    float3 primary_dir_saved = make_float3(0.0f, 0.0f, 1.0f);
    bool primary_is_hit = false;

    if (params.dlss_enabled) {
        // DLSS ON: per-frame jitter is uniform across pixels and samples.
        // Primary ray is identical for all samples — compute once, loop
        // only over path tracing.
        const float3 primary_dir = compute_primary_dir(
            idx.x, idx.y, params.jitter_x, params.jitter_y);
        primary_dir_saved = primary_dir;

        for (uint32_t s = 0; s < params.samples_per_frame; ++s) {
            const uint32_t sample_index = params.sample_count + s;
            frame_radiance = frame_radiance + trace_sample(
                cam_origin, primary_dir, pixel_index, sample_index,
                s == 0, primary_hit_pos, primary_sky_color, primary_is_hit);
        }
    } else {
        // DLSS OFF: per-pixel Sobol + CP rotation, per-sample jitter.
        // Each sample gets a different primary ray for Monte Carlo convergence.
        for (uint32_t s = 0; s < params.samples_per_frame; ++s) {
            const uint32_t sample_index = params.sample_count + s;
            const float jx = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, kDimJitterX);
            const float jy = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, kDimJitterY);
            const float3 primary_dir = compute_primary_dir(idx.x, idx.y, jx, jy);

            if (s == 0) {
                primary_dir_saved = primary_dir;
            }

            frame_radiance = frame_radiance + trace_sample(
                cam_origin, primary_dir, pixel_index, sample_index,
                s == 0, primary_hit_pos, primary_sky_color, primary_is_hit);
        }
    }

    // ---- Color output ----
    // Alpha is always 1.0 — the DLSS-OFF tonemap resampling paths (Catmull-Rom
    // upscale / box-filter downscale) skip alpha interpolation and hardcode
    // 1.0, so a non-trivial alpha written here would be silently discarded
    // when render != display resolution. Keep in sync if alpha gains meaning.
    // DLSS ON: mean single-frame noisy HDR (no persistent accumulation).
    // DLSS OFF: Separate Sum — read old total via tex2D, add new, write total.
    {
        const int sx = static_cast<int>(idx.x);
        const int sy = static_cast<int>(idx.y);
        float4 out_color;
        if (params.dlss_enabled) {
            const float inverse_sample_count =
                1.0f / static_cast<float>(params.samples_per_frame);
            out_color = make_float4(
                frame_radiance.x * inverse_sample_count,
                frame_radiance.y * inverse_sample_count,
                frame_radiance.z * inverse_sample_count,
                1.0f);
        } else if (params.sample_count == 0) {
            out_color = make_float4(frame_radiance.x, frame_radiance.y, frame_radiance.z, 1.0f);
        } else {
            const float4 old = tex2D<float4>(params.color_input,
                                              static_cast<float>(idx.x) + 0.5f,
                                              static_cast<float>(idx.y) + 0.5f);
            out_color = make_float4(old.x + frame_radiance.x,
                                    old.y + frame_radiance.y,
                                    old.z + frame_radiance.z, 1.0f);
        }
        surf2Dwrite(out_color, params.color_output,
                    sx * static_cast<int>(sizeof(float4)), sy);
    }

    // ---- Aux G-buffer: motion vectors + sky defaults ----
    const int sx = static_cast<int>(idx.x);
    const int sy = static_cast<int>(idx.y);
    const float2 resolution = make_float2(
        static_cast<float>(params.width), static_cast<float>(params.height));

    // MV: project world position (w=1 hit) or direction (w=0 sky) through
    // current and previous unjittered VP. Pixel-space result with MVScale=1.
    {
        const float hw = primary_is_hit ? 1.0f : 0.0f;
        const float3 pos = primary_is_hit ? primary_hit_pos : primary_dir_saved;
        const float4 homo = make_float4(pos.x, pos.y, pos.z, hw);
        const float4 curr_clip = mul(params.view_projection, homo);
        const float4 prev_clip = mul(params.prev_view_projection, homo);
        const float2 curr_ndc = make_float2(curr_clip.x / curr_clip.w,
                                            curr_clip.y / curr_clip.w);
        const float2 prev_ndc = make_float2(prev_clip.x / prev_clip.w,
                                            prev_clip.y / prev_clip.w);
        const float2 mv = make_float2(
            (prev_ndc.x - curr_ndc.x) * 0.5f * resolution.x,
            (prev_ndc.y - curr_ndc.y) * 0.5f * resolution.y);
        surf2Dwrite(mv, params.aux_motion_vectors,
                    sx * static_cast<int>(sizeof(float2)), sy);
    }

    // Sky pixels: closesthit never ran, so write aux defaults.
    if (!primary_is_hit) {
        const float inf = __int_as_float(0x7f800000);
        surf2Dwrite(inf, params.aux_depth,
                    sx * static_cast<int>(sizeof(float)), sy);

        // Tonemapped sky radiance as diffuse albedo guide (HDR compressed
        // to [0,1] so DLSS-RR preserves sky detail without denoising it).
        const float3 sky_albedo = pbr_neutral_tonemap(primary_sky_color);
        surf2Dwrite(make_float4(sky_albedo.x, sky_albedo.y, sky_albedo.z, 1.0f),
                    params.aux_diffuse_albedo,
                    sx * static_cast<int>(sizeof(float4)), sy);

        surf2Dwrite(make_float4(0.0f, 0.0f, 0.0f, 0.0f), params.aux_specular_albedo,
                    sx * static_cast<int>(sizeof(float4)), sy);
        surf2Dwrite(make_float4(0.0f, 0.0f, 0.0f, 0.0f), params.aux_normals,
                    sx * static_cast<int>(sizeof(float4)), sy);
        surf2Dwrite(0.0f, params.aux_roughness,
                    sx * static_cast<int>(sizeof(float)), sy);
    }
}

/// Samples the HDR environment cubemap along the missed ray direction.
/// Applies IBL Y-axis rotation (world → env space) before cubemap lookup.
/// Falls back to a constant background when no env cubemap is loaded.
__global__ void __miss__env() { // NOLINT(*-reserved-identifier)
    using namespace qualquer::renderer;

    float3 env_color = make_float3(0.0f, 0.0f, 0.0f);
    if (params.env_cubemap != 0) {
        const float3 world_dir = optixGetWorldRayDirection();
        // Forward IBL rotation: world space → env space.
        const float3 dir = rotate_y_dir(world_dir,
                                        params.env_rotation_sin, params.env_rotation_cos);
        const auto texel = texCubemap<float4>(params.env_cubemap, dir.x, dir.y, dir.z);
        env_color = make_float3(texel.x, texel.y, texel.z);
    }

    payload_set_color(env_color);
    payload_set_hit_distance(-1.0f);
}

/// Closest hit: geometry + material setup, normal mapping, ray offset,
/// 18-register payload write. BRDF sampling added in the next checkpoint.
__global__ void __closesthit__ch() { // NOLINT(*-reserved-identifier)
    using namespace qualquer::renderer;

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

    const float3 obj_pos = w0 * v0.position + w1 * v1.position + w2 * v2.position;
    const float3 obj_normal = w0 * v0.normal + w1 * v1.normal + w2 * v2.normal;
    const float2 uv = w0 * v0.uv0 + w1 * v1.uv0 + w2 * v2.uv0;
    const float4 vert_color = w0 * v0.color + w1 * v1.color + w2 * v2.color;
    const float4 tangent = w0 * v0.tangent + w1 * v1.tangent + w2 * v2.tangent;

    const float3 face_normal_obj = cross(v1.position - v0.position,
                                         v2.position - v0.position);

    // ---- World-space transforms ----
    const float3 world_pos = optixTransformPointFromObjectToWorldSpace(obj_pos);
    float3 N_interp = normalize(optixTransformNormalFromObjectToWorldSpace(obj_normal));
    float3 N_face = normalize(optixTransformNormalFromObjectToWorldSpace(face_normal_obj));
    const float3 T_world = normalize(optixTransformVectorFromObjectToWorldSpace(
        make_float3(tangent.x, tangent.y, tangent.z)));

    // ---- Back-face detection + single-sided pass-through ----
    const float3 ray_dir = optixGetWorldRayDirection();
    const bool is_back_face = dot(N_face, ray_dir) > 0.0f;

    if (is_back_face && mat.double_sided == 0u) {
        // Single-sided material hit from behind: pass through the surface.
        // Throughput unchanged, consumes one bounce. Origin is pushed past
        // the hit point along the ray direction to avoid re-intersection.
        const float hit_t = optixGetRayTmax();
        const float pass_eps = fmaxf(hit_t * 1e-4f, 1e-6f);
        const float3 pass_origin = optixGetWorldRayOrigin() + ray_dir * (hit_t + pass_eps);

        payload_set_next_origin(pass_origin);
        payload_set_next_direction(ray_dir);
        payload_set_throughput_update(make_float3(1.0f, 1.0f, 1.0f));
        payload_set_color(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_hit_distance(hit_t);
        payload_set_env_mis_weight(1.0f);
        // last_brdf_pdf: intentionally NOT written. Payload registers persist
        // across bounces, so the previous closesthit's BRDF pdf carries through
        // the pass-through unchanged — correct for MIS when the path later hits
        // an emissive surface or misses into the environment.
        return;
    }

    // Double-sided back-face: flip normals to face the incoming ray.
    if (is_back_face) {
        N_face = -N_face;
        N_interp = -N_interp;
    }

    // ---- PBR texture sampling ----
    const cudaTextureObject_t *tex = params.texture_objects;

    const auto bc_texel = tex2D<float4>(tex[mat.base_color_tex], uv.x, uv.y);
    const float3 base_color = make_float3(bc_texel.x * mat.base_color_factor.x * vert_color.x,
                                          bc_texel.y * mat.base_color_factor.y * vert_color.y,
                                          bc_texel.z * mat.base_color_factor.z * vert_color.z);

    const auto mr_texel = tex2D<float4>(tex[mat.metallic_roughness_tex], uv.x, uv.y);
    const float metallic = mr_texel.z * mat.metallic_factor;
    const float roughness = fmaxf(mr_texel.y * mat.roughness_factor, 0.04f);
    const float alpha = fmaxf(roughness * roughness, 1e-4f);

    // ---- Normal mapping + consistency ----
    const auto nm_texel = tex2D<float4>(tex[mat.normal_tex], uv.x, uv.y);
    const float3 N_mapped = get_shading_normal(
        N_interp, T_world, tangent.w,
        make_float2(nm_texel.x, nm_texel.y), mat.normal_scale);
    const float3 N_shading = ensure_normal_consistency(N_mapped, N_interp);

    // ---- Emissive (with BRDF-strategy MIS weight for bounce > 0) ----
    const auto em_texel = tex2D<float4>(tex[mat.emissive_tex], uv.x, uv.y);
    float3 emissive = make_float3(em_texel.x * mat.emissive_factor.x,
                                  em_texel.y * mat.emissive_factor.y,
                                  em_texel.z * mat.emissive_factor.z);

    // Bounce 0 (direct view): weight 1.0 — primary ray is not a BRDF sample.
    // Bounce > 0 with emissive NEE active: BRDF hit emissive is one of two MIS
    // strategies, weight by power_heuristic(brdf_pdf, light_pdf).
    // Bounce > 0 without emissive NEE: weight 1.0 — no competing strategy.
    const uint32_t bounce = payload_get_bounce();
    if (bounce > 0 && params.emissive_count > 0) {
        const float emi_lum = 0.2126f * mat.emissive_factor.x
            + 0.7152f * mat.emissive_factor.y + 0.0722f * mat.emissive_factor.z;
        if (emi_lum > 0.0f) {
            const float cos_theta_l = fabsf(dot(N_face, ray_dir));
            const float hit_dist = optixGetRayTmax();
            const float light_pdf = emissive_light_pdf(
                emi_lum, hit_dist, cos_theta_l, params.emissive_total_power);
            const float last_brdf_pdf = payload_get_last_brdf_pdf();
            const float mis_w = mis_power_heuristic(last_brdf_pdf, light_pdf);
            emissive = emissive * mis_w;
        }
    }

    // ---- Ray offset ----
    const float3 offset_pos = offset_ray_origin(world_pos, N_face);

    // ---- BRDF sampling ----
    const float3 V = -ray_dir;
    float3 T_basis, B_basis;
    build_orthonormal_basis(N_shading, T_basis, B_basis);
    const BrdfParams bp = init_brdf_params(
        V, N_shading, T_basis, B_basis,
        base_color, metallic, roughness, alpha);

    const uint3 launch_idx = optixGetLaunchIndex();
    const uint32_t pixel_index = launch_idx.y * params.width + launch_idx.x;

    // ---- Aux G-buffer writes (first sample's bounce 0 only) ----
    // D37: all samples share per-frame jitter → primary ray identical →
    // aux data identical. Write once at the first sample of the batch.
    if (bounce == 0 && payload_get_sample_index() == params.sample_count) {
        const int sx = static_cast<int>(launch_idx.x);
        const int sy = static_cast<int>(launch_idx.y);

        // View-space Z depth: dot(camera_forward, hitPos - camera_origin).
        // Camera forward = inv_view * (0,0,-1,0); camera origin = inv_view * (0,0,0,1).
        // inv_view is an orthonormal rotation + translation, so the extracted
        // forward direction is already unit length (no normalize needed).
        const float3 cam_forward = make_float3(
            -params.inv_view.rows[0].z,
            -params.inv_view.rows[1].z,
            -params.inv_view.rows[2].z);
        const float3 cam_origin = make_float3(
            params.inv_view.rows[0].w,
            params.inv_view.rows[1].w,
            params.inv_view.rows[2].w);
        const float view_z = dot(cam_forward, world_pos - cam_origin);
        surf2Dwrite(view_z, params.aux_depth,
                    sx * static_cast<int>(sizeof(float)), sy);

        // Diffuse albedo: raw base_color (not pre-multiplied by (1-metallic)).
        surf2Dwrite(make_float4(base_color.x, base_color.y, base_color.z, 1.0f),
                    params.aux_diffuse_albedo,
                    sx * static_cast<int>(sizeof(float4)), sy);

        // Specular albedo: E_glossy per channel (Turquin-compensated specular
        // directional reflectance, self-consistent with our energy compensation).
        surf2Dwrite(make_float4(bp.E_glossy_rgb.x, bp.E_glossy_rgb.y, bp.E_glossy_rgb.z, 1.0f),
                    params.aux_specular_albedo,
                    sx * static_cast<int>(sizeof(float4)), sy);

        // World-space shading normal.
        surf2Dwrite(make_float4(N_shading.x, N_shading.y, N_shading.z, 0.0f),
                    params.aux_normals,
                    sx * static_cast<int>(sizeof(float4)), sy);

        // Linear roughness.
        surf2Dwrite(roughness, params.aux_roughness,
                    sx * static_cast<int>(sizeof(float)), sy);
    }

    const uint32_t sample_index = payload_get_sample_index();
    const uint32_t dim_base = bounce_dim_base(bounce);

    const float u_lobe = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index, dim_base + kBounceOffsetLobeSelect);
    const float u0 = sobol_rng(params.sobol_directions, pixel_index,
                               sample_index, params.frame_index, dim_base + kBounceOffsetBrdfXi0);
    const float u1 = sobol_rng(params.sobol_directions, pixel_index,
                               sample_index, params.frame_index, dim_base + kBounceOffsetBrdfXi1);

    const BrdfSample bs = brdf_sample(bp, u_lobe, u0, u1);

    // ---- NEE: Environment light (alias table importance sampling + MIS) ----
    float3 nee_radiance = make_float3(0.0f, 0.0f, 0.0f);

    if (params.env_cubemap != 0 && params.env_alias_table != nullptr) {
        const float env_r1 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEnvNee);
        const float env_r2 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEnvNee + 1);
        const float env_r3 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEnvNee + 2);
        const float env_r4 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEnvNee + 3);

        const float3 L = sample_env_alias_table(
            params.env_alias_table, params.env_alias_count,
            params.env_alias_width, params.env_alias_height,
            params.env_rotation_sin, params.env_rotation_cos,
            env_r1, env_r2, env_r3, env_r4);
        const float NdotL = dot(N_shading, L);

        if (NdotL > 0.0f) {
            const uint32_t visible = trace_shadow_ray(
                params.traversable, offset_pos, L, 1e16f);

            if (visible) {
                // Environment radiance at the sampled direction.
                // L is in world space; rotate to env space for cubemap lookup.
                const float3 env_L = rotate_y_dir(
                    L, params.env_rotation_sin, params.env_rotation_cos);
                const auto env_texel = texCubemap<float4>(
                    params.env_cubemap, env_L.x, env_L.y, env_L.z);
                const float3 env_color = make_float3(
                    env_texel.x, env_texel.y, env_texel.z);

                // Evaluate BRDF at the light direction.
                const float3 brdf_val = brdf_eval(bp, L, NdotL);

                // Combined multi-lobe BRDF PDF at the light direction.
                const float3 H = normalize(bp.V + L);
                const float NdotH_e = fmaxf(dot(N_shading, H), 0.0f);
                const float pdf_spec_e = pdf_ggx_vndf(NdotH_e, bp.NdotV, bp.alpha);
                const float3 V_ts = world_to_tangent(bp.T, bp.B, bp.N, bp.V);
                const float3 L_ts = world_to_tangent(bp.T, bp.B, bp.N, L);
                const float pdf_diff_e = pdf_EON(V_ts, L_ts, bp.r);
                const float brdf_pdf_e = combined_lobe_pdf(pdf_spec_e, pdf_diff_e, bp.p_spec);

                // Light-strategy MIS weight.
                const float pdf_light = env_pdf(
                    params.env_alias_table,
                    params.env_alias_width, params.env_alias_height,
                    params.env_total_luminance,
                    params.env_rotation_sin, params.env_rotation_cos, L);
                const float mis_w = mis_power_heuristic(pdf_light, brdf_pdf_e);

                const float st_factor = shadow_terminator_factor(
                    N_face, N_shading, L);
                nee_radiance = env_color * brdf_val * NdotL * mis_w * st_factor
                    / fmaxf(pdf_light, 1e-7f);
            }
        }
    }

    // ---- NEE: Emissive area lights (alias table importance sampling + MIS) ----
    if (params.emissive_count > 0 && params.emissive_triangles != nullptr) {
        const float emi_r1 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEmissiveNee);
        const float emi_r2 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEmissiveNee + 1);
        const float emi_r3 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEmissiveNee + 2);
        const float emi_r4 = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, dim_base + kBounceOffsetEmissiveNee + 3);

        // Select emissive triangle from power-weighted alias table.
        const uint32_t tri_idx = sample_emissive_alias_table(
            params.emissive_alias_table, params.emissive_count, emi_r1, emi_r2);
        const EmissiveTriangle &tri = params.emissive_triangles[tri_idx];

        // Uniform sample point on triangle.
        const float3 light_bary = triangle_barycentric(emi_r3, emi_r4);
        const float3 light_pos = tri.v0 * light_bary.x + tri.v1 * light_bary.y + tri.v2 * light_bary.z;

        // Direction and distance to the light sample.
        const float3 to_light = light_pos - offset_pos;
        const float dist2 = dot(to_light, to_light);
        const float dist = sqrtf(dist2);
        const float3 L = to_light * (1.0f / dist);

        // Light triangle geometric normal.
        const float3 light_edge1 = tri.v1 - tri.v0;
        const float3 light_edge2 = tri.v2 - tri.v0;
        const float3 light_normal = normalize(cross(light_edge1, light_edge2));
        float cos_theta_light = dot(light_normal, -L);

        // Double-sided handling: follow material double_sided flag.
        const Material &light_mat = params.materials[tri.material_index];
        bool light_visible = cos_theta_light > 0.0f;
        if (!light_visible && light_mat.double_sided == 1u) {
            cos_theta_light = -cos_theta_light;
            light_visible = true;
        }

        const float NdotL_emi = dot(N_shading, L);

        if (light_visible && NdotL_emi > 0.0f) {
            // Shadow ray (tMax shortened to avoid hitting the target triangle).
            const uint32_t visible = trace_shadow_ray(
                params.traversable, offset_pos, L, dist * (1.0f - 1e-4f));

            if (visible) {
                // Emissive radiance at the sample point (textured emission).
                const float2 light_uv = tri.uv0 * light_bary.x + tri.uv1 * light_bary.y
                                       + tri.uv2 * light_bary.z;
                const auto le_texel = tex2D<float4>(
                    tex[light_mat.emissive_tex], light_uv.x, light_uv.y);
                const float3 Le = make_float3(
                    le_texel.x * light_mat.emissive_factor.x,
                    le_texel.y * light_mat.emissive_factor.y,
                    le_texel.z * light_mat.emissive_factor.z);

                // Evaluate BRDF at the light direction.
                const float3 brdf_val_emi = brdf_eval(bp, L, NdotL_emi);

                // Light PDF (solid-angle).
                const float emission_lum = 0.2126f * tri.emission.x
                    + 0.7152f * tri.emission.y + 0.0722f * tri.emission.z;
                const float light_pdf_emi = emissive_light_pdf(
                    emission_lum, dist, cos_theta_light, params.emissive_total_power);

                // Combined multi-lobe BRDF PDF at the light direction.
                const float3 H_emi = normalize(bp.V + L);
                const float NdotH_emi = fmaxf(dot(N_shading, H_emi), 0.0f);
                const float pdf_spec_emi = pdf_ggx_vndf(NdotH_emi, bp.NdotV, bp.alpha);
                const float3 V_ts_emi = world_to_tangent(bp.T, bp.B, bp.N, bp.V);
                const float3 L_ts_emi = world_to_tangent(bp.T, bp.B, bp.N, L);
                const float pdf_diff_emi = pdf_EON(V_ts_emi, L_ts_emi, bp.r);
                const float brdf_pdf_emi = combined_lobe_pdf(
                    pdf_spec_emi, pdf_diff_emi, bp.p_spec);

                // MIS weight (light sampling strategy).
                const float mis_w_emi = mis_power_heuristic(light_pdf_emi, brdf_pdf_emi);

                const float st_factor_emi = shadow_terminator_factor(
                    N_face, N_shading, L);
                nee_radiance = nee_radiance + Le * brdf_val_emi * NdotL_emi
                    * mis_w_emi * st_factor_emi / fmaxf(light_pdf_emi, 1e-7f);
            }
        }
    }

    // ---- Env MIS weight for BRDF-sampled direction ----
    // When the BRDF ray misses (hits environment), the raygen loop scales its
    // contribution by this weight. Without NEE it is 1.0; with env NEE it is
    // power_heuristic(brdf_pdf, env_pdf(brdf_dir)) to avoid double-counting.
    float env_mis_w = 1.0f;
    if (bs.pdf_combined > 0.0f && params.env_alias_table != nullptr) {
        const float pdf_env_at_brdf = env_pdf(
            params.env_alias_table,
            params.env_alias_width, params.env_alias_height,
            params.env_total_luminance,
            params.env_rotation_sin, params.env_rotation_cos, bs.next_dir);
        env_mis_w = mis_power_heuristic(bs.pdf_combined, pdf_env_at_brdf);
    }

    // ---- Write 18-register payload ----
    payload_set_next_origin(offset_pos);
    payload_set_color(emissive + nee_radiance);
    payload_set_bounce(bounce);

    if (bs.pdf_combined == 0.0f) {
        // Invalid BRDF sample (specular reflected below surface): terminate
        // via zero throughput — raygen's throughput check breaks the loop
        // while this bounce's emissive/NEE contribution is kept. The hit
        // distance stays the real hit t: a negative value means "geometric
        // miss" to raygen (sky classification for aux data), which this
        // surface hit is not.
        payload_set_next_direction(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_throughput_update(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_hit_distance(optixGetRayTmax());
        payload_set_env_mis_weight(1.0f);
        payload_set_last_brdf_pdf(0.0f);
        return;
    }

    payload_set_next_direction(bs.next_dir);
    payload_set_throughput_update(bs.throughput_update);
    payload_set_hit_distance(optixGetRayTmax());
    payload_set_env_mis_weight(env_mis_w);
    payload_set_last_brdf_pdf(bs.pdf_combined);
}

/// Shadow miss (missIndex=1): the shadow ray reached tMax without hitting
/// geometry, so the light source is visible (not occluded). Sets visible=1
/// via payload register p0. The caller initializes p0=0 before tracing.
__global__ void __miss__shadow() { // NOLINT(*-reserved-identifier)
    optixSetPayload_0(1);
}

/// Any-hit alpha test for non-opaque geometry.
///
/// Opaque geometry never reaches this (BLAS flag DISABLE_ANYHIT). For
/// non-opaque geometry with alpha_mode==Mask, samples the base-color
/// texture alpha, multiplies by base_color_factor.a and interpolated
/// vertex color alpha, and discards the intersection if the result is
/// below alpha_cutoff.
__global__ void __anyhit__ah() { // NOLINT(*-reserved-identifier)
    using namespace qualquer::renderer;

    const uint32_t geo_index = optixGetInstanceId() + optixGetSbtGASIndex();
    const auto &geo = params.geometry_infos[geo_index];
    const auto &mat = params.materials[geo.material_buffer_offset];

    // Opaque materials should never reach any-hit (BLAS geometry flag
    // prevents it), but guard against misconfiguration.
    if (mat.alpha_mode == 0u) {
        return;
    }

    // ---- Lightweight UV + vertex color alpha interpolation ----
    const uint32_t prim_idx = optixGetPrimitiveIndex();
    const auto *indices = reinterpret_cast<const uint32_t *>(geo.index_buffer_address);
    const uint32_t i0 = indices[3 * prim_idx];
    const uint32_t i1 = indices[3 * prim_idx + 1];
    const uint32_t i2 = indices[3 * prim_idx + 2];

    const auto *verts = reinterpret_cast<const DeviceVertex *>(geo.vertex_buffer_address);

    const float2 bary = optixGetTriangleBarycentrics();
    const float w0 = 1.0f - bary.x - bary.y;
    const float w1 = bary.x;
    const float w2 = bary.y;

    const float2 uv = w0 * verts[i0].uv0 + w1 * verts[i1].uv0 + w2 * verts[i2].uv0;
    const float vert_alpha = w0 * verts[i0].color.w + w1 * verts[i1].color.w + w2 * verts[i2].color.w;

    // ---- Alpha test ----
    const cudaTextureObject_t *tex = params.texture_objects;
    const float texel_alpha = tex2D<float4>(tex[mat.base_color_tex], uv.x, uv.y).w;
    const float alpha = texel_alpha * mat.base_color_factor.w * vert_alpha;

    if (mat.alpha_mode == 1u) {
        if (alpha < mat.alpha_cutoff) {
            optixIgnoreIntersection();
        }
    }
}
} // extern "C"
