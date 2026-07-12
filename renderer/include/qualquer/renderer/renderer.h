#pragma once

/**
 * @file renderer.h
 * @brief Renderer: single-frame render content (CUDA submit + Vulkan recording).
 */

#include <cuda_runtime.h>
#include <vulkan/vulkan.h>

#include <array>
#include <span>
#include <string>

#include <glm/glm.hpp>

#include <qualquer/optix/accel_structure.h>
#include <qualquer/optix/cuda_array_buffer.h>
#include <qualquer/optix/cuda_buffer.h>
#include <qualquer/optix/dlss_rr.h>
#include <qualquer/optix/pipeline.h>
#include <qualquer/renderer/camera.h>
#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/material.h>
#include <qualquer/renderer/render_settings.h>
#include <qualquer/renderer/scene_types.h>

namespace qualquer::optix {
    class Context;
}

namespace qualquer::vulkan {
    class InteropImage;
    class Swapchain;
    class ImGuiBackend;
}

namespace qualquer::renderer {

    /**
     * @brief OptiX SBT record carrying only the opaque program header.
     *
     * Used for raygen, miss, and hitgroup records, which all access scene data
     * through LaunchParams and carry no per-program user data.
     */
    struct SbtRecord {
        alignas(OPTIX_SBT_RECORD_ALIGNMENT) char header[OPTIX_SBT_RECORD_HEADER_SIZE];
    };

    /**
     * @brief Per-frame handles passed to Renderer::record_vulkan.
     *
     * Every member is owned elsewhere (Application or Context); this struct only
     * borrows references for the duration of one record_vulkan call, per the
     * ownership principle (no handle caching by users).
     */
    struct RenderInput {
        /** @brief Command buffer to record into (already begun by the caller). */
        VkCommandBuffer cmd = VK_NULL_HANDLE;

        /** @brief CUDA context: surface, stream, external semaphores. */
        optix::Context &cuda_context;

        /** @brief Display buffer (interop image) blitted to the swapchain image. */
        vulkan::InteropImage &display_buffer;

        /** @brief Swapchain providing the target image and its view. */
        vulkan::Swapchain &swapchain;

        /** @brief Index of the acquired swapchain image for this frame. */
        uint32_t image_index = 0;

        /** @brief Graphics queue family index for display buffer ownership transfers. */
        uint32_t graphics_queue_family = 0;

        /** @brief ImGui backend for overlay recording. */
        vulkan::ImGuiBackend &imgui;

#ifndef NDEBUG
        /** @brief Timestamp query pool for display pipeline timing (VK_NULL_HANDLE to skip). */
        VkQueryPool timestamp_pool = VK_NULL_HANDLE;

        /** @brief Base query index for this frame's timestamp pair (slot * 2). */
        uint32_t timestamp_query_base = 0;
#endif
    };

    /**
     * @brief Per-frame scene state passed to Renderer::submit_cuda.
     *
     * Every member is owned elsewhere (Application owns the Camera, SceneLoader
     * owns the material and texture-object buffers); this struct only borrows
     * references for the duration of one submit_cuda call, per the ownership
     * principle (no handle caching by users).
     */
    struct SceneRenderInput {
        /** @brief Camera providing inverse view/projection for primary-ray generation. */
        const Camera &camera;

        /** @brief Runtime render settings (bounces, spp, exposure, accumulation toggle). */
        const RenderSettings &settings;

        /** @brief Device material array (indexed via GPUGeometryInfo::material_buffer_offset). */
        const optix::CudaBuffer<Material> &materials;

        /** @brief Device texture-object array (indexed via Material tex fields). */
        const optix::CudaBuffer<cudaTextureObject_t> &texture_objects;

        // ---- Environment map (from SceneLoader) ----

        /** @brief Env cubemap texture object (0 = no env map loaded). */
        cudaTextureObject_t env_cubemap = 0;

        /** @brief Device alias table (null when no env map loaded). */
        const EnvAliasEntry *env_alias_table = nullptr;

        /** @brief Alias table entry count (0 when no env map loaded). */
        uint32_t env_alias_count = 0;

        /** @brief Alias table width (equirect source width). */
        uint32_t env_alias_width = 0;

        /** @brief Alias table height (equirect source height). */
        uint32_t env_alias_height = 0;

        /** @brief Sin-weighted total luminance across the environment map. */
        float env_total_luminance = 0.0f;

