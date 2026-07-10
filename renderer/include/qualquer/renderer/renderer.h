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
#include <qualquer/optix/pipeline.h>
#include <qualquer/renderer/camera.h>
#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/material.h>
#include <qualquer/renderer/render_settings.h>
#include <qualquer/renderer/scene_types.h>

namespace qualquer::optix {
    class Context;
    class DlssRR;
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
        VkCommandBuffer cmd;

        /** @brief CUDA context: surface, stream, external semaphores. */
        optix::Context &cuda_context;

        /** @brief Display buffer (interop image) blitted to the swapchain image. */
        vulkan::InteropImage &display_buffer;

        /** @brief Swapchain providing the target image and its view. */
        vulkan::Swapchain &swapchain;

        /** @brief Index of the acquired swapchain image for this frame. */
        uint32_t image_index;

        /** @brief Graphics queue family index for display buffer ownership transfers. */
        uint32_t graphics_queue_family;

        /** @brief ImGui backend for overlay recording. */
        vulkan::ImGuiBackend &imgui;
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
        cudaTextureObject_t env_cubemap;

        /** @brief Device alias table (null when no env map loaded). */
        const EnvAliasEntry *env_alias_table;

        /** @brief Alias table entry count (0 when no env map loaded). */
        uint32_t env_alias_count;

        /** @brief Alias table width (equirect source width). */
        uint32_t env_alias_width;

        /** @brief Alias table height (equirect source height). */
        uint32_t env_alias_height;

        /** @brief Sin-weighted total luminance across the environment map. */
        float env_total_luminance;

        // ---- Emissive triangles (from SceneLoader) ----

        /** @brief Device emissive triangle array (null when no emissive geometry). */
        const EmissiveTriangle *emissive_triangles;

        /** @brief Device alias table over emissive triangles (null when no emissive geometry). */
        const AliasEntry *emissive_alias_table;

        /** @brief Number of emissive triangles (0 when no emissive geometry). */
        uint32_t emissive_count;

        /** @brief Total radiant power across all emissive triangles. */
        float emissive_total_power;
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
         * compute_stream: waits previous tonemap done → params upload + optixLaunch →
         * records raygen done. display_stream: waits previous raygen done + waits
         * reverse semaphore → tonemap → records tonemap done → signals forward
         * semaphore. The two streams run in parallel; CUDA events enforce the
         * ping-pong buffer dependencies, the reverse semaphore protects the display
         * surface's write-after-read dependency (blit read before tonemap write).
         *
         * The render resolution derives from scene.settings.render_height and the
         * display aspect ratio; when it differs from the current accumulation-buffer
         * allocation, both streams are drained and the buffers are reallocated
         * (sample counts reset to 0).
         * @param cuda_context CUDA context (surface, streams, external semaphores).
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
         * @brief Forces the next frame to overwrite instead of accumulating.
         *
         * Sets a deferred flag consumed by the next submit_cuda, which forces
         * chain_count=0 without clearing per-slot counts (same path as camera
         * reset, avoids black-frame flash).
         */
        void reset_accumulation();

    private:
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
         * @brief Ping-pong HDR accumulation buffers (RGBA32F, CUDA-owned).
         *
         * Frame N reads buffer [accum_index_] and writes [1 - accum_index_], then
         * flips the index for the next frame; see technical-decisions.md.
         */
        std::array<optix::CudaBuffer<float4>, 2> accum_buffers_;

        /** @brief Device-side launch-params buffer (one LaunchParams). */
        optix::CudaBuffer<LaunchParams> params_buffer_;

        /** @brief Index into accum_buffers_ of the buffer read this frame; flipped per frame. */
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
         * Indexes the ping-pong buffers and CUDA events via frame_counter_ % 2,
         * and is uploaded as LaunchParams::frame_index for device-side temporal
         * variation (e.g. RNG seed). Never reset so both uses advance
         * monotonically across the session.
         */
        uint32_t frame_counter_ = 0;

