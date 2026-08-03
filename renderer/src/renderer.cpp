/**
 * @file renderer.cpp
 * @brief Renderer implementation.
 */

#include <qualquer/renderer/renderer.h>

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cuda_runtime.h>

#include <vulkan/vulkan.h>

#include <qualquer/optix/context.h>
#include <qualquer/optix/dlss_rr.h>
#include <qualquer/optix/cuda_check.h>
#include <qualquer/optix/optix_check.h>
#include <qualquer/optix/pipeline.h>
#include <qualquer/renderer/camera.h>
#include <qualquer/vulkan/imgui_backend.h>
#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/sobol_direction_data.h>
#include <qualquer/renderer/tonemap.h>
#include <qualquer/vulkan/interop.h>
#include <qualquer/vulkan/swapchain.h>

#include <optix_stubs.h>
#include <spdlog/spdlog.h>

namespace qualquer::renderer {
    namespace {
        /**
         * @brief Host-side Sobol sample (mirrors the device sobol_sample).
         *
         * Pure integer math — identical algorithm to rng.cuh sobol_sample.
         * Used to compute the global per-frame jitter for DLSS-RR (without
         * per-pixel Cranley-Patterson rotation, so all pixels share the
         * same sub-pixel offset).
         */
        uint32_t sobol_sample_host(const uint32_t *directions,
                                    const uint32_t dim,
                                    uint32_t sample_index) {
            uint32_t result = 0;
            const uint32_t offset = dim * 32u;
            while (sample_index != 0u) {
                const uint32_t bit = std::countr_zero(sample_index);
                result ^= directions[offset + bit];
                sample_index &= sample_index - 1u;
            }
            return result;
        }

        /**
         * @brief Computes global per-frame jitter for DLSS-RR mode.
         *
         * Uses Sobol dim 0/1 with NO per-pixel Cranley-Patterson rotation
         * (all pixels share the same sub-pixel offset). No temporal offset —
         * frame_index as sequence_index already advances through the Sobol
         * (0,2)-sequence.
         */
        float global_jitter(const uint32_t *directions,
                            const uint32_t frame_index,
                            const uint32_t dimension) {
            const uint32_t sobol_val = sobol_sample_host(directions, dimension, frame_index);
            return static_cast<float>(sobol_val >> 8) * 0x1p-24f;
        }

        /**
         * @brief Converts a glm column-major mat4 to the row-major float4x4 the
         *        device reads (rows[i] = i-th row).
         *
         * glm stores m[col][row]; float4x4::rows[i] is the i-th row, so each
         * output row gathers the i-th component across all four columns.
         */
        float4x4 to_float4x4(const glm::mat4 &m) {
            return {
                .rows = {
                    make_float4(m[0][0], m[1][0], m[2][0], m[3][0]),
                    make_float4(m[0][1], m[1][1], m[2][1], m[3][1]),
                    make_float4(m[0][2], m[1][2], m[2][2], m[3][2]),
                    make_float4(m[0][3], m[1][3], m[2][3], m[3][3]),
                },
            };
        }

        /**
         * @brief Per-group intermediate data shared across the AS build stages.
         *
         * Produced by group_meshes(); group_to_blas is filled by
         * build_blas_groups(). The remaining fields are complete after group_meshes().
         */
        struct SceneGrouping {
            /** @brief Per-group BLAS geometry inputs (one glTF mesh = one multi-geometry BLAS). */
            std::vector<std::vector<optix::BLASGeometry>> group_geometries;

            /** @brief Per-group material ids, parallel to group_geometries. */
            std::vector<std::vector<uint32_t>> group_mat_ids;

            /** @brief Per-group primitive count including degenerate ones (TLAS stepping). */
            std::vector<uint32_t> group_prim_count;

            /** @brief group_id -> index into AccelStructure::blas_handles() (UINT32_MAX = none). */
            std::vector<uint32_t> group_to_blas;

            /** @brief group_id -> starting index in the geometry-info array (= TLAS instanceId). */
            std::vector<uint32_t> group_base_offset;

            /** @brief Total non-degenerate geometry count (geometry-info array length). */
            uint32_t total_geometries = 0;
        };

        /**
         * @brief Groups meshes by group_id and computes the geometry-info layout.
         *
         * A degenerate primitive (no vertices or fewer than 3 indices) still
         * occupies a primitive slot in group_prim_count — SceneLoader emits
         * same-node primitives contiguously, so TLAS stepping advances by this
         * count to skip a whole mesh instance in one stride — but contributes no
         * geometry to its group's BLAS.
         */
        SceneGrouping group_meshes(const std::span<const Mesh> meshes) {
            uint32_t max_group = 0;
            for (const auto &mesh : meshes) {
                max_group = std::max(max_group, mesh.group_id);
            }
            const uint32_t group_count = max_group + 1;

            SceneGrouping grouping;
            grouping.group_geometries.resize(group_count);
            grouping.group_mat_ids.resize(group_count);
            grouping.group_prim_count.assign(group_count, 0);

            for (const auto &mesh : meshes) {
                if (mesh.vertex_count == 0 || mesh.index_count < 3) {
                    ++grouping.group_prim_count[mesh.group_id];
                    continue;
                }
                grouping.group_geometries[mesh.group_id].push_back({
                    .vertex_buffer = mesh.vertex_buffer.device_ptr(),
                    .index_buffer = mesh.index_buffer.device_ptr(),
                    .vertex_count = mesh.vertex_count,
                    .index_count = mesh.index_count,
                    .vertex_stride = sizeof(Vertex),
                    .opaque = mesh.opaque,
                });
                grouping.group_mat_ids[mesh.group_id].push_back(mesh.material_id);
                ++grouping.group_prim_count[mesh.group_id];
            }

            // Geometries are laid out contiguously per group; group_base_offset[g]
            // becomes the TLAS instance's instanceId so closest-hit resolves a hit
            // via instanceId + the GAS-relative geometry index.
            grouping.group_base_offset.assign(group_count, 0);
            for (uint32_t g = 0; g < group_count; ++g) {
                grouping.group_base_offset[g] = grouping.total_geometries;
                grouping.total_geometries += static_cast<uint32_t>(grouping.group_geometries[g].size());
            }
            return grouping;
        }