        // ---- Emissive triangles (from SceneLoader) ----

        /** @brief Device emissive triangle array (null when no emissive geometry). */
        const EmissiveTriangle *emissive_triangles = nullptr;

        /** @brief Device alias table over emissive triangles (null when no emissive geometry). */
        const AliasEntry *emissive_alias_table = nullptr;

        /** @brief Number of emissive triangles (0 when no emissive geometry). */
        uint32_t emissive_count = 0;

        /** @brief Total radiant power across all emissive triangles. */
        float emissive_total_power = 0.0f;

        /** @brief Frame delta time in milliseconds (for DLSS InFrameTimeDeltaInMsec). */
        float frame_time_ms = 0.0f;

        /** @brief DLSS render preset for feature recreation on resolution change. */
        optix::DlssRenderPreset dlss_preset = optix::DlssRenderPreset::E;
    };

    /**
     * @brief Single-frame render content: CUDA submit + Vulkan command recording.
     *
     * Encapsulates what one frame draws — the CUDA write into the display buffer,
     * the Vulkan blit to the swapchain image, and the ImGui overlay — so the
     * Application keeps only the frame-loop timing skeleton. Owns the OptiX
     * pipeline, SBT record buffers, ping-pong HDR accumulation buffers, and the
     * device-side launch-params buffer; the frame counter driving temporal
     * animation is likewise owned state.
     */
    class Renderer {
    public:
        /**
         * @brief Creates all OptiX render resources sized for the given output.
         *
         * @param cuda_context CUDA context owning the stream and device context the
         *                      pipeline builds against; the stream sequences SBT
         *                      uploads and accumulation-buffer clears before the first
         *                      frame's optixLaunch on the same stream.
         * @param width          Initial render resolution width in pixels
         *                       (accumulation buffer size).
         * @param height         Initial render resolution height in pixels.
         * @param optixir_path   Path to the compiled .optixir file (passed to the
         *                       pipeline; resolved relative to the process working
         *                       directory, so the build must deploy the file there).
         */
        void init(const optix::Context &cuda_context,
                  uint32_t width,
                  uint32_t height,
                  const std::string &optixir_path);

        /**
         * @brief Releases all OptiX render resources.
         *
         * Idempotent: owned handles are reset, so a repeat call is a no-op (matches
         * the optix layer's destroy convention).
         */
        void destroy();

        /**
         * @brief Builds acceleration structures and the geometry-info buffer from
         *        loaded scene data.
         *
         * Independent of init(): callable any time after init to load or switch
         * scenes. Destroys the previously built scene resources first (AS +
         * geometry-info buffer). Each geometry's BLAS opaque flag is read from
         * Mesh::opaque (set by SceneLoader from glTF alpha_mode at load time).
         * An empty mesh list skips AS construction, leaving the TLAS handle at 0
         * (submit_cuda must then keep the traversable at 0 so raygen skips optixTrace).
         *
         * @param cuda_context CUDA context (device context + compute stream for AS
         *                     builds and buffer uploads).
         * @param meshes       Loaded meshes (one per glTF primitive).
         * @param instances    Scene mesh instances (one per node-primitive).
         */
        void load_scene(const optix::Context &cuda_context,
                        std::span<const Mesh> meshes,
                        std::span<const MeshInstance> instances);

        /**
         * @brief Submits raygen and tonemap on two CUDA streams, then signals the semaphore.
         *
         * compute_stream: waits until the write slot is no longer displayed,
         * then uploads params, launches raygen, and records that slot as produced.
         * display_stream waits until the read slot is produced plus the reverse
         * semaphore, evaluates DLSS/tonemaps, records that slot as consumed, and
         * signals the forward semaphore. The two streams run in parallel while
         * slot-indexed CUDA events protect color and guide resources together.
         *
         * The render resolution derives from scene.settings.render_height and the
         * display aspect ratio; when it differs from the current accumulation-buffer
         * allocation, both streams are drained and the buffers are reallocated
         * (sample counts reset to 0).
         * @param cuda_context CUDA context (surface, streams, external semaphores).
         * @param dlss_rr      DLSS-RR feature (owned by the caller; create/evaluate/release
         *                     driven on this path while feature_active).
         * @param scene        Camera and scene data (materials, texture objects).
         * @param width        Display buffer width in pixels.
         * @param height       Display buffer height in pixels.
         * @param frame_index  Current frame-in-flight slot, indexing external_semaphores.
         */
        void submit_cuda(const optix::Context &cuda_context,
                         optix::DlssRR &dlss_rr,
                         const SceneRenderInput &scene,
                         uint32_t width,
                         uint32_t height,
                         uint32_t frame_index);

