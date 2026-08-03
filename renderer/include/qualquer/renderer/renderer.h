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

        /** @brief Runtime render settings (bounces, spp, exposure, render toggle). */
        const RenderSettings &settings;

        /** @brief Device material array (indexed via GPUGeometryInfo::material_buffer_offset). */
        const optix::CudaBuffer<Material> &materials;

        /** @brief Device texture-object array (indexed via Material tex fields). */
        const optix::CudaBuffer<cudaTextureObject_t> &texture_objects;

        /**
         * @brief Environment-map light resources from SceneLoader.
         *
         * Device pointers are borrowed for the duration of submit_cuda; the
         * owning CudaBuffer / CudaTexture remain in SceneLoader.
         */
        EnvLightData env{};

        /**
         * @brief Emissive-triangle light resources from SceneLoader.
         *
         * Device pointers are borrowed for the duration of submit_cuda; the
         * owning CudaBuffers remain in SceneLoader.
         */
        EmissiveLightData emissive{};

        /** @brief Frame delta time in milliseconds (for DLSS InFrameTimeDeltaInMsec). */
        float frame_time_ms = 0.0f;
    };

    /**
     * @brief Single-frame render content: CUDA submit + Vulkan command recording.
     *
     * Encapsulates what one frame draws — the CUDA write into the display buffer,
     * the Vulkan blit to the swapchain image, and the ImGui overlay — so the
     * Application keeps only the frame-loop timing skeleton. Owns the OptiX
     * pipeline, SBT record buffers, HDR color buffer, the device-side
     * launch-params buffer, and the DLSS-RR feature lifecycle; the frame counter
     * driving temporal animation is likewise owned state.
     */
    class Renderer {
    public:
        /**
         * @brief Creates all OptiX render resources sized for the given output.
         *
         * Initializes DLSS-RR (NGX SDK) from the CUDA device selected by
         * cuda_context, then builds the OptiX pipeline and frame resources.
         *
         * @param cuda_context CUDA context owning the stream and device context the
         *                      pipeline builds against; the stream sequences SBT
         *                      uploads and color-buffer allocation before the first
         *                      frame's optixLaunch on the same stream. Its device_id
         *                      also seeds DlssRR::init.
         * @param width          Initial render resolution width in pixels
         *                       (color buffer size).
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
         * @brief Submits raygen and display work on two CUDA streams (serial).
         *
         * compute_stream: waits for the previous display pass to finish reading
         * the color buffer, then uploads params and launches raygen.
         * display_stream: waits for raygen + reverse interop semaphore, then
         * evaluates DLSS (if ON) / tonemaps, and signals the forward semaphore.
         * The two streams execute serially (display waits for compute via event).
         *
         * The render resolution derives from scene.settings.render_height and the
         * display aspect ratio; on mismatch both streams are drained and the
         * buffer is reallocated (sample_count reset to 0). DLSS-RR feature
         * create/evaluate/release is driven from dlss_rr_.
         * @param cuda_context CUDA context (surface, streams, external semaphores).
         * @param scene        Camera and scene data (materials, texture objects).
         * @param width        Display buffer width in pixels.
         * @param height       Display buffer height in pixels.
         * @param frame_index  Current frame-in-flight slot, indexing external_semaphores.
         */
        void submit_cuda(const optix::Context &cuda_context,
                         const SceneRenderInput &scene,
                         uint32_t width,
                         uint32_t height,
                         uint32_t frame_index);

        /**
         * @brief Records the Vulkan command sequence (blit, ImGui, layout transitions).
         * @param input Per-frame handles (see RenderInput). cmd must be begun by caller.
         */
        static void record_vulkan(const RenderInput &input);

        /** @brief Actual TLAS instance count after group folding (set by load_scene). */
        [[nodiscard]] uint32_t tlas_instance_count() const;

        /**
         * @brief DLSS-RR wrapper owned by this renderer (read-only borrow).
         *
         * DebugUIContext uses this for availability / quality / VRAM queries,
         * matching the existing const vulkan::Context& borrow pattern.
         */
        [[nodiscard]] const optix::DlssRR &dlss() const { return dlss_rr_; }

        /**
         * @brief Discards DLSS-RR temporal history and cached output.
         *
         * Sets deferred flags consumed by submit_cuda: the next complete DLSS
         * input carries a history-reset token.
         * Used for scene switch, camera teleport, env map reload, manual Reset.
         */
        void reset_dlss();

    private:
        /**
         * @brief Camera matrices that define the primary-ray integral domain.
         *
         * Packed for a single defaulted equality check so DLSS history
         * invalidation compares the full camera key without parallel prev_ members.
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

            /** @brief Whether the guide resources are allocated. */
            [[nodiscard]] bool valid() const { return depth.valid(); }

            /** @brief View-space Z depth (R32F). */
            optix::CudaArrayBuffer<float> depth;

            /** @brief Screen-space motion vectors (RG32F). */
            optix::CudaArrayBuffer<float2> motion_vectors;

            /** @brief Raw base-color diffuse albedo (RGBA8 UNORM). */
            optix::CudaArrayBuffer<uchar4> diffuse_albedo;

            /** @brief Specular reflectance albedo (RGBA32F). */
            optix::CudaArrayBuffer<float4> specular_albedo;

            /** @brief World-space shading normal (xyz) + linear roughness (w), RGBA16F. */
            optix::CudaArrayBuffer<float4> normal_roughness;
        };

        /**
         * @brief Previous-frame DLSS input metadata for motion vector computation.
         *
         * Stores the jitter and camera matrices from the last frame that
         * produced a valid DLSS input, so the current frame can compute
         * previous_vp for motion vectors and has_temporal_predecessor.
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

            /** @brief Whether this contains a valid DLSS input frame. */
            bool valid = false;
        };

        /**
         * @brief Color buffer and DLSS guide resources with serial sync events.
         *
         * Single instance (no ping-pong). production_event and consumption_event
         * enforce the serial compute → display ordering across frames: raygen
         * waits for consumption before writing, display waits for production
         * before reading.
         */
        struct FrameSlot {
            /**
             * @brief Allocates color resources at the given resolution.
             *
             * Aux guides are not allocated here — they are managed by the
             * DLSS lifecycle in submit_cuda (allocated on enable, freed on
             * disable) to avoid wasting VRAM when DLSS is off.
             */
            void alloc(uint32_t width, uint32_t height);

            /**
             * @brief Resizes color resources and zeros sample_count.
             *
             * Aux guides are resized separately when allocated. Resized
             * content is undefined.
             */
            void resize(uint32_t width, uint32_t height);

            /** @brief Releases color and guide resources. */
            void free();

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
             * @brief Color buffer validity flag.
             *
             * 0 after alloc/resize (uninitialised); 1 after raygen writes a
             * valid per-frame mean. Tonemap outputs black when 0.
             */
            uint32_t sample_count = 0;

            /**
             * @brief Recorded after raygen produces color and guides.
             *
             * display_stream waits on this before reading.
             */
            cudaEvent_t production_event = nullptr;

            /**
             * @brief Recorded after display finishes reading color.
             *
             * compute_stream waits on this before the next raygen overwrites.
             */
            cudaEvent_t consumption_event = nullptr;
        };

        /**
         * @brief Requests DLSS history reset.
         *
         * Sets the pending reset token so the next DLSS evaluation discards
         * temporal history. Does NOT clear dlss_output_valid_ — the last
         * DLSS output remains displayable as a frozen frame.
         */
        void invalidate_dlss_history();

        /**
         * @brief Full DLSS state invalidation: history + cached output.
         *
         * Calls invalidate_dlss_history() and additionally clears
         * dlss_output_valid_, so the display falls back to raw-color tonemap.
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
         * @brief Single color/aux resource slot with serial sync events.
         *
         * Raygen writes per-frame mean; display reads after raygen finishes.
         * Aux guides allocated on demand when DLSS is enabled.
         * CUDA arrays are required for DLSS CUDA API resource consumption.
         */
        FrameSlot frame_slot_;

        /** @brief Device-side launch-params buffer (one LaunchParams). */
        optix::CudaBuffer<LaunchParams> params_buffer_;

        /**
         * @brief Per-frame pinned host staging buffers for launch-params upload.
         *
         * Page-locked memory ensures cudaMemcpyAsync is truly asynchronous — the
         * DMA engine reads directly from host memory without driver-side staging
         * or implicit stream synchronization that pageable memory may cause.
         *
         * Double-buffered because the CPU writes next frame's params before the
         * GPU's DMA may have read the current frame's staging data (the fence
         * only guarantees frame N-2 completion, not frame N-1).
         */
        std::array<LaunchParams *, 2> params_staging_ = {nullptr, nullptr};

        /**
         * @brief Render resolution width the color buffer is allocated for.
         *
         * submit_cuda compares the desired render resolution (derived from
         * RenderSettings::render_height and the display aspect ratio) against
         * this pair and reallocates the buffer on mismatch.
         */
        uint32_t render_width_ = 0;

        /** @brief Render resolution height the color buffer is allocated for (see render_width_). */
        uint32_t render_height_ = 0;

        /**
         * @brief Monotonic frame counter; never reset.
         *
         * Indexes debug timing events and seeds the global DLSS jitter.
         */
        uint32_t frame_counter_ = 0;

        /**
         * @brief Cumulative Sobol sequence base for the next frame.
         *
         * Advances by samples_per_frame after each frame that produces samples.
         * Monotonically increasing regardless of dynamic spp changes — unlike
         * frame_index * spp which can repeat or skip indices when spp varies.
         */
        uint32_t sequence_base_ = 0;

        /** @brief Actual TLAS instance count after same-node primitive folding. */
        uint32_t tlas_instance_count_ = 0;

        // ---- DLSS-RR (owned lifecycle + display-resolution output) ----

        /**
         * @brief DLSS Ray Reconstruction feature (NGX lifecycle + create/evaluate).
         *
         * Owned here because submit_cuda manages the full feature lifecycle
         * (create/release/recreate/evaluate). Initialized in init() from the
         * CUDA device id; destroyed in destroy().
         */
        optix::DlssRR dlss_rr_;

        /**
         * @brief Intermediate HDR buffer at output resolution (RGBA32F).
         *
         * DLSS-RR writes its denoised+upscaled result here (via surfaceObject);
         * tonemap reads from here (via textureObject) instead of the color
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
         * @brief Deferred DLSS history reset flag set by reset_dlss().
         *
         * The next complete DLSS input slot receives the token; evaluation
         * consumes it together with that slot's color, guides, and metadata.
         */
        bool dlss_reset_requested_ = false;

        /**
         * @brief Previous-frame camera key (DLSS history invalidation).
         *
         * Camera change during pause invalidates DLSS history.
         */
        CameraKey prev_camera_{};

        /** @brief Previous-frame max_clamp (DLSS history invalidation on bias change). */
        float prev_max_clamp_ = 10.0f;

        /** @brief Previous-frame DLSS render preset (feature-recreation detection). */
        optix::DlssRenderPreset prev_dlss_preset_ = optix::DlssRenderPreset::E;

        /**
         * @brief Previous-frame DLSS metadata for motion vector computation.
         *
         * Saved at the end of each producing frame so the next frame can
         * derive prev_view_projection and has_temporal_predecessor.
         */
        DlssFrameMetadata prev_dlss_metadata_{};

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