/**
 * @file renderer.cpp
 * @brief Renderer implementation.
 */

#include <qualquer/renderer/renderer.h>

#include <algorithm>
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
            for (uint32_t bit = 0; bit < 32u && sample_index != 0u; ++bit) {
                if ((sample_index & 1u) != 0u) {
                    result ^= directions[offset + bit];
                }
                sample_index >>= 1u;
            }
            return result;
        }

        /**
         * @brief Computes global per-frame jitter for DLSS-RR mode.
         *
         * Uses Sobol dim 0/1 with golden-ratio temporal offset but NO
         * per-pixel Cranley-Patterson rotation. Returns [0,1) in pixel space.
         */
        float global_jitter(const uint32_t *directions,
                            const uint32_t frame_index,
                            const uint32_t dimension) {
            uint32_t sobol_val = sobol_sample_host(directions, dimension, frame_index);
            // Golden-ratio temporal offset (same as device rng, 2654435769 ≈ φ × 2^32)
            sobol_val += frame_index * 2654435769u;
            return static_cast<float>(sobol_val) / static_cast<float>(0xFFFFFFFFu);
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
            for (uint32_t g = 0; g < grouping.group_geometries.size(); ++g) {
                if (grouping.group_geometries[g].empty()) {
                    continue;
                }
                grouping.group_to_blas[g] = static_cast<uint32_t>(accel.blas_handles().size());
                accel.build_blas(cuda_context.device_context,
                                 cuda_context.compute_stream,
                                 grouping.group_geometries[g]);
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
                inst.flags = OPTIX_INSTANCE_FLAG_NONE;
                inst.traversableHandle = blas_handles[grouping.group_to_blas[group_id]].handle;
                tlas_instances.push_back(inst);

                inst_idx += prim_count;
            }
            return tlas_instances;
        }
    } // namespace

    void Renderer::init(const optix::Context &cuda_context,
                        const uint32_t width,
                        const uint32_t height,
                        const std::string &optixir_path) {
        // The launch-params constant name ("params") and its size are device-side
        // facts this layer cannot take from the renderer header (single-direction
        // dependency).
        pipeline_.init(cuda_context.device_context,
                       optixir_path,
                       sizeof(LaunchParams),
                       "params");

        // SBT records are header-only (global data via LaunchParams).
        // Miss SBT has 2 entries: env (missIndex=0) and shadow (missIndex=1).
        SbtRecord record{};
        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.raygen_program, &record));
        sbt_raygen_.alloc(1);
        sbt_raygen_.upload(&record, 1, cuda_context.compute_stream);

        std::array<SbtRecord, 2> miss_records{};
        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.miss_env_program, &miss_records[0]));
        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.miss_shadow_program, &miss_records[1]));
        sbt_miss_.alloc(2);
        sbt_miss_.upload(miss_records.data(), 2, cuda_context.compute_stream);

        OPTIX_CHECK(optixSbtRecordPackHeader(pipeline_.hitgroup_program, &record));
        sbt_hit_.alloc(1);
        sbt_hit_.upload(&record, 1, cuda_context.compute_stream);

        // Two ping-pong color buffers as CUDA arrays (CudaArrayBuffer) for
        // DLSS-RR texture-object consumption and surf2Dwrite by raygen. No
        // explicit clear: accum_counts_ = {0,0} makes raygen overwrite on the
        // first frame and tonemap output black (count == 0 guard), so
        // uninitialised content is never read.
        for (auto &buffer: accum_buffers_) {
            buffer.alloc(width, height);
        }

        // Aux G-buffer channels for DLSS-RR. Allocated at render resolution;
        // reallocated alongside accum_buffers_ when render resolution changes.
        aux_depth_.alloc(width, height);
        aux_motion_vectors_.alloc(width, height);
        aux_diffuse_albedo_.alloc(width, height);
        aux_specular_albedo_.alloc(width, height);
        aux_normals_.alloc(width, height);
        aux_roughness_.alloc(width, height);

        // DLSS-RR output at display resolution. Initially display == render
        // (before any user-driven render-height change); resized in submit_cuda
        // when the display dimensions change.
        dlss_output_.alloc(width, height);
        dlss_output_width_ = width;
        dlss_output_height_ = height;

        render_width_ = width;
        render_height_ = height;

        params_buffer_.alloc(1);
        accum_index_ = 0;
        accum_counts_ = {0, 0};

        // Events start recorded on compute_stream so the first frame's waits
        // (on slot 1, the "previous" slot) pass immediately — the stream ordering
        // guarantees the records complete after the buffer clears above.
        for (auto &event: event_raygen_done_) {
            CUDA_CHECK(cudaEventCreateWithFlags(&event, cudaEventDisableTiming));
            CUDA_CHECK(cudaEventRecord(event, cuda_context.compute_stream));
        }
        for (auto &event: event_tonemap_done_) {
            CUDA_CHECK(cudaEventCreateWithFlags(&event, cudaEventDisableTiming));
            CUDA_CHECK(cudaEventRecord(event, cuda_context.compute_stream));
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
        // (accum_index_, frame_counter_, sample_count_) are intentionally not
        // reset here — release is the sole responsibility; a subsequent init
        // resets them.
        pipeline_.destroy();
        sbt_raygen_.free();
        sbt_miss_.free();
        sbt_hit_.free();
        accel_.destroy();
        geometry_info_buffer_.free();
        for (auto &buffer: accum_buffers_) {
            buffer.free();
        }
        aux_depth_.free();
        aux_motion_vectors_.free();
        aux_diffuse_albedo_.free();
        aux_specular_albedo_.free();
        aux_normals_.free();
        aux_roughness_.free();
        dlss_output_.free();
        params_buffer_.free();
        for (auto &event: event_raygen_done_) {
            if (event != nullptr) {
                CUDA_CHECK(cudaEventDestroy(event));
                event = nullptr;
            }
        }
        for (auto &event: event_tonemap_done_) {
            if (event != nullptr) {
                CUDA_CHECK(cudaEventDestroy(event));
                event = nullptr;
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
    }

    void Renderer::load_scene(const optix::Context &cuda_context,
                              const std::span<const Mesh> meshes,
                              const std::span<const MeshInstance> instances) {
        // Runtime scene switching: tear down the previous scene's AS and
        // geometry-info buffer before rebuilding.
        accel_.destroy();
        geometry_info_buffer_.free();

        if (meshes.empty()) {
            // No geometry to trace; submit_cuda must keep the traversable at 0 so
            // raygen skips optixTrace.
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
                               optix::DlssRR &dlss_rr,
                               const SceneRenderInput &scene,
                               const uint32_t width,
                               const uint32_t height,
                               const uint32_t frame_index) {
        // DLSS output buffer tracks display resolution (window resize), not
        // render resolution. Drain both streams before reallocating — the
        // previous frame's tonemap may still be reading the old buffer.
        bool display_res_changed = false;
        if (width != dlss_output_width_ || height != dlss_output_height_) {
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
            dlss_output_.resize(width, height);
            dlss_output_width_ = width;
            dlss_output_height_ = height;
            display_res_changed = true;
            spdlog::info("DLSS output buffer reallocated ({}x{} display resolution)",
                         width, height);
        }

        // Ensure DLSS optimal settings are cached before resolving render
        // height — resolve_render_height reads the cached min/max/optimal
        // values.  Cache on first enable and on display resolution change.
        if (scene.settings.dlss_enabled && dlss_rr.available()) {
            if (!dlss_rr.feature_active() || display_res_changed) {
                if (!dlss_rr.feature_active()) {
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
                }
                dlss_rr.cache_optimal_settings(width, height);
            }
        }

        // Accumulation buffers follow the render resolution, not the display
        // resolution. When DLSS is on, the render resolution is clamped by
        // resolve_render_height so that buffers, raygen launch, and NGX all
        // use the same dimensions.  On mismatch both streams are drained
        // first — the previous frame's raygen/tonemap may still be reading or
        // writing the old allocations. Counts reset to 0: raygen enters
        // overwrite mode (never reads the fresh buffers) and tonemap outputs
        // black until valid data arrives, so the uninitialised contents are
        // never consumed.
        uint32_t render_height = scene.settings.render_height;
        if (scene.settings.dlss_enabled && dlss_rr.available()) {
            render_height = dlss_rr.resolve_render_height(render_height, height).render_height;
        }
        const uint32_t render_width = compute_render_width(render_height, width, height);
        bool render_res_changed = false;
        if (render_width != render_width_ || render_height != render_height_) {
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
            for (auto &buffer: accum_buffers_) {
                buffer.resize(render_width, render_height);
            }
            aux_depth_.resize(render_width, render_height);
            aux_motion_vectors_.resize(render_width, render_height);
            aux_diffuse_albedo_.resize(render_width, render_height);
            aux_specular_albedo_.resize(render_width, render_height);
            aux_normals_.resize(render_width, render_height);
            aux_roughness_.resize(render_width, render_height);
            accum_counts_ = {0, 0};
            render_width_ = render_width;
            render_height_ = render_height;
            render_res_changed = true;
            spdlog::info("Render buffers reallocated ({}x{} render resolution)",
                         render_width, render_height);
        }

        // Recreate DLSS-RR feature when resolution changed, or create it for
        // the first time when the user enables DLSS. Only when DLSS is
        // enabled — resolution changes while DLSS is off must not create a
        // feature.  Optimal settings are already cached above.
        if (scene.settings.dlss_enabled && dlss_rr.available()) {
            const bool needs_recreate = render_res_changed || display_res_changed
                                        || !dlss_rr.feature_active();
            if (needs_recreate) {
                if (!dlss_rr.feature_active()) {
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
                    CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
                }
                dlss_rr.create_feature(render_width, render_height, width, height,
                                       scene.dlss_preset, cuda_context.display_stream);
            }
        }
        // Release feature when user disables DLSS (free VRAM immediately).
        if (!scene.settings.dlss_enabled && dlss_rr.feature_active()) {
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.compute_stream));
            CUDA_CHECK(cudaStreamSynchronize(cuda_context.display_stream));
            dlss_rr.release_feature();
        }

        // Dual-stream overlap: compute_stream runs raygen while display_stream
        // runs tonemap + semaphore signal in parallel. Ping-pong buffers guarantee
        // raygen and tonemap access different buffers within the same frame; CUDA
        // events enforce the cross-frame dependencies (each buffer must finish
        // being written before tonemap reads it, and finish being read before
        // raygen overwrites it).
        const uint32_t slot = frame_counter_ % 2;
        const uint32_t prev_slot = 1 - slot;

        // --- compute_stream: wait prev tonemap → upload + raygen → record ---
        // Wait until the previous frame's tonemap finished reading the buffer
        // that this frame's raygen is about to overwrite.
        CUDA_CHECK(cudaStreamWaitEvent(cuda_context.compute_stream, event_tonemap_done_[prev_slot]));

        // Accumulation reset: any change in camera matrices or render settings
        // breaks the chain — chain_count drops to 0 so raygen overwrites instead
        // of accumulating. No buffer clearing: the read slot keeps its valid
        // count for tonemap; the write slot's count is set at frame end.
        //
        // Accumulation pause (accumulation_enabled=false): effective_spp=0 so
        // raygen's sample loop doesn't execute — it reads the old total and
        // writes it back unchanged, count stays the same, display freezes.
        const float exposure_linear = std::pow(2.0f, scene.settings.exposure_ev);

        // Runtime DLSS flag: user wants it AND feature is actually created.
        const bool dlss_active = scene.settings.dlss_enabled && dlss_rr.feature_active();

        const bool camera_changed =
            scene.camera.inv_view != prev_inv_view_ ||
            scene.camera.inv_projection != prev_inv_projection_;
        // render_height is intentionally absent: a render-resolution change
        // triggers buffer reallocation above, which resets accum_counts_ to
        // {0,0} — chain_count becomes 0 through that path, not through
        // needs_reset.
        const bool settings_changed =
            scene.settings.max_bounces != prev_max_bounces_ ||
            scene.settings.samples_per_frame != prev_samples_per_frame_ ||
            scene.settings.env_rotation != prev_env_rotation_ ||
            scene.settings.dlss_enabled != prev_dlss_enabled_;
        const bool needs_reset = camera_changed || settings_changed || reset_requested_;
        const bool dlss_reset = dlss_reset_requested_;
        reset_requested_ = false;
        dlss_reset_requested_ = false;
        // DLSS ON: always overwrite (single-frame output, no accumulation).
        // DLSS OFF: normal Separate Sum chain.
        const uint32_t chain_count = (needs_reset || dlss_active)
                                         ? 0
                                         : accum_counts_[accum_index_];
        const uint32_t effective_spp = scene.settings.accumulation_enabled
                                           ? scene.settings.samples_per_frame
                                           : 0;

        prev_inv_view_ = scene.camera.inv_view;
        prev_inv_projection_ = scene.camera.inv_projection;
        prev_max_bounces_ = scene.settings.max_bounces;
        prev_samples_per_frame_ = scene.settings.samples_per_frame;
        prev_env_rotation_ = scene.settings.env_rotation;
        prev_dlss_enabled_ = scene.settings.dlss_enabled;

        // Global per-frame jitter for DLSS mode (no per-pixel CP rotation).
        const float jitter_x = global_jitter(kSobolDirectionData, frame_counter_, 0);
        const float jitter_y = global_jitter(kSobolDirectionData, frame_counter_, 1);

        // Unjittered VP for motion vector computation (row-major for device mul()).
        const float4x4 current_vp = to_float4x4(scene.camera.projection * scene.camera.view);

        LaunchParams params{
            // Ping-pong color buffers as CUDA arrays: raygen writes via
            // surf2Dwrite to the write slot, reads (DLSS OFF only) via tex2D
            // from the read slot.
            .color_output = accum_buffers_[1 - accum_index_].surf_object(),
            .color_input = accum_buffers_[accum_index_].tex_object(),
            .width = render_width,
            .height = render_height,
            .frame_index = frame_counter_,
            .traversable = accel_.tlas_handle(),
            .geometry_infos = geometry_info_buffer_.data(),
            .materials = scene.materials.data(),
            .texture_objects = scene.texture_objects.data(),
            .inv_view = to_float4x4(scene.camera.inv_view),
            .inv_projection = to_float4x4(scene.camera.inv_projection),
            .max_bounces = scene.settings.max_bounces,
            .samples_per_frame = effective_spp,
            .sample_count = chain_count,
            .dlss_enabled = dlss_active ? 1u : 0u,
            .jitter_x = jitter_x,
            .jitter_y = jitter_y,
            // The rotation angle is a launch constant: precompute the sin/cos
            // pair so device code avoids a per-hit sincosf.
            .env_rotation_sin = std::sin(scene.settings.env_rotation),
            .env_rotation_cos = std::cos(scene.settings.env_rotation),
            // Env light resources (from SceneLoader via SceneRenderInput).
            .env_cubemap = scene.env_cubemap,
            .env_alias_table = scene.env_alias_table,
            .env_alias_count = scene.env_alias_count,
            .env_alias_width = scene.env_alias_width,
            .env_alias_height = scene.env_alias_height,
            .env_total_luminance = scene.env_total_luminance,
            .emissive_triangles = scene.emissive_triangles,
            .emissive_alias_table = scene.emissive_alias_table,
            .emissive_count = scene.emissive_count,
            .emissive_total_power = scene.emissive_total_power,
            // Aux G-buffer surfaces for closesthit/raygen writes.
            .aux_depth = aux_depth_.surf_object(),
            .aux_motion_vectors = aux_motion_vectors_.surf_object(),
            .aux_diffuse_albedo = aux_diffuse_albedo_.surf_object(),
            .aux_specular_albedo = aux_specular_albedo_.surf_object(),
            .aux_normals = aux_normals_.surf_object(),
            .aux_roughness = aux_roughness_.surf_object(),
            // Unjittered VP matrices for motion vector computation.
            .view_projection = current_vp,
            .prev_view_projection = prev_view_projection_,
            // sobol_directions filled below via memcpy (array can't be
            // initialized from another array in a designated initializer).
        };
        std::memcpy(params.sobol_directions, kSobolDirectionData, sizeof(params.sobol_directions));

#ifndef NDEBUG
        // Read frame N-2's PT timing. Synchronize the end event to ensure the
        // compute_stream work is complete (fence only guarantees display_stream).
        // Nearly zero wait in practice — frame N-2's raygen is consumed by
        // frame N-1's display_stream before the fence.
        if (frame_counter_ >= 2) {
            CUDA_CHECK(cudaEventSynchronize(event_pt_end_[slot]));
            cudaEventElapsedTime(&pt_ms_, event_pt_start_[slot], event_pt_end_[slot]);
        }
        CUDA_CHECK(cudaEventRecord(event_pt_start_[slot], cuda_context.compute_stream));
#endif

        params_buffer_.upload(&params, 1, cuda_context.compute_stream);

        const OptixShaderBindingTable sbt{
            .raygenRecord = sbt_raygen_.device_ptr(),
            .exceptionRecord = 0,
            .missRecordBase = sbt_miss_.device_ptr(),
            .missRecordStrideInBytes = sizeof(SbtRecord),
            .missRecordCount = 2,
            .hitgroupRecordBase = sbt_hit_.device_ptr(),
            .hitgroupRecordStrideInBytes = sizeof(SbtRecord),
            .hitgroupRecordCount = 1,
            .callablesRecordBase = 0,
            .callablesRecordStrideInBytes = 0,
            .callablesRecordCount = 0,
        };

        // traversable=0 is valid: raygen does not call optixTrace, so no
        // acceleration structure is traversed.
        OPTIX_CHECK(optixLaunch(pipeline_.handle,
            cuda_context.compute_stream,
            params_buffer_.device_ptr(),
            sizeof(LaunchParams),
            &sbt,
            render_width, render_height, 1));

#ifndef NDEBUG
        CUDA_CHECK(cudaEventRecord(event_pt_end_[slot], cuda_context.compute_stream));
#endif

        CUDA_CHECK(cudaEventRecord(event_raygen_done_[slot], cuda_context.compute_stream));

        // --- display_stream: wait reverse sem + wait prev raygen → tonemap → record → signal ---
        // Wait for the previous frame's blit to finish reading display_surface
        // before this frame's tonemap overwrites it (write-after-read). A single
        // semaphore suffices: the forward chain (tonemap → forward signal → blit →
        // reverse signal) structurally prevents double-signaling.
        // Enqueued before the raygen event wait: the reverse semaphore typically
        // signals earlier (blit is lighter than raygen), so checking it first lets
        // the GPU scheduler resolve the faster wait and prepare for the next one.
        // ReSharper disable once CppLocalVariableMayBeConst
        cudaExternalSemaphore_t reverse_sem = cuda_context.reverse_external_semaphore;
        constexpr cudaExternalSemaphoreWaitParams reverse_wait_params{};
        CUDA_CHECK(cudaWaitExternalSemaphoresAsync(&reverse_sem, &reverse_wait_params, 1, cuda_context.display_stream));

        // Wait until the previous frame's raygen finished writing the buffer
        // that this frame's tonemap is about to read.
        CUDA_CHECK(cudaStreamWaitEvent(cuda_context.display_stream, event_raygen_done_[prev_slot]));

#ifndef NDEBUG
        if (frame_counter_ >= 2
            && cudaEventQuery(event_display_end_[slot]) == cudaSuccess) {
            cudaEventElapsedTime(&cuda_display_ms_,
                                 event_display_start_[slot],
                                 event_display_end_[slot]);
        }
        CUDA_CHECK(cudaEventRecord(event_display_start_[slot], cuda_context.display_stream));
#endif

        if (dlss_active) {
            // DLSS ON: evaluate reads the previous frame's noisy buffer (read
            // slot) and writes the denoised+upscaled result to dlss_output.
            // Then tonemap reads dlss_output at display resolution (1:1, no
            // resampling, no Separate Sum division — sample_count=1).
            const optix::DlssRR::EvalInput eval_input{
                .color_tex = accum_buffers_[accum_index_].tex_object(),
                .output_surf = dlss_output_.surf_object(),
                .depth_tex = aux_depth_.tex_object(),
                .motion_vectors_tex = aux_motion_vectors_.tex_object(),
                .diffuse_albedo_tex = aux_diffuse_albedo_.tex_object(),
                .specular_albedo_tex = aux_specular_albedo_.tex_object(),
                .normals_tex = aux_normals_.tex_object(),
                .roughness_tex = aux_roughness_.tex_object(),
                .render_width = render_width,
                .render_height = render_height,
                .jitter_x = jitter_x,
                .jitter_y = jitter_y,
                .view_matrix = glm::value_ptr(scene.camera.view),
                .projection_matrix = glm::value_ptr(scene.camera.projection),
                .reset = dlss_reset,
                .frame_time_ms = scene.frame_time_ms,
            };
            dlss_rr.evaluate(eval_input);

            launch_tonemap(dlss_output_.tex_object(),
                           cuda_context.display_surface,
                           width, height,      // dlss_output is display resolution
                           width, height,      // 1:1, no resampling
                           1,                  // no Separate Sum division
                           exposure_linear,
                           cuda_context.display_stream);
        } else {
            // DLSS OFF: tonemap reads the ping-pong read slot directly,
            // resamples if render != display, divides by sample count.
            launch_tonemap(accum_buffers_[accum_index_].tex_object(),
                           cuda_context.display_surface,
                           render_width, render_height,
                           width, height,
                           accum_counts_[accum_index_],
                           exposure_linear,
                           cuda_context.display_stream);
        }

#ifndef NDEBUG
        CUDA_CHECK(cudaEventRecord(event_display_end_[slot], cuda_context.display_stream));
#endif

        CUDA_CHECK(cudaEventRecord(event_tonemap_done_[slot], cuda_context.display_stream));

        // Signal after tonemap completes on display_stream. Binary OPAQUE_WIN32
        // needs no fence value — signal_params stays zeroed.
        // ReSharper disable once CppLocalVariableMayBeConst
        cudaExternalSemaphore_t sem = cuda_context.external_semaphores[frame_index];
        constexpr cudaExternalSemaphoreSignalParams signal_params{};
        CUDA_CHECK(cudaSignalExternalSemaphoresAsync(&sem, &signal_params, 1, cuda_context.display_stream));

        // The write slot now holds chain_count + samples_per_frame samples.
        // Empty scene (traversable == 0): device wrote black, no valid samples
        // produced — keep the write-slot count at 0 so tonemap shows black and
        // the next frame enters overwrite mode when a scene is loaded.
        accum_counts_[1 - accum_index_] = accel_.tlas_handle() == 0
                                              ? 0
                                              : chain_count + params.samples_per_frame;
        accum_index_ = 1 - accum_index_;
        prev_view_projection_ = current_vp;
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

        // --- Blit: display buffer (R8G8B8A8_UNORM) -> swapchain (B8G8R8A8_SRGB) ---
        // Hardware handles UNORM->float, RGBA->BGRA channel swap, and sRGB encoding.
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
            .filter = VK_FILTER_LINEAR,
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

    uint32_t Renderer::accumulated_samples() const {
        return accum_counts_[accum_index_];
    }

    uint32_t Renderer::tlas_instance_count() const {
        return tlas_instance_count_;
    }

    void Renderer::reset_accumulation() {
        reset_requested_ = true;
        dlss_reset_requested_ = true;
    }
} // namespace qualquer::renderer
