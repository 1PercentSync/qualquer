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
#include <qualquer/renderer/nee.cuh>

// OptiX locates program groups by the entry function symbol name (e.g.
// "__raygen__rg"), so entry points must be extern "C" at global scope. The
// launch-params constant is bound by name to pipelineLaunchParamsVariableName.
extern "C" {
__constant__ qualquer::renderer::LaunchParams params;

/// Ray generation: samples_per_frame paths per pixel with subpixel jitter,
/// accumulated in registers and written once to the Separate Sum buffer.
__global__ void __raygen__rg() { // NOLINT(*-reserved-identifier)
    using namespace qualquer::renderer;

    const uint3 idx = optixGetLaunchIndex();
    const uint32_t pixel_index = idx.y * params.width + idx.x;

    // Empty scene (no geometry loaded → traversable == 0): optixTrace on a
    // null traversable is undefined behavior. Copy the read buffer to the write
    // buffer so the Separate Sum chain stays valid and tonemap keeps showing
    // the last frame (or black on first frame).
    if (params.traversable == 0) {
        const float4 old = params.accumulation_buffer_read[pixel_index];
        params.accumulation_buffer[pixel_index] =
                make_float4(old.x, old.y, old.z, 1.0f);
        return;
    }

    // Camera origin (constant across all samples within this frame).
    const float4 origin_w = mul(params.inv_view, make_float4(0.0f, 0.0f, 0.0f, 1.0f));
    const float3 cam_origin = make_float3(origin_w.x, origin_w.y, origin_w.z);

    // Register-local accumulation across all samples in this frame.
    float3 frame_radiance = make_float3(0.0f, 0.0f, 0.0f);

    for (uint32_t s = 0; s < params.samples_per_frame; ++s) {
        const uint32_t sample_index = params.sample_count + s;

        // ---- Primary ray with subpixel jitter ----
        const float jx = rng(pixel_index, sample_index, kDimJitterX);
        const float jy = rng(pixel_index, sample_index, kDimJitterY);

        const float u = (static_cast<float>(idx.x) + jx) / static_cast<float>(params.width);
        const float v = (static_cast<float>(idx.y) + jy) / static_cast<float>(params.height);
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

        // ---- PathState ----
        PathState path{};
        path.origin = cam_origin;
        path.direction = normalize(make_float3(dir_w.x, dir_w.y, dir_w.z));
        path.throughput = make_float3(1.0f, 1.0f, 1.0f);
        path.radiance = make_float3(0.0f, 0.0f, 0.0f);
        path.pixel_index = pixel_index;
        path.sample_index = sample_index;
        path.bounce = 0;
        path.alive = true;

        // ---- Bounce loop ----
        // Payload registers persist across bounces: closesthit's env_mis_weight
        // (p13) survives into the next iteration's miss read.
        uint32_t p0 = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
        uint32_t p6 = 0, p7 = 0, p8 = 0, p9 = 0, p10 = 0, p11 = 0;
        uint32_t p12 = 0, p13 = __float_as_uint(1.0f), p14 = 0;
        uint32_t p15 = 0, p16 = 0, p17 = 0;

        while (path.alive && path.bounce < params.max_bounces) {
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
            // SER: reorder by material for texture cache coherence.
            // Single hitgroup record; the default hint only separates hit
            // vs miss. Material-based hint improves texture cache locality.
            uint32_t reorder_hint = 0;
            if (optixHitObjectIsHit()) {
                const uint32_t geo_idx = optixHitObjectGetInstanceId()
                                       + optixHitObjectGetSbtGASIndex();
                reorder_hint = params.geometry_infos[geo_idx].material_buffer_offset;
            }
            optixReorder(reorder_hint, 16);
            optixInvoke(p0, p1, p2, p3, p4, p5,
                        p6, p7, p8, p9, p10, p11,
                        p12, p13, p14, p15, p16, p17);

            const PayloadData d = payload_unpack(
                p0, p1, p2, p3, p4, p5,
                p6, p7, p8, p9, p10, p11,
                p12, p13, p14, p15, p16, p17);

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

        frame_radiance = frame_radiance + path.radiance;
    }

    // ---- Separate Sum accumulation (all samples written at once) ----
    if (params.sample_count == 0) {
        params.accumulation_buffer[pixel_index] =
                make_float4(frame_radiance.x, frame_radiance.y, frame_radiance.z, 1.0f);
    } else {
        const float4 old = params.accumulation_buffer_read[pixel_index];
        params.accumulation_buffer[pixel_index] =
                make_float4(old.x + frame_radiance.x,
                            old.y + frame_radiance.y,
                            old.z + frame_radiance.z, 1.0f);
    }
}

/// Samples the HDR environment cubemap along the missed ray direction.
/// Falls back to a constant background when no env cubemap is loaded.
__global__ void __miss__env() { // NOLINT(*-reserved-identifier)
    using namespace qualquer::renderer;

    float3 env_color = make_float3(0.0f, 0.0f, 0.0f);
    if (params.env_cubemap != 0) {
        const float3 dir = optixGetWorldRayDirection();
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
        payload_set_last_brdf_pdf(0.0f);
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

    // ---- Emissive ----
    const auto em_texel = tex2D<float4>(tex[mat.emissive_tex], uv.x, uv.y);
    const float3 emissive = make_float3(em_texel.x * mat.emissive_factor.x,
                                        em_texel.y * mat.emissive_factor.y,
                                        em_texel.z * mat.emissive_factor.z);

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
    const uint32_t sample_index = payload_get_sample_index();
    const uint32_t bounce = payload_get_bounce();
    const uint32_t dim_base = bounce_dim_base(bounce);

    const float u_lobe = rng(pixel_index, sample_index, dim_base + kBounceOffsetLobeSelect);
    const float u0 = rng(pixel_index, sample_index, dim_base + kBounceOffsetBrdfXi0);
    const float u1 = rng(pixel_index, sample_index, dim_base + kBounceOffsetBrdfXi1);

    const BrdfSample bs = brdf_sample(bp, u_lobe, u0, u1);

    // ---- NEE: Environment light (alias table importance sampling + MIS) ----
    float3 nee_radiance = make_float3(0.0f, 0.0f, 0.0f);

    if (params.env_cubemap != 0 && params.env_alias_table != nullptr) {
        const float env_r1 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEnvNee);
        const float env_r2 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEnvNee + 1);
        const float env_r3 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEnvNee + 2);
        const float env_r4 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEnvNee + 3);

        const float3 L = sample_env_alias_table(
            params.env_alias_table, params.env_alias_count,
            params.env_alias_width, params.env_alias_height,
            env_r1, env_r2, env_r3, env_r4);
        const float NdotL = dot(N_shading, L);

        if (NdotL > 0.0f) {
            const uint32_t visible = trace_shadow_ray(
                params.traversable, offset_pos, L, 1e16f);

            if (visible) {
                // Environment radiance at the sampled direction.
                const auto env_texel = texCubemap<float4>(
                    params.env_cubemap, L.x, L.y, L.z);
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
                    params.env_total_luminance, L);
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
        const float emi_r1 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEmissiveNee);
        const float emi_r2 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEmissiveNee + 1);
        const float emi_r3 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEmissiveNee + 2);
        const float emi_r4 = rng(pixel_index, sample_index, dim_base + kBounceOffsetEmissiveNee + 3);

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
            params.env_total_luminance, bs.next_dir);
        env_mis_w = mis_power_heuristic(bs.pdf_combined, pdf_env_at_brdf);
    }

    // ---- Write 18-register payload ----
    payload_set_next_origin(offset_pos);
    payload_set_color(emissive + nee_radiance);
    payload_set_bounce(bounce);

    if (bs.pdf_combined == 0.0f) {
        // Invalid BRDF sample (specular reflected below surface): terminate.
        payload_set_next_direction(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_throughput_update(make_float3(0.0f, 0.0f, 0.0f));
        payload_set_hit_distance(-1.0f);
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