        /**
         * @brief Records the Vulkan command sequence (blit, ImGui, layout transitions).
         * @param input Per-frame handles (see RenderInput). cmd must be begun by caller.
         */
        static void record_vulkan(const RenderInput &input);

        /**
         * @brief Number of samples accumulated in the buffer currently being read
         *        by tonemap (visible on screen).
         *
         * DebugUI displays this as the live sample counter. The value reflects the
         * read slot's count, which is the latest fully-written accumulation total.
         */
        [[nodiscard]] uint32_t accumulated_samples() const;

        /** @brief Actual TLAS instance count after group folding (set by load_scene). */
        [[nodiscard]] uint32_t tlas_instance_count() const;

        /**
         * @brief Forces the next frame to overwrite instead of accumulating,
         *        and discards DLSS-RR temporal history.
         *
         * Sets deferred flags consumed by submit_cuda: chain_count=0 triggers
         * accumulation overwrite, and the next complete DLSS input carries a
         * history-reset token in its slot metadata.
         * Used for scene switch, camera teleport, env map reload, manual Reset.
         * Continuous camera motion and content changes (env rotation, DLSS
         * toggle) trigger accumulation reset through camera_changed /
         * content_changed in submit_cuda, which intentionally does NOT discard
         * DLSS history. Quality/throughput knobs (max_bounces, samples_per_frame)
         * do not reset accumulation.
         */
        void reset_accumulation();

    private:
        /**
         * @brief Camera matrices that define the primary-ray integral domain.
         *
         * Packed for a single defaulted equality check so accumulation reset
         * detection compares the full camera key without parallel prev_ members.
         */
        struct CameraKey {
            /** @brief Inverse view matrix (view → world). */
            glm::mat4 inv_view{1.0f};

            /** @brief Inverse projection matrix (clip → view). */
            glm::mat4 inv_projection{1.0f};

            /** @brief Memberwise exact float comparison of both matrices. */
            bool operator==(const CameraKey &) const = default;
        };

        /**
         * @brief Six render-resolution guide resources belonging to one color slot.
         *
         * Keeping the resources under one slot owner prevents DLSS from mixing
         * guide data with a color buffer produced by another frame.
         */
        struct AuxBufferSet {
            /** @brief Allocates every guide resource at the same resolution. */
            void alloc(uint32_t width, uint32_t height);

            /** @brief Resizes every guide resource to the same resolution. */
            void resize(uint32_t width, uint32_t height);

            /** @brief Releases every guide resource. */
            void free();

            /** @brief View-space Z depth (R32F). */
            optix::CudaArrayBuffer<float> depth;

            /** @brief Screen-space motion vectors (RG32F). */
            optix::CudaArrayBuffer<float2> motion_vectors;

            /** @brief Raw base-color diffuse albedo (RGBA32F). */
            optix::CudaArrayBuffer<float4> diffuse_albedo;

            /** @brief Specular reflectance albedo (RGBA32F). */
            optix::CudaArrayBuffer<float4> specular_albedo;

            /** @brief World-space shading normal (RGBA32F, .w unused). */
            optix::CudaArrayBuffer<float4> normals;

            /** @brief Linear roughness (R32F). */
            optix::CudaArrayBuffer<float> roughness;
        };

        /**
         * @brief Host inputs produced with one color/aux slot.
         *
         * These values travel with their GPU resources so DLSS evaluation never
         * combines previous-frame images with current-frame camera state.
         */
        struct DlssFrameMetadata {
            /** @brief Raw horizontal Sobol jitter in [0,1). */
            float jitter_x = 0.0f;

            /** @brief Raw vertical Sobol jitter in [0,1). */
            float jitter_y = 0.0f;

            /** @brief World-to-view matrix for this input frame. */
            glm::mat4 view_matrix{1.0f};

            /** @brief View-to-clip matrix for this input frame. */
            glm::mat4 projection_matrix{1.0f};

            /** @brief Frame delta supplied with this input frame, in milliseconds. */
            float frame_time_ms = 0.0f;

            /** @brief Whether evaluation of this input discards DLSS history. */
            bool reset = false;

            /** @brief Whether this slot contains a complete DLSS input frame. */
            bool valid = false;
        };