        /**
         * @brief Builds one BLAS per non-empty group, filling group_to_blas.
         */
        void build_blas_groups(optix::AccelStructure &accel,
                               const optix::Context &cuda_context,
                               SceneGrouping &grouping) {
            grouping.group_to_blas.assign(grouping.group_geometries.size(), UINT32_MAX);

            // Collect non-empty groups for batched build.
            std::vector<uint32_t> active_groups;
            std::vector<std::span<const optix::BLASGeometry>> active_geoms;
            for (uint32_t g = 0; g < grouping.group_geometries.size(); ++g) {
                if (!grouping.group_geometries[g].empty()) {
                    active_groups.push_back(g);
                    active_geoms.push_back(grouping.group_geometries[g]);
                }
            }

            const auto base = static_cast<uint32_t>(accel.blas_handles().size());
            accel.build_all_blas(cuda_context.device_context,
                                 cuda_context.compute_stream,
                                 active_geoms);

            for (uint32_t i = 0; i < active_groups.size(); ++i) {
                grouping.group_to_blas[active_groups[i]] = base + i;
            }
        }

        /**
         * @brief Builds and uploads the geometry-info array from the grouping.
         */
        void build_geometry_info(optix::CudaBuffer<GPUGeometryInfo> &buffer,
                                 // ReSharper disable once CppParameterMayBeConst
                                 cudaStream_t stream,
                                 const SceneGrouping &grouping) {
            std::vector<GPUGeometryInfo> geometry_infos(grouping.total_geometries);
            for (uint32_t g = 0; g < grouping.group_geometries.size(); ++g) {
                const auto &geoms = grouping.group_geometries[g];
                const auto &mat_ids = grouping.group_mat_ids[g];
                const uint32_t base = grouping.group_base_offset[g];
                for (uint32_t j = 0; j < static_cast<uint32_t>(geoms.size()); ++j) {
                    geometry_infos[base + j] = {
                        .vertex_buffer_address = geoms[j].vertex_buffer,
                        .index_buffer_address = geoms[j].index_buffer,
                        .material_buffer_offset = mat_ids[j],
                        .padding = 0,
                    };
                }
            }
            buffer.alloc(grouping.total_geometries);
            buffer.upload(geometry_infos.data(), grouping.total_geometries, stream);
        }

        /**
         * @brief Assembles TLAS instances, folding same-node primitives (contiguous,
         *        shared transform) into one instance per mesh instance.
         */
        std::vector<OptixInstance> build_tlas_instances(const std::span<const Mesh> meshes,
                                                        const std::span<const MeshInstance> instances,
                                                        const SceneGrouping &grouping,
                                                        const optix::AccelStructure &accel) {
            const auto &blas_handles = accel.blas_handles();
            std::vector<OptixInstance> tlas_instances;
            uint32_t inst_idx = 0;
            while (inst_idx < static_cast<uint32_t>(instances.size())) {
                const auto &first_inst = instances[inst_idx];
                const uint32_t mesh_id = first_inst.mesh_id;
                const uint32_t group_id = meshes[mesh_id].group_id;
                const uint32_t prim_count = grouping.group_prim_count[group_id];

                // Skip instances whose group yielded no BLAS (all primitives degenerate).
                if (grouping.group_to_blas[group_id] == UINT32_MAX || prim_count == 0) {
                    inst_idx += prim_count;
                    continue;
                }

                OptixInstance inst{};
                // glm is column-major (m[col][row]); OptixInstance.transform is a
                // row-major 3x4 laid out as float[12] — transposing element-wise.
                for (int row = 0; row < 3; ++row) {
                    for (int col = 0; col < 4; ++col) {
                        inst.transform[row * 4 + col] = first_inst.transform[col][row];
                    }
                }
                inst.instanceId = grouping.group_base_offset[group_id];
                inst.sbtOffset = 0;
                inst.visibilityMask = 0xFF;
                // glTF spec §1688: negative determinant reverses triangle winding.
                // FLIP_TRIANGLE_FACING corrects OptiX front/back classification so
                // optixIsTriangleBackFaceHit() returns the spec-correct answer.
                const float det = glm::determinant(glm::mat3(first_inst.transform));
                inst.flags = det < 0.0f
                    ? OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING
                    : OPTIX_INSTANCE_FLAG_NONE;
                inst.traversableHandle = blas_handles[grouping.group_to_blas[group_id]].handle;
                tlas_instances.push_back(inst);

                inst_idx += prim_count;
            }
            return tlas_instances;
        }
    } // namespace