        /**
         * @brief Per-slot sample count: accum_counts_[i] is the number of samples
         *        whose contributions are summed in accum_buffers_[i].
         *
         * Paired with the buffer contents so tonemap always divides by the
         * count that matches what it reads, even across reset boundaries.
         * On reset only the chain_count (fed to raygen) drops to 0; the read
         * slot's count stays valid until raygen overwrites that buffer.
         */
        std::array<uint32_t, 2> accum_counts_ = {0, 0};

        /** @brief Actual TLAS instance count after same-node primitive folding. */
        uint32_t tlas_instance_count_ = 0;

        // ---- Aux G-buffer channels (render resolution, for DLSS-RR input) ----

        /** @brief View-space Z depth (R32F). */
        optix::CudaArrayBuffer<float> aux_depth_;

        /** @brief Screen-space motion vectors (RG32F). */
        optix::CudaArrayBuffer<float2> aux_motion_vectors_;

        /** @brief Raw base_color diffuse albedo (RGBA32F). */
        optix::CudaArrayBuffer<float4> aux_diffuse_albedo_;

        /** @brief Specular reflectance albedo (RGBA32F). */
        optix::CudaArrayBuffer<float4> aux_specular_albedo_;

        /** @brief World-space shading normal (RGBA32F, .w unused). */
        optix::CudaArrayBuffer<float4> aux_normals_;

        /** @brief Linear roughness (R32F). */
        optix::CudaArrayBuffer<float> aux_roughness_;

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

        /**
         * @brief Deferred reset flag set by reset_accumulation().
         *
         * Consumed by submit_cuda on the next frame: forces chain_count to 0
         * (same path as camera-change reset) without clearing accum_counts_,
         * so the read slot keeps a valid count and tonemap avoids a black frame.
         */
        bool reset_requested_ = false;

        /**
         * @brief Previous-frame inverse view matrix (accumulation-reset detection).
         *
         * Exact byte-compare against the current frame's inv_view; any change
         * (camera move) zeros chain_count so raygen overwrites.
         */
        glm::mat4 prev_inv_view_{1.0f};

        /**
         * @brief Previous-frame inverse projection matrix (accumulation-reset detection).
         *
         * Exact byte-compare against the current frame's inv_projection; any change
         * (fov/aspect/near/far) zeros chain_count so raygen overwrites.
         */
        glm::mat4 prev_inv_projection_{1.0f};

        /** @brief Previous-frame max_bounces (accumulation-reset detection). */
        uint32_t prev_max_bounces_ = 16;

        /** @brief Previous-frame samples_per_frame (accumulation-reset detection). */
        uint32_t prev_samples_per_frame_ = 1;

        /** @brief Previous-frame env_rotation (accumulation-reset detection). */
        float prev_env_rotation_ = 0.0f;

        /**
         * @brief Previous-frame unjittered view-projection matrix (row-major).
         *
         * Cached at frame end; uploaded as LaunchParams::prev_view_projection
         * for the next frame's motion vector computation.
         */
        float4x4 prev_view_projection_{
            .rows = {
                make_float4(1, 0, 0, 0),
                make_float4(0, 1, 0, 0),
                make_float4(0, 0, 1, 0),
                make_float4(0, 0, 0, 1),
            },
        };

        /**
         * @brief Recorded after raygen completes on compute_stream.
         *
         * The next frame's tonemap waits on the previous slot's event to ensure the
         * accumulation buffer it reads has been fully written.
         * Double-buffered by frame_counter_ % 2.
         */
        std::array<cudaEvent_t, 2> event_raygen_done_{};

        /**
         * @brief Recorded after tonemap completes on display_stream.
         *
         * The next frame's raygen waits on the previous slot's event to ensure the
         * accumulation buffer it writes is no longer being read by tonemap.
         * Double-buffered by frame_counter_ % 2.
         */
        std::array<cudaEvent_t, 2> event_tonemap_done_{};
    };
} // namespace qualquer::renderer