        /**
         * @brief One ping-pong resource slot: color, guides, sample count, DLSS
         *        metadata, and the production/consumption CUDA events that fence them.
         *
         * Keeping these six previously parallel arrays under one owner makes it
         * impossible to resize color without resizing guides, or to evaluate DLSS
         * with metadata from a different frame than the textures.
         */
        struct FrameSlot {
            /** @brief Allocates color and guide resources at the given resolution. */
            void alloc(uint32_t width, uint32_t height);

            /**
             * @brief Resizes color and guide resources and zeros sample_count.
             *
             * Resized content is undefined; sample_count must not claim prior
             * accumulation. DLSS metadata is left to invalidate().
             */
            void resize(uint32_t width, uint32_t height);

            /** @brief Releases color and guide resources. */
            void free();

            /**
             * @brief Marks DLSS metadata as not containing a valid input frame.
             *
             * Does not touch sample_count or GPU resources.
             */
            void invalidate();

            /**
             * @brief Creates production/consumption sync events and records them once
             *        so the first waits pass without reading unrecorded events.
             */
            void create_events(cudaStream_t stream);

            /** @brief Destroys production/consumption sync events. */
            void destroy_events();

            /** @brief HDR color buffer (RGBA32F, CUDA array + tex/surf). */
            optix::CudaArrayBuffer<float4> color;

            /** @brief Six DLSS guide resources matching color's resolution. */
            AuxBufferSet aux;

            /**
             * @brief Normalization count paired with color.
             *
             * DLSS OFF stores a Separate Sum and its accumulated sample count.
             * DLSS ON stores a per-frame mean and therefore uses count 1.
             */
            uint32_t sample_count = 0;

            /** @brief Host inputs produced with this slot's color/aux. */
            DlssFrameMetadata dlss_metadata{};

            /**
             * @brief Recorded after raygen produces this slot's color and guides.
             *
             * Display waits on this before consuming the slot.
             */
            cudaEvent_t production_event = nullptr;

            /**
             * @brief Recorded after display finishes consuming this slot.
             *
             * Compute waits on this before overwriting the slot.
             */
            cudaEvent_t consumption_event = nullptr;
        };

        /**
         * @brief Invalidates DLSS input slots and requests history reset.
         *
         * Marks both slots as not containing a valid DLSS input and sets a
         * pending reset token. Does NOT clear dlss_output_valid_ — the last
         * DLSS output remains displayable as a frozen frame (e.g. during
         * paused camera motion).
         */
        void invalidate_dlss_history();

        /**
         * @brief Full DLSS state invalidation: history + cached output.
         *
         * Calls invalidate_dlss_history() and additionally clears
         * dlss_output_valid_, so the display falls back to raw-color tonemap.
         * Used when the output buffer itself is reallocated or the scene
         * content changes (feature lifecycle, resize, scene switch).
         */
        void invalidate_dlss_state();

        /** @brief OptiX pipeline (module, program groups, linked handle). */
        optix::Pipeline pipeline_;

        /** @brief Raygen SBT record buffer (single record, no user data). */
        optix::CudaBuffer<SbtRecord> sbt_raygen_;
        /** @brief Miss SBT record buffer (single record, no user data). */
        optix::CudaBuffer<SbtRecord> sbt_miss_;
        /** @brief Hit-group SBT record buffer (single header-only record). */
        optix::CudaBuffer<SbtRecord> sbt_hit_;

        /** @brief Scene acceleration structures (BLAS per group_id + single TLAS). */
        optix::AccelStructure accel_;

        /** @brief Device-side per-geometry RT query data (GPUGeometryInfo[]). */
        optix::CudaBuffer<GPUGeometryInfo> geometry_info_buffer_;

        /**
         * @brief Ping-pong resource slots (color + guides + count + metadata + events).
         *
         * A produced frame reads slot [accum_index_] and writes [1 - accum_index_],
         * then flips the index. A paused frame keeps the index unchanged. CUDA
         * arrays are required for DLSS CUDA API resource consumption.
         */
        std::array<FrameSlot, 2> frame_slots_;

        /** @brief Device-side launch-params buffer (one LaunchParams). */
        optix::CudaBuffer<LaunchParams> params_buffer_;

        /** @brief Read slot index; flipped only after producing a new sample frame. */
        uint32_t accum_index_ = 0;