    void Renderer::AuxBufferSet::alloc(const uint32_t width, const uint32_t height) {
        depth.alloc(width, height, cudaCreateChannelDesc<float>(), cudaReadModeElementType);
        motion_vectors.alloc(width, height, cudaCreateChannelDesc<float2>(), cudaReadModeElementType);
        diffuse_albedo.alloc(width, height, cudaCreateChannelDesc<uchar4>(), cudaReadModeNormalizedFloat);
        specular_albedo.alloc(width, height, cudaCreateChannelDesc<float4>(), cudaReadModeElementType);
        normal_roughness.alloc(width, height, cudaCreateChannelDescHalf4(), cudaReadModeElementType);
    }

    void Renderer::AuxBufferSet::resize(const uint32_t width, const uint32_t height) {
        depth.resize(width, height);
        motion_vectors.resize(width, height);
        diffuse_albedo.resize(width, height);
        specular_albedo.resize(width, height);
        normal_roughness.resize(width, height);
    }

    void Renderer::AuxBufferSet::free() {
        depth.free();
        motion_vectors.free();
        diffuse_albedo.free();
        specular_albedo.free();
        normal_roughness.free();
    }

    void Renderer::FrameSlot::alloc(const uint32_t width, const uint32_t height) {
        color.alloc(width, height, cudaCreateChannelDesc<float4>(), cudaReadModeElementType);
        // aux guides are allocated/freed by the DLSS lifecycle, not here.
        sample_count = 0;
    }

    void Renderer::FrameSlot::resize(const uint32_t width, const uint32_t height) {
        color.resize(width, height);
        if (aux.valid()) {
            aux.resize(width, height);
        }
        // Resized content is undefined.
        sample_count = 0;
    }

    void Renderer::FrameSlot::free() {
        color.free();
        aux.free();
        sample_count = 0;
    }

    // ReSharper disable once CppParameterMayBeConst
    void Renderer::FrameSlot::create_events(cudaStream_t stream) {
        CUDA_CHECK(cudaEventCreateWithFlags(&production_event, cudaEventDisableTiming));
        CUDA_CHECK(cudaEventRecord(production_event, stream));
        CUDA_CHECK(cudaEventCreateWithFlags(&consumption_event, cudaEventDisableTiming));
        CUDA_CHECK(cudaEventRecord(consumption_event, stream));
    }

    void Renderer::FrameSlot::destroy_events() {
        if (production_event != nullptr) {
            CUDA_CHECK(cudaEventDestroy(production_event));
            production_event = nullptr;
        }
        if (consumption_event != nullptr) {
            CUDA_CHECK(cudaEventDestroy(consumption_event));
            consumption_event = nullptr;
        }
    }

    void Renderer::invalidate_dlss_history() {
        prev_dlss_metadata_.valid = false;
        dlss_reset_requested_ = true;
    }

    void Renderer::invalidate_dlss_state() {
        invalidate_dlss_history();
        dlss_output_valid_ = false;
    }

    void Renderer::init(const optix::Context &cuda_context,
                        const uint32_t width,
                        const uint32_t height,
                        const std::string &optixir_path) {
        // NGX lifecycle brackets the OptiX/CUDA resources that feed DLSS-RR.
        // Init before the pipeline so a failed availability probe still leaves
        // the renderer usable (feature create is deferred to submit_cuda).
        dlss_rr_.init(cuda_context.device_id());

        // Bounce payload: 15 registers with per-register read/write semantics.
        // The compiler uses this table to know which registers to save/restore
        // across trace calls — shadow traces pass zero payload, so the compiler
        // skips saving all 15 bounce registers across them.
        //
        // Layout: p0-p2 origin, p3-p5 direction, p6-p8 throughput,
        //         p9-p11 color, p12 hit_distance, p13 last_brdf_pdf,
        //         p14 packed (bounce | sample_s | primary_aux_needed).
        constexpr unsigned int kBouncePayloadSemantics[15] = {
            // p0-p2: next_origin — caller reads after trace, CH writes
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            // p3-p5: next_direction — caller reads after trace, CH writes
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            // p6-p8: throughput_update — caller reads after trace, CH writes
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE,
            // p9-p11: color — caller reads after trace, CH writes, MS writes
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE
                | OPTIX_PAYLOAD_SEMANTICS_MS_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE
                | OPTIX_PAYLOAD_SEMANTICS_MS_WRITE,
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE
                | OPTIX_PAYLOAD_SEMANTICS_MS_WRITE,
            // p12: hit_distance — caller reads after trace, CH writes, MS writes
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ | OPTIX_PAYLOAD_SEMANTICS_CH_WRITE
                | OPTIX_PAYLOAD_SEMANTICS_MS_WRITE,
            // p13: last_brdf_pdf — caller reads+writes, CH reads+writes, MS reads
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ_WRITE | OPTIX_PAYLOAD_SEMANTICS_CH_READ_WRITE
                | OPTIX_PAYLOAD_SEMANTICS_MS_READ,
            // p14: packed (bounce|sample_s|primary_aux) — caller writes, CH reads
            OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_WRITE | OPTIX_PAYLOAD_SEMANTICS_CH_READ,
        };

        const OptixPayloadType bounce_payload_type{
            .numPayloadValues = 15,
            .payloadSemantics = kBouncePayloadSemantics,
        };

        // The launch-params constant name ("params") and its size are device-side
        // facts this layer cannot take from the renderer header (single-direction
        // dependency).
        pipeline_.init(cuda_context.device_context,
                       optixir_path,
                       sizeof(LaunchParams),
                       "params",
                       &bounce_payload_type,
                       1);

        // SBT records are header-only (global data via LaunchParams).
        // Miss SBT has 1 entry: env (missIndex=0). Shadow rays use zero-payload
        // optixTraverse + optixHitObjectIsHit() and do not invoke any miss program.
        SbtRecord record{};
        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.raygen_program, &record));
        sbt_raygen_.alloc(1);
        sbt_raygen_.upload(&record, 1, cuda_context.compute_stream);

        SbtRecord miss_record{};
        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.miss_env_program, &miss_record));
        sbt_miss_.alloc(1);
        sbt_miss_.upload(&miss_record, 1, cuda_context.compute_stream);

        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.hitgroup_program, &record));
        sbt_hit_.alloc(1);
        sbt_hit_.upload(&record, 1, cuda_context.compute_stream);

        // Single FrameSlot (color + events). Aux guides and dlss_output_ are
        // allocated on demand when DLSS is enabled. sample_count = 0 makes
        // tonemap output black until the first raygen writes valid data.
        // create_events records both events so the first waits pass without
        // reading unrecorded event objects.
        frame_slot_.alloc(width, height);
        frame_slot_.create_events(cuda_context.compute_stream);

        render_width_ = width;
        render_height_ = height;

        params_buffer_.alloc(1);
        for (auto &staging : params_staging_) {
            CUDA_CHECK(cudaHostAlloc(&staging, sizeof(LaunchParams), cudaHostAllocDefault));
        }

