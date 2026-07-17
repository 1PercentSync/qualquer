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

/// Writes "no surface" aux G-buffer defaults (sky / single-sided pass-through).
///
/// depth=inf, specular albedo=0, normals=0, roughness=0; only diffuse_albedo is
/// parameterized so sky can pass tonemapped env color while pass-through passes
/// black — both mean "no meaningful surface" to DLSS-RR.
__forceinline__ __device__ void write_aux_no_surface(
    const int sx, const int sy, const float4 diffuse_albedo
) {
    const float inf = __int_as_float(0x7f800000);
    surf2Dwrite(inf, params.aux_depth,
                sx * static_cast<int>(sizeof(float)), sy);
    surf2Dwrite(diffuse_albedo, params.aux_diffuse_albedo,
                sx * static_cast<int>(sizeof(float4)), sy);
    surf2Dwrite(make_float4(0.0f, 0.0f, 0.0f, 0.0f), params.aux_specular_albedo,
                sx * static_cast<int>(sizeof(float4)), sy);
    surf2Dwrite(make_float4(0.0f, 0.0f, 0.0f, 0.0f), params.aux_normals,
                sx * static_cast<int>(sizeof(float4)), sy);
    surf2Dwrite(0.0f, params.aux_roughness,
                sx * static_cast<int>(sizeof(float)), sy);
}

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

    uint32_t p0 = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
    uint32_t p6 = 0, p7 = 0, p8 = 0, p9 = 0, p10 = 0, p11 = 0;
    uint32_t p12 = 0, p13 = 0, p14 = 0, p15 = 0;

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

        p14 = sample_index;
        p15 = path.bounce;

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
                      p12, p13, p14, p15);
        // SER: reorder secondary bounces by material for texture cache
        // coherence. Primary rays (bounce 0) are spatially coherent —
        // adjacent pixels hit nearby geometry with similar materials.
        // Reordering them by material scatters this spatial locality,
        // hurting texture cache and aux surface-write coalescing.
        // Secondary bounces lose spatial coherence after BRDF sampling,
        // so material grouping is a net win.
        if (path.bounce > 0) {
            uint32_t reorder_hint = 0;
            if (optixHitObjectIsHit()) {
                const uint32_t geo_idx = optixHitObjectGetInstanceId()
                                       + optixHitObjectGetSbtGASIndex();
                reorder_hint = params.geometry_infos[geo_idx].material_buffer_offset;
            }
            optixReorder(reorder_hint & 0x3FFu, 10);
        }
        optixInvoke(p0, p1, p2, p3, p4, p5,
                    p6, p7, p8, p9, p10, p11,
                    p12, p13, p14, p15);

        const PayloadData d = payload_unpack(
            p0, p1, p2, p3, p4, p5,
            p6, p7, p8, p9, p10, p11,
            p12, p13, p14, p15);

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

        // Accumulate radiance contribution from this bounce. Miss shader
        // applies env MIS weight directly to color, so no post-multiply needed.
        path.radiance = path.radiance + path.throughput * d.color;

        if (d.hit_distance < 0.0f) {
            path.alive = false;
            break;
        }

        path.throughput = path.throughput * d.throughput_update;

        if (fmaxf(path.throughput.x, fmaxf(path.throughput.y, path.throughput.z)) == 0.0f) {
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

    // Unproject to view space. Homogeneous divide (1/w) is skipped: w is a
    // positive uniform scale (1/far for glm::perspective RH_NO with ndc_z=1),
    // and normalize absorbs it.
    const float4 clip_target = make_float4(ndc_x, ndc_y, 1.0f, 1.0f);
    const float4 view_target = mul(params.inv_projection, clip_target);
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

    // First-bounce capture for MV + sky aux defaults (DLSS ON only).
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
            const float3 sample_radiance = apply_firefly_clamp(
                trace_sample(cam_origin, primary_dir, pixel_index, sample_index,
                             s == 0, primary_hit_pos, primary_sky_color,
                             primary_is_hit),
                params.max_clamp);
            frame_radiance = frame_radiance + sample_radiance;
        }
    } else {
        // DLSS OFF: per-pixel Sobol + CP rotation, per-sample jitter.
        // Each sample gets a different primary ray for Monte Carlo convergence.
        // No primary capture — aux data has no consumer when DLSS is off.
        for (uint32_t s = 0; s < params.samples_per_frame; ++s) {
            const uint32_t sample_index = params.sample_count + s;
            const float jx = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, kDimJitterX);
            const float jy = sobol_rng(params.sobol_directions, pixel_index,
                                       sample_index, params.frame_index, kDimJitterY);
            const float3 primary_dir = compute_primary_dir(idx.x, idx.y, jx, jy);

            const float3 sample_radiance = apply_firefly_clamp(
                trace_sample(cam_origin, primary_dir, pixel_index, sample_index,
                             false, primary_hit_pos, primary_sky_color,
                             primary_is_hit),
                params.max_clamp);
            frame_radiance = frame_radiance + sample_radiance;
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
    // DLSS OFF: no consumer for aux data, skip entirely (launch-uniform
    // branch, no warp divergence).
    if (params.dlss_enabled) {
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

            // Guard against degenerate perspective division. clip.w ≈ 0 when
            // the projected point/direction lies near the camera plane: sky
            // direction perpendicular to the previous camera forward (fast
            // rotation), or a hit point crossing the previous near plane (fast
            // translation). The MV is undefined in these cases; zero tells
            // DLSS-RR "static pixel", falling back to its internal heuristics.
            constexpr float kClipWMin = 1e-4f;
            float2 mv = make_float2(0.0f, 0.0f);
            if (fabsf(curr_clip.w) >= kClipWMin && fabsf(prev_clip.w) >= kClipWMin) {
                const float2 curr_ndc = make_float2(curr_clip.x / curr_clip.w,
                                                    curr_clip.y / curr_clip.w);
                const float2 prev_ndc = make_float2(prev_clip.x / prev_clip.w,
                                                    prev_clip.y / prev_clip.w);
                mv = make_float2(
                    (prev_ndc.x - curr_ndc.x) * 0.5f * resolution.x,
                    (prev_ndc.y - curr_ndc.y) * 0.5f * resolution.y);
            }
            surf2Dwrite(mv, params.aux_motion_vectors,
                        sx * static_cast<int>(sizeof(float2)), sy);
        }

        // Sky pixels: closesthit never ran, so write aux defaults.
        // Tonemapped sky radiance as diffuse albedo guide (HDR compressed to
        // [0,1] so DLSS-RR preserves sky detail without denoising it).
        if (!primary_is_hit) {
            const float3 sky_albedo = pbr_neutral_tonemap(primary_sky_color);
            write_aux_no_surface(
                sx, sy,
                make_float4(sky_albedo.x, sky_albedo.y, sky_albedo.z, 1.0f));
        }
    }
}

/// Samples the HDR environment cubemap along the missed ray direction.
/// Applies IBL Y-axis rotation (world → env space) before cubemap lookup.
/// When env NEE is active and this miss follows a BRDF sample, applies the
/// MIS weight here instead of precomputing it in closesthit — env_pdf()
/// (atan2f + asinf + alias table random read) only runs on actual misses.
/// Falls back to a constant background when no env cubemap is loaded.
__global__ void __miss__env() { // NOLINT(*-reserved-identifier)
    using namespace qualquer::renderer;

    const float3 world_dir = optixGetWorldRayDirection();

    float3 env_color = make_float3(0.0f, 0.0f, 0.0f);
    if (params.env.cubemap != 0) {
        // Forward IBL rotation: world space → env space.
        const float3 dir = rotate_y_dir(world_dir,
                                        params.env.rotation_sin, params.env.rotation_cos);
        const auto texel = texCubemap<float4>(params.env.cubemap, dir.x, dir.y, dir.z);
        env_color = make_float3(texel.x, texel.y, texel.z);
    }

    // Env MIS: when this miss follows a BRDF sample (last_brdf_pdf > 0)
    // and env NEE is active, weight by power_heuristic(brdf_pdf, env_pdf)
    // to avoid double-counting with the closesthit NEE strategy.
    // Primary ray (bounce 0), pass-through, or invalid BRDF sample:
    // last_brdf_pdf == 0 → weight stays 1.0.
    const float last_brdf_pdf = payload_get_last_brdf_pdf();
    if (last_brdf_pdf > 0.0f && params.env.alias_table != nullptr) {
        const float pdf_env = env_pdf(params.env, world_dir);
        env_color = env_color * mis_power_heuristic(last_brdf_pdf, pdf_env);
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

    // ---- Early pass-through check (positions + face normal only) ----
    // Single-sided back-face pass-through needs only positions (face normal),
    // ray direction, and double_sided. Check before the full interpolation to
    // skip normal/UV/color/tangent work on the pass-through path.
    const float3 face_normal_obj = cross(v1.position - v0.position,
                                         v2.position - v0.position);
    float3 N_face = normalize(optixTransformNormalFromObjectToWorldSpace(face_normal_obj));
    const float3 ray_dir = optixGetWorldRayDirection();
    const bool is_back_face = dot(N_face, ray_dir) > 0.0f;

    if (is_back_face && mat.double_sided == 0u) {
        // Single-sided material hit from behind: pass through the surface.
        // Throughput unchanged, consumes one bounce. Origin is pushed past
        // the hit point along the ray direction to avoid re-intersection.
        const float hit_t = optixGetRayTmax();
        const float pass_eps = fmaxf(hit_t * 1e-4f, 1e-6f);
        const float3 pass_origin = optixGetWorldRayOrigin() + ray_dir * (hit_t + pass_eps);

        // Aux defaults for the invisible pass-through surface. Without
        // this write the aux data stays stale from the previous frame
        // (the SDK warns: "A common integration error is to leave sky
        // pixels uncleared"). The pass-through surface has no meaningful
        // material attributes, so write "no surface" values matching the
        // sky/miss convention used by the reference projects.
        if (params.dlss_enabled
            && payload_get_bounce() == 0
            && payload_get_sample_index() == params.sample_count) {
            const uint3 li = optixGetLaunchIndex();
            write_aux_no_surface(static_cast<int>(li.x), static_cast<int>(li.y),
                                 make_float4(0.0f, 0.0f, 0.0f, 0.0f));
        }

        payload_set_next_origin(pass_origin);
        payload_set_next_direction(ray_dir);
        payload_set_throughput_update(make_float3(1.0f, 1.0f, 1.0f));
        payload_set_color(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_hit_distance(hit_t);
        // Shadow rays use TERMINATE_ON_FIRST_HIT without closesthit, so they
        // cannot pass through single-sided back-faces. NEE at the previous
        // shading point is therefore blind to anything beyond this surface.
        // Clear last_brdf_pdf so that a subsequent emissive hit takes full
        // weight (no competing NEE strategy), and miss shader skips env MIS.
        payload_set_last_brdf_pdf(0.0f);
        return;
    }

    // ---- Full barycentric interpolation (only reached for shaded surfaces) ----
    const float2 bary = optixGetTriangleBarycentrics();
    const float w0 = 1.0f - bary.x - bary.y;
    const float w1 = bary.x;
    const float w2 = bary.y;

    const float3 obj_pos = w0 * v0.position + w1 * v1.position + w2 * v2.position;
    const float3 obj_normal = w0 * v0.normal + w1 * v1.normal + w2 * v2.normal;
    const float2 uv = w0 * v0.uv0 + w1 * v1.uv0 + w2 * v2.uv0;
    const float4 vert_color = w0 * v0.color + w1 * v1.color + w2 * v2.color;
    const float4 tangent = w0 * v0.tangent + w1 * v1.tangent + w2 * v2.tangent;

    // ---- World-space transforms ----
    const float3 world_pos = optixTransformPointFromObjectToWorldSpace(obj_pos);

    // Interpolated normal: barycentric interpolation of three unit normals
    // can cancel to near-zero (e.g. cone-tip topology with 120°-spread
    // normals). normalize(zero) → NaN, which would poison the entire
    // shading pipeline. Fall back to the geometric face normal, which is
    // guaranteed non-zero by the OptiX hit (non-degenerate triangle).
    const float3 N_interp_raw = optixTransformNormalFromObjectToWorldSpace(obj_normal);
    float3 N_interp;
    {
        const float len_sq = dot(N_interp_raw, N_interp_raw);
        N_interp = len_sq >= 1e-12f
            ? N_interp_raw * rsqrtf(len_sq)
            : N_face;
    }

    // Tangent: passed unnormalized to get_shading_normal(), which has its
    // own length guard + rsqrtf normalization. Normalizing here would turn
    // a zero-length interpolated tangent into NaN, bypassing that guard
    // (NaN < threshold is false in IEEE 754).
    const float3 T_world = optixTransformVectorFromObjectToWorldSpace(
        make_float3(tangent.x, tangent.y, tangent.z));

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
    const float alpha = roughness * roughness;

    // ---- Normal mapping + consistency ----
    const auto nm_texel = tex2D<float4>(tex[mat.normal_tex], uv.x, uv.y);
    const float3 N_mapped = get_shading_normal(
        N_interp, T_world, tangent.w,
        make_float2(nm_texel.x, nm_texel.y), mat.normal_scale);
    const float3 N_shading = ensure_normal_consistency(N_mapped, N_face);

    // ---- Emissive (with BRDF-strategy MIS weight for bounce > 0) ----
    // Skip the emissive texture fetch when emissive_factor is zero (the
    // vast majority of materials). SER groups threads by material, so
    // this branch is warp-coherent and cheaper than a bindless tex2D to
    // the default white texture whose result would be multiplied to zero.
    const uint32_t bounce = payload_get_bounce();
    const float emi_lum = luminance(make_float3(
        mat.emissive_factor.x, mat.emissive_factor.y, mat.emissive_factor.z));
    float3 emissive = make_float3(0.0f, 0.0f, 0.0f);
    if (emi_lum > 0.0f) {
        const auto em_texel = tex2D<float4>(tex[mat.emissive_tex], uv.x, uv.y);
        emissive = make_float3(em_texel.x * mat.emissive_factor.x,
                               em_texel.y * mat.emissive_factor.y,
                               em_texel.z * mat.emissive_factor.z);

        // Bounce 0 (direct view): weight 1.0 — primary ray is not a BRDF sample.
        // Bounce > 0 with emissive NEE active: BRDF hit emissive is one of two MIS
        // strategies, weight by power_heuristic(brdf_pdf, light_pdf).
        // Bounce > 0 without emissive NEE: weight 1.0 — no competing strategy.
        if (bounce > 0 && params.emissive.count > 0) {
            const float last_brdf_pdf = payload_get_last_brdf_pdf();
            // MIS only when a competing BRDF strategy exists. last_brdf_pdf == 0
            // means no real BRDF sample preceded this bounce (camera ray followed
            // by one or more single-sided pass-throughs); the emissive contribution
            // keeps full weight as if directly visible.
            if (last_brdf_pdf > 0.0f) {
                const float cos_theta_l = fabsf(dot(N_face, ray_dir));
                const float hit_dist = optixGetRayTmax();
                const float light_pdf = emissive_light_pdf(
                    emi_lum, hit_dist, cos_theta_l, params.emissive.total_power);
                const float mis_w = mis_power_heuristic(last_brdf_pdf, light_pdf);
                emissive = emissive * mis_w;
            }
        }
    }

    // ---- Ray offset ----
    const float3 offset_pos = offset_ray_origin(world_pos, N_face);

    // ---- BRDF sampling ----
    const float3 V = -ray_dir;

    // Grazing-angle fallback: at low-poly silhouettes, normal mapping can
    // push N_shading into V's back hemisphere (dot(N,V) <= 0). Rotate N
    // minimally toward V so that NdotV is just barely positive. This keeps
    // shading continuous at the cost of higher variance (NdotV ≈ ε), which
    // DLSS-RR temporal denoising handles well.
    float3 N_brdf = N_shading;
    {
        const float NdotV = dot(N_shading, V);
        if (NdotV <= 0.0f) {
            // Shift N toward V: N' = normalize(N + V * (-NdotV + ε))
            // This is the smallest additive correction that places N' just
            // inside V's hemisphere.
            constexpr float kGrazingEps = 1e-3f;
            N_brdf = normalize(N_shading + V * (-NdotV + kGrazingEps));
        }
    }

    float3 T_basis, B_basis;
    build_orthonormal_basis(N_brdf, T_basis, B_basis);
    const BrdfParams bp = init_brdf_params(
        V, N_brdf, T_basis, B_basis,
        base_color, metallic, roughness, alpha,
        params.dlss_enabled, bounce);

    const uint3 launch_idx = optixGetLaunchIndex();
    const uint32_t pixel_index = launch_idx.y * params.width + launch_idx.x;

    // ---- Aux G-buffer writes (first sample's bounce 0 only) ----
    // D37: all samples share per-frame jitter → primary ray identical →
    // aux data identical. Write once at the first sample of the batch.
    // DLSS OFF: no consumer for aux data, skip entirely (launch-uniform
    // branch, no warp divergence).
    if (params.dlss_enabled
        && bounce == 0 && payload_get_sample_index() == params.sample_count) {
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

    // ---- NEE: environment + emissive (full evaluation lives in nee.cuh) ----
    // NEE is valid on every bounce including the last (direct light contribution).
    const float3 nee_radiance =
        evaluate_env_nee(bp, offset_pos, N_face, N_brdf,
                         pixel_index, sample_index, dim_base)
        + evaluate_emissive_nee(bp, offset_pos, N_face, N_brdf,
                                pixel_index, sample_index, dim_base);

    // ---- Write common payload ----
    payload_set_next_origin(offset_pos);
    payload_set_color(emissive + nee_radiance);
    payload_set_hit_distance(optixGetRayTmax());

    // Last bounce: emissive + NEE already collected above. The next ray would
    // never be traced (raygen loop condition), so skip BRDF sampling and RNG.
    // Terminate via zero throughput.
    if (bounce >= params.max_bounces - 1) {
        payload_set_next_direction(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_throughput_update(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_last_brdf_pdf(0.0f);
        return;
    }

    // ---- BRDF sampling (not last bounce) ----
    const float u_lobe = sobol_rng(params.sobol_directions, pixel_index,
                                   sample_index, params.frame_index, dim_base + kBounceOffsetLobeSelect);
    const float u0 = sobol_rng(params.sobol_directions, pixel_index,
                               sample_index, params.frame_index, dim_base + kBounceOffsetBrdfXi0);
    const float u1 = sobol_rng(params.sobol_directions, pixel_index,
                               sample_index, params.frame_index, dim_base + kBounceOffsetBrdfXi1);

    const BrdfSample bs = brdf_sample(bp, u_lobe, u0, u1);

    if (bs.pdf_combined == 0.0f) {
        // Invalid BRDF sample (specular reflected below surface): terminate
        // via zero throughput — raygen's throughput check breaks the loop
        // while this bounce's emissive/NEE contribution is kept. The hit
        // distance stays the real hit t: a negative value means "geometric
        // miss" to raygen (sky classification for aux data), which this
        // surface hit is not.
        payload_set_next_direction(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_throughput_update(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_last_brdf_pdf(0.0f);
        return;
    }

    payload_set_next_direction(bs.next_dir);
    payload_set_throughput_update(bs.throughput_update);
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