        /**
         * @brief Render resolution width the accumulation buffers are allocated for.
         *
         * submit_cuda compares the desired render resolution (derived from
         * RenderSettings::render_height and the display aspect ratio) against
         * this pair and reallocates the buffers on mismatch.
         */
        uint32_t render_width_ = 0;

        /** @brief Render resolution height the accumulation buffers are allocated for (see render_width_). */
        uint32_t render_height_ = 0;

        /**
         * @brief Monotonic frame counter; never reset.
         *
         * Uploaded as LaunchParams::frame_index for device-side temporal
         * variation and indexes debug timing events. Resource ownership uses
         * accum_index_ independently, so paused frames do not flip resources.
         */
        uint32_t frame_counter_ = 0;

        /** @brief Actual TLAS instance count after same-node primitive folding. */
        uint32_t tlas_instance_count_ = 0;

        // ---- DLSS-RR output (display resolution) ----

        /**
         * @brief Intermediate HDR buffer at output resolution (RGBA32F).
         *
         * DLSS-RR writes its denoised+upscaled result here (via surfaceObject);
         * tonemap reads from here (via textureObject) instead of the accumulation
         * buffer when DLSS-RR is active. Allocated at display resolution, resized
         * on window resize (not on render resolution change).
         */
        optix::CudaArrayBuffer<float4> dlss_output_;

        /** @brief Display width the DLSS output buffer is allocated for. */
        uint32_t dlss_output_width_ = 0;

        /** @brief Display height the DLSS output buffer is allocated for. */
        uint32_t dlss_output_height_ = 0;

        /** @brief Whether dlss_output_ represents a valid evaluation result. */
        bool dlss_output_valid_ = false;

        /**
         * @brief Deferred accumulation reset flag set by reset_accumulation().
         *
         * Consumed by submit_cuda on the next frame: forces chain_count to 0
         * (same path as camera-change reset) without clearing FrameSlot::sample_count,
         * so the read slot keeps a valid count and tonemap avoids a black frame.
         */
        bool reset_requested_ = false;

        /**
         * @brief Deferred DLSS history reset flag set by reset_accumulation().
         *
         * The next complete DLSS input slot receives the token; evaluation
         * consumes it together with that slot's color, guides, and metadata.
         */
        bool dlss_reset_requested_ = false;

        /**
         * @brief Previous-frame camera key (accumulation-reset detection).
         *
         * Any change in inv_view or inv_projection zeros chain_count so raygen
         * overwrites. During pause, a camera change also invalidates DLSS history.
         */
        CameraKey prev_camera_{};

        /**
         * @brief Previous-frame env_rotation (content-change reset detection).
         *
         * Content changes reset accumulation but do not invalidate DLSS history
         * during pause (continuous lighting change; DLSS adapts temporally).
         */
        float prev_env_rotation_ = 0.0f;

        /**
         * @brief Previous-frame dlss_enabled (content-change reset detection).
         *
         * Toggle still resets Separate Sum chain when leaving/entering DLSS;
         * feature-lifecycle invalidation is handled on the create/release path.
         */
        bool prev_dlss_enabled_ = false;

        /** @brief Previous-frame DLSS render preset (feature-recreation detection). */
        optix::DlssRenderPreset prev_dlss_preset_ = optix::DlssRenderPreset::E;

#ifndef NDEBUG
        /** @brief Timing event recorded before display_stream work (DLSS + tonemap). */
        std::array<cudaEvent_t, 2> event_display_start_{};

        /** @brief Timing event recorded after display_stream work. */
        std::array<cudaEvent_t, 2> event_display_end_{};

        /** @brief Timing event recorded before compute_stream work (params upload + raygen). */
        std::array<cudaEvent_t, 2> event_pt_start_{};

        /** @brief Timing event recorded after compute_stream work (raygen done). */
        std::array<cudaEvent_t, 2> event_pt_end_{};

        /** @brief Most recent CUDA display-stream elapsed time in milliseconds. */
        float cuda_display_ms_ = 0.0f;

        /** @brief Most recent PT (raygen) elapsed time in milliseconds. */
        float pt_ms_ = 0.0f;
    public:
        /** @brief CUDA display pipeline time (debug only). */
        [[nodiscard]] float cuda_display_ms() const { return cuda_display_ms_; }

        /** @brief PT (raygen) pipeline time (debug only). */
        [[nodiscard]] float pt_ms() const { return pt_ms_; }
    private:
#endif
    };
} // namespace qualquer::renderer