#ifndef NDEBUG
        // Timing events (no DisableTiming) for pipeline profiling.
        for (auto *arr : {&event_display_start_, &event_display_end_,
                          &event_pt_start_, &event_pt_end_}) {
            for (auto &event : *arr) {
                CUDA_CHECK(cudaEventCreate(&event));
                CUDA_CHECK(cudaEventRecord(event, cuda_context.compute_stream));
            }
        }
#endif

        spdlog::info("Renderer initialized ({}x{}, {} SBT records)",
                     width,
                     height,
                     4);
    }

    void Renderer::destroy() {
        // Reverse init creation order: the pipeline references the module and
        // program groups, so it is torn down before the SBT buffers whose device
        // memory it bound. Pipeline::destroy and CudaBuffer::free are both
        // idempotent (null-reset), so a repeat call is a no-op. State members
        // (frame_counter_, FrameSlot::sample_count) are intentionally not reset
        // here — release is the sole responsibility; a subsequent init resets
        // them. DLSS-RR is last among render resources so feature release and
        // NGX shutdown happen after any stream work that referenced them.
        pipeline_.destroy();
        sbt_raygen_.free();
        sbt_miss_.free();
        sbt_hit_.free();
        accel_.destroy();
        geometry_info_buffer_.free();
        frame_slot_.free();
        frame_slot_.destroy_events();
        dlss_output_.free();
        params_buffer_.free();
        for (auto &staging : params_staging_) {
            if (staging != nullptr) {
                CUDA_CHECK(cudaFreeHost(staging));
                staging = nullptr;
            }
        }
#ifndef NDEBUG
        for (auto *arr : {&event_display_start_, &event_display_end_,
                          &event_pt_start_, &event_pt_end_}) {
            for (auto &event : *arr) {
                if (event != nullptr) {
                    CUDA_CHECK(cudaEventDestroy(event));
                    event = nullptr;
                }
            }
        }
#endif
        dlss_rr_.destroy();
    }

    void Renderer::load_scene(const optix::Context &cuda_context,
                              const std::span<const Mesh> meshes,
                              const std::span<const MeshInstance> instances) {
        invalidate_dlss_state();

        // Runtime scene switching: tear down the previous scene's AS and
        // geometry-info buffer before rebuilding.
        accel_.destroy();
        geometry_info_buffer_.free();

        if (meshes.empty()) {
            // No geometry to trace; submit_cuda must keep the traversable at 0 so
            // raygen skips optixTrace.
            tlas_instance_count_ = 0;
            spdlog::warn("Renderer::load_scene: empty scene, no acceleration structures built");
            return;
        }

        // Group meshes by group_id, build a multi-geometry BLAS per group, and
        // upload the per-geometry query data. See the anonymous-namespace helpers
        // above for the grouping invariants (degenerate primitives, layout offsets).
        SceneGrouping grouping = group_meshes(meshes);
        build_blas_groups(accel_, cuda_context, grouping);
        build_geometry_info(geometry_info_buffer_, cuda_context.compute_stream, grouping);

        std::vector<OptixInstance> tlas_instances = build_tlas_instances(meshes, instances, grouping, accel_);
        tlas_instance_count_ = static_cast<uint32_t>(tlas_instances.size());
        if (tlas_instances.empty()) {
            spdlog::warn("Renderer::load_scene: no TLAS instances (all meshes degenerate?)");
            return;
        }

        accel_.build_tlas(cuda_context.device_context,
                          cuda_context.compute_stream,
                          tlas_instances);

        spdlog::info("Renderer::load_scene: {} meshes, {} instances, {} BLAS, {} TLAS instances",
                     meshes.size(), instances.size(), accel_.blas_handles().size(), tlas_instances.size());
    }

    void Renderer::submit_cuda(const optix::Context &cuda_context,
                               const SceneRenderInput &scene,
                               const uint32_t width,
                               const uint32_t height,
                               const uint32_t frame_index) {
        // DLSS output buffer tracks display resolution (window resize), not
        // render resolution. Only resize when already allocated (DLSS is on);
        // first allocation happens when DLSS is enabled below.
        bool display_res_changed = false;
        if (width != dlss_output_width_ || height != dlss_output_height_) {
            if (dlss_output_.valid()) {
                CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
                CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
                dlss_output_.resize(width, height);
                dlss_output_valid_ = false;
                spdlog::info("DLSS output buffer reallocated ({}x{} display resolution)",
                             width, height);
            }
            dlss_output_width_ = width;
            dlss_output_height_ = height;
            display_res_changed = true;
        }

        // Ensure DLSS optimal settings are cached before resolving render
        // height — resolve_render_height reads the cached min/max/optimal
        // values.  Cache on first enable and on display resolution change.
        if (scene.settings.dlss_enabled && dlss_rr_.available()) {
            if (!dlss_rr_.feature_active() || display_res_changed) {
                if (!dlss_rr_.feature_active()) {
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
                }
                dlss_rr_.cache_optimal_settings(width, height);
            }
        }

        // Color buffer follows the render resolution, not the display
        // resolution. When DLSS is on, the render resolution is clamped by
        // resolve_render_height so that buffer, raygen launch, and NGX all
        // use the same dimensions. On mismatch both streams are drained
        // first — the previous frame's raygen/tonemap may still be reading or
        // writing the old allocation. sample_count resets to 0: tonemap
        // outputs black until valid data arrives.
        uint32_t render_height = scene.settings.render_height;
        if (scene.settings.dlss_enabled && dlss_rr_.available()) {
            render_height = dlss_rr_.resolve_render_height(render_height, height).render_height;
        }
        const uint32_t render_width = compute_render_width(render_height, width, height);
        bool render_res_changed = false;
        if (render_width != render_width_ || render_height != render_height_) {
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
            frame_slot_.resize(render_width, render_height);
            invalidate_dlss_state();
            render_width_ = render_width;
            render_height_ = render_height;
            render_res_changed = true;
            spdlog::info("Render buffer reallocated ({}x{} render resolution)",
                         render_width, render_height);
        }

        // Recreate DLSS-RR feature when resolution changed, preset changed,
        // or create it for the first time when the user enables DLSS. Only
        // when DLSS is enabled — resolution changes while DLSS is off must
        // not create a feature. Optimal settings are already cached above.
        if (scene.settings.dlss_enabled && dlss_rr_.available()) {
            // Allocate aux guides and DLSS output on first enable.
            if (!frame_slot_.aux.valid()) {
                frame_slot_.aux.alloc(render_width, render_height);
            }
            if (!dlss_output_.valid()) {
                dlss_output_.alloc(width, height,
                                   cudaCreateChannelDescHalf4(),
                                   cudaReadModeElementType);
                dlss_output_width_ = width;
                dlss_output_height_ = height;
                dlss_output_valid_ = false;
            }

            const bool preset_changed = scene.settings.dlss_preset != prev_dlss_preset_;
            const bool needs_recreate = render_res_changed || display_res_changed
                                        || preset_changed
                                        || !dlss_rr_.feature_active();
            if (needs_recreate) {
                if (!dlss_rr_.feature_active() || preset_changed) {
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
                }
                dlss_rr_.create_feature(render_width, render_height, width, height,
                                        scene.settings.dlss_preset,
                                        cuda_context.display_stream);
                invalidate_dlss_state();
            }
        }
        // Release feature and free DLSS resources when user disables DLSS.
        if (!scene.settings.dlss_enabled
            && (dlss_rr_.feature_active() || frame_slot_.aux.valid())) {
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
            if (dlss_rr_.feature_active()) {
                dlss_rr_.release_feature();
            }
            frame_slot_.aux.free();
            dlss_output_.free();
            dlss_output_width_ = 0;
            dlss_output_height_ = 0;
            invalidate_dlss_state();
        }

        // --- Serial pipeline: compute_stream (raygen) → display_stream (DLSS/tonemap) ---
#ifndef NDEBUG
        const uint32_t timing_slot = frame_counter_ % 2;
#endif
        const uint32_t effective_spp = scene.settings.accumulation_enabled
                                           ? scene.settings.samples_per_frame
                                           : 0;
        const bool has_new_samples = effective_spp > 0;

        // compute_stream: wait until previous display pass finished reading
        // the color buffer before raygen overwrites it.
        if (has_new_samples) {
            CUDA_CHECK(cudaStreamWaitEvent(
                cuda_context.compute_stream, frame_slot_.consumption_event));
        }

        const float exposure_linear = std::pow(2.0f, scene.settings.exposure_ev);
        const bool dlss_active = scene.settings.dlss_enabled && dlss_rr_.feature_active();

        // DLSS history management: camera or content changes during pause
        // invalidate temporal history so the first resumed frame starts fresh.
        const CameraKey camera_key{
            .inv_view = scene.camera.inv_view,
            .inv_projection = scene.camera.inv_projection,
        };
        const bool camera_changed = camera_key != prev_camera_;
        const bool max_clamp_changed =
            scene.settings.max_clamp != prev_max_clamp_;
        const bool content_changed =
            scene.settings.env_rotation != prev_env_rotation_ ||
            scene.settings.dlss_enabled != prev_dlss_enabled_ ||
            max_clamp_changed;
        if (has_new_samples) {
            reset_requested_ = false;
            if (dlss_active && max_clamp_changed) {
                invalidate_dlss_history();
            }
        } else if (camera_changed || content_changed) {
            reset_requested_ = true;
            if (dlss_active && (camera_changed || max_clamp_changed)) {
                invalidate_dlss_history();
            }
        }

        const bool produces_dlss_input = dlss_active && has_new_samples;
        const bool slot_reset = produces_dlss_input && dlss_reset_requested_;
        if (produces_dlss_input) {
            dlss_reset_requested_ = false;
        }

        prev_camera_ = camera_key;
        prev_env_rotation_ = scene.settings.env_rotation;
        prev_dlss_enabled_ = scene.settings.dlss_enabled;
        prev_dlss_preset_ = scene.settings.dlss_preset;
        prev_max_clamp_ = scene.settings.max_clamp;

        // Global per-frame jitter for DLSS mode (no per-pixel CP rotation).
        const float jitter_x = global_jitter(kSobolDirectionData, frame_counter_, 0);
        const float jitter_y = global_jitter(kSobolDirectionData, frame_counter_, 1);

        // Unjittered VP for motion vector computation (row-major for device mul()).
        const float4x4 current_vp = to_float4x4(scene.camera.projection * scene.camera.view);
        const bool has_temporal_predecessor = dlss_active
                                              && prev_dlss_metadata_.valid
                                              && !slot_reset;
        const float4x4 previous_vp = has_temporal_predecessor
                                              ? to_float4x4(
                                                    prev_dlss_metadata_.projection_matrix
                                                    * prev_dlss_metadata_.view_matrix)
                                              : current_vp;

        // Build current-frame DLSS metadata (used for evaluate + saved as prev).
        const DlssFrameMetadata current_dlss_metadata{
            .jitter_x = jitter_x,
            .jitter_y = jitter_y,
            .view_matrix = scene.camera.view,
            .projection_matrix = scene.camera.projection,
            .frame_time_ms = scene.frame_time_ms,
            .reset = slot_reset,
            .valid = produces_dlss_input,
        };

        LaunchParams params{
            .color_output = frame_slot_.color.surf_object(),
            .width = render_width,
            .height = render_height,
            .sequence_base = sequence_base_,
            .traversable = accel_.tlas_handle(),
            .geometry_infos = geometry_info_buffer_.data(),
            .materials = scene.materials.data(),
            .texture_objects = scene.texture_objects.data(),
            .inv_view = to_float4x4(scene.camera.inv_view),
            .inv_projection = to_float4x4(scene.camera.inv_projection),
            .max_bounces = scene.settings.max_bounces,
            .samples_per_frame = effective_spp,
            .ser_hint_bits = [&] {
                const uint32_t n = scene.materials.count();
                if (n <= 1) { return 1u; }
                const auto bits = static_cast<uint32_t>(std::bit_width(n - 1));
                return std::clamp(bits, 1u, 16u);
            }(),
            .dlss_enabled = dlss_active ? 1u : 0u,
            .jitter_x = jitter_x,
            .jitter_y = jitter_y,
            .max_clamp = scene.settings.max_clamp,
            .env = [&] {
                auto e = scene.env;
                e.rotation_sin = std::sin(scene.settings.env_rotation);
                e.rotation_cos = std::cos(scene.settings.env_rotation);
                return e;
            }(),
            .emissive = scene.emissive,
            .aux_depth = frame_slot_.aux.depth.surf_object(),
            .aux_motion_vectors = frame_slot_.aux.motion_vectors.surf_object(),
            .aux_diffuse_albedo = frame_slot_.aux.diffuse_albedo.surf_object(),
            .aux_specular_albedo = frame_slot_.aux.specular_albedo.surf_object(),
            .aux_normal_roughness = frame_slot_.aux.normal_roughness.surf_object(),
            .view_projection = current_vp,
            .prev_view_projection = previous_vp,
            // sobol_directions filled below via memcpy.
        };
        std::memcpy(params.sobol_directions, kSobolDirectionData, sizeof(params.sobol_directions));

        if (has_new_samples) {
#ifndef NDEBUG
            if (frame_counter_ >= 2) {
                CUDA_CHECK(cudaEventSynchronize(event_pt_end_[timing_slot]));
                cudaEventElapsedTime(
                    &pt_ms_, event_pt_start_[timing_slot], event_pt_end_[timing_slot]);
            }
            CUDA_CHECK(cudaEventRecord(
                event_pt_start_[timing_slot], cuda_context.compute_stream));
#endif

            auto *staging = params_staging_[frame_index % params_staging_.size()];
            std::memcpy(staging, &params, sizeof(LaunchParams));
            params_buffer_.upload(staging, 1, cuda_context.compute_stream);

            const OptixShaderBindingTable sbt{
                .raygenRecord = sbt_raygen_.device_ptr(),
                .exceptionRecord = 0,
                .missRecordBase = sbt_miss_.device_ptr(),
                .missRecordStrideInBytes = sizeof(SbtRecord),
                .missRecordCount = 1,
                .hitgroupRecordBase = sbt_hit_.device_ptr(),
                .hitgroupRecordStrideInBytes = sizeof(SbtRecord),
                .hitgroupRecordCount = 1,
                .callablesRecordBase = 0,
                .callablesRecordStrideInBytes = 0,
                .callablesRecordCount = 0,
            };

            OPTIX_CHECK(optixLaunch(pipeline_.handle,
                cuda_context.compute_stream,
                params_buffer_.device_ptr(),
                sizeof(LaunchParams),
                &sbt,
                render_width, render_height, 1));

#ifndef NDEBUG
            CUDA_CHECK(cudaEventRecord(
                event_pt_end_[timing_slot], cuda_context.compute_stream));
#endif

            CUDA_CHECK(cudaEventRecord(
                frame_slot_.production_event, cuda_context.compute_stream));
        }

        // --- display_stream: wait production + reverse sem → DLSS/tonemap → signal ---
        // Reverse semaphore: wait for the previous frame's blit to finish
        // reading display_surface before tonemap overwrites it.
        // ReSharper disable once CppLocalVariableMayBeConst
        cudaExternalSemaphore_t reverse_sem = cuda_context.reverse_external_semaphore;
        constexpr cudaExternalSemaphoreWaitParams reverse_wait_params{};
        CUDA_CHECK(cudaWaitExternalSemaphoresAsync(&reverse_sem, &reverse_wait_params, 1, cuda_context.display_stream));

        // Wait until raygen has produced color (and guides).
        CUDA_CHECK(cudaStreamWaitEvent(
            cuda_context.display_stream, frame_slot_.production_event));

#ifndef NDEBUG
        if (frame_counter_ >= 2
            && cudaEventQuery(event_display_end_[timing_slot]) == cudaSuccess) {
            cudaEventElapsedTime(&cuda_display_ms_,
                                 event_display_start_[timing_slot],
                                 event_display_end_[timing_slot]);
        }
        CUDA_CHECK(cudaEventRecord(
            event_display_start_[timing_slot], cuda_context.display_stream));
#endif

        // DLSS evaluate reads the current frame's color (serial: raygen is
        // done) rather than the previous frame's.
        const bool evaluate_dlss = dlss_active && has_new_samples;
        if (evaluate_dlss) {
            const optix::DlssRR::EvalInput eval_input{
                .color_tex = frame_slot_.color.tex_object(),
                .output_surf = dlss_output_.surf_object(),
                .depth_tex = frame_slot_.aux.depth.tex_object(),
                .motion_vectors_tex = frame_slot_.aux.motion_vectors.tex_object(),
                .diffuse_albedo_tex = frame_slot_.aux.diffuse_albedo.tex_object(),
                .specular_albedo_tex = frame_slot_.aux.specular_albedo.tex_object(),
                .normal_roughness_tex = frame_slot_.aux.normal_roughness.tex_object(),
                .render_width = render_width,
                .render_height = render_height,
                .jitter_x = current_dlss_metadata.jitter_x,
                .jitter_y = current_dlss_metadata.jitter_y,
                .view_matrix = glm::value_ptr(current_dlss_metadata.view_matrix),
                .projection_matrix = glm::value_ptr(current_dlss_metadata.projection_matrix),
                .reset = current_dlss_metadata.reset,
                .frame_time_ms = current_dlss_metadata.frame_time_ms,
            };
            dlss_rr_.evaluate(eval_input);
            dlss_output_valid_ = true;

            launch_tonemap(dlss_output_.tex_object(),
                           cuda_context.display_surface,
                           width, height,
                           width, height,
                           1,
                           exposure_linear,
                           cuda_context.display_stream);
        } else if (dlss_active && dlss_output_valid_) {
            // No new input but a valid cached DLSS output exists: reuse it.
            launch_tonemap(dlss_output_.tex_object(),
                           cuda_context.display_surface,
                           width, height,
                           width, height,
                           1,
                           exposure_linear,
                           cuda_context.display_stream);
        } else {
            // DLSS OFF: tonemap the color buffer directly. Raygen writes
            // per-frame mean; pass 1 so tonemap does not re-divide. Zero
            // count (no raygen yet) produces black.
            launch_tonemap(frame_slot_.color.tex_object(),
                           cuda_context.display_surface,
                           render_width, render_height,
                           width, height,
                           frame_slot_.sample_count > 0 ? 1u : 0u,
                           exposure_linear,
                           cuda_context.display_stream);
        }

#ifndef NDEBUG
        CUDA_CHECK(cudaEventRecord(
            event_display_end_[timing_slot], cuda_context.display_stream));
#endif

        // Record consumption so next frame's raygen waits for this tonemap.
        CUDA_CHECK(cudaEventRecord(
            frame_slot_.consumption_event, cuda_context.display_stream));

        // Signal forward semaphore after tonemap completes.
        // ReSharper disable once CppLocalVariableMayBeConst
        cudaExternalSemaphore_t sem = cuda_context.external_semaphores[frame_index];
        constexpr cudaExternalSemaphoreSignalParams signal_params{};
        CUDA_CHECK(cudaSignalExternalSemaphoresAsync(&sem, &signal_params, 1, cuda_context.display_stream));

        if (has_new_samples) {
            frame_slot_.sample_count = 1;
            sequence_base_ += effective_spp;
            // Save current metadata as prev for next frame's motion vectors.
            prev_dlss_metadata_ = current_dlss_metadata;
        }
        ++frame_counter_;
    }

    void Renderer::record_vulkan(const RenderInput &input) {
        // ReSharper disable once CppLocalVariableMayBeConst
        VkCommandBuffer cmd = input.cmd;
        // ReSharper disable once CppLocalVariableMayBeConst
        VkImage swapchain_image = input.swapchain.images[input.image_index];
        const VkExtent2D extent = input.swapchain.extent;

#ifndef NDEBUG
        if (input.timestamp_pool != VK_NULL_HANDLE) {
            vkCmdResetQueryPool(cmd, input.timestamp_pool,
                                input.timestamp_query_base, 2);
            vkCmdWriteTimestamp2(cmd, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                input.timestamp_pool,
                                input.timestamp_query_base);
        }
#endif

        // --- Layout transitions before the blit ---
        // Display buffer acquire: CUDA just wrote it via a surface object (external
        // access). The queue family ownership transfer from EXTERNAL makes the CUDA
        // writes visible to Vulkan. GENERAL is the layout compatible with external
        // access; the transition to TRANSFER_SRC_OPTIMAL prepares for the blit read.
        // The external semaphore wait in end_frame provides the execution dependency.
        const VkImageMemoryBarrier2 display_acquire{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL,
            .dstQueueFamilyIndex = input.graphics_queue_family,
            .image = input.display_buffer.image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        // Swapchain: UNDEFINED -> TRANSFER_DST_OPTIMAL. No prior GPU work touched the
        // acquired image (acquire hands it over via the wait semaphore in end_frame),
        // so the src stage is TOP_OF_PIPE.
        const VkImageMemoryBarrier2 swapchain_to_dst{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchain_image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        const VkImageMemoryBarrier2 pre_blit_barriers[2]{display_acquire, swapchain_to_dst};
        // ReSharper disable once CppVariableCanBeMadeConstexpr
        const VkDependencyInfo pre_blit_dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 2,
            .pImageMemoryBarriers = pre_blit_barriers,
        };
        vkCmdPipelineBarrier2(cmd, &pre_blit_dep);

        // --- Blit: display buffer (R16G16B16A16_SFLOAT) -> swapchain (B8G8R8A8_SRGB) ---
        // Hardware handles float->UNORM, RGBA->BGRA channel swap, and sRGB encoding.
        const VkImageBlit2 region{
            .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .srcSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .srcOffsets = {{0, 0, 0}, {static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), 1}},
            .dstSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .dstOffsets = {{0, 0, 0}, {static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), 1}},
        };

        const VkBlitImageInfo2 blit_info{
            .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .srcImage = input.display_buffer.image,
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage = swapchain_image,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = 1,
            .pRegions = &region,
            .filter = VK_FILTER_NEAREST,
        };
        vkCmdBlitImage2(cmd, &blit_info);

        // --- Post-blit barriers (display buffer release + swapchain transition) ---
        // Display buffer release: hand ownership back to EXTERNAL so the next
        // frame's CUDA tonemap can write it. Transitions back to GENERAL (the
        // layout compatible with external access). The reverse semaphore signal
        // in end_frame provides the execution dependency toward CUDA.
        const VkImageMemoryBarrier2 display_release{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .srcQueueFamilyIndex = input.graphics_queue_family,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL,
            .image = input.display_buffer.image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        // Swapchain: TRANSFER_DST_OPTIMAL -> COLOR_ATTACHMENT_OPTIMAL for ImGui.
        const VkImageMemoryBarrier2 to_attachment{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchain_image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        const VkImageMemoryBarrier2 post_blit_barriers[2]{display_release, to_attachment};
        // ReSharper disable once CppVariableCanBeMadeConstexpr
        const VkDependencyInfo post_blit_dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 2,
            .pImageMemoryBarriers = post_blit_barriers,
        };
        vkCmdPipelineBarrier2(cmd, &post_blit_dep);

        // --- ImGui overlay on top of the blitted image (loadOp=LOAD keeps it) ---
        const VkRenderingAttachmentInfo color_attachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = input.swapchain.image_views[input.image_index],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };

        const VkRenderingInfo rendering_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea =
            {
                .offset = {0, 0},
                .extent = extent,
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment,
        };

        vkCmdBeginRendering(cmd, &rendering_info);
        input.imgui.render(cmd);
        vkCmdEndRendering(cmd);

        // --- Swapchain: COLOR_ATTACHMENT_OPTIMAL -> PRESENT_SRC_KHR ---
        const VkImageMemoryBarrier2 to_present{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchain_image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        const VkDependencyInfo to_present_dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &to_present,
        };
        vkCmdPipelineBarrier2(cmd, &to_present_dep);

#ifndef NDEBUG
        if (input.timestamp_pool != VK_NULL_HANDLE) {
            vkCmdWriteTimestamp2(cmd, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
                                input.timestamp_pool,
                                input.timestamp_query_base + 1);
        }
#endif
    }

    bool Renderer::has_valid_frame() const {
        return frame_slot_.sample_count > 0;
    }

    uint32_t Renderer::tlas_instance_count() const {
        return tlas_instance_count_;
    }

    void Renderer::reset_accumulation() {
        reset_requested_ = true;
        invalidate_dlss_state();
    }
} // namespace qualquer::renderer
