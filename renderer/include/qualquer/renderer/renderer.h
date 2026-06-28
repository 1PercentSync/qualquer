#pragma once

/**
 * @file renderer.h
 * @brief Renderer: single-frame render content (CUDA submit + Vulkan recording).
 */

#include <cuda_runtime.h>
#include <optix.h>
#include <vulkan/vulkan.h>

#include <array>
#include <string>

#include <qualquer/optix/cuda_buffer.h>
#include <qualquer/optix/pipeline.h>
#include <qualquer/renderer/launch_params.h>

namespace qualquer::optix {
    class Context;
}

namespace qualquer::vulkan {
    class InteropImage;

    class Swapchain;
}

namespace qualquer::renderer {
    class ImGuiBackend;

    /**
     * @brief OptiX SBT record carrying only the opaque program header.
     *
     * The raygen/miss/hit records carry no user data, so the record is the
     * 32-byte header alone. Defined at namespace scope because it appears as a
     * CudaBuffer template argument in the Renderer members. An explicit
     * alignment replaces the OptiX example's @c __align__ compiler extension so
     * the same record type is well-formed in host C++20.
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

        /** @brief ImGui backend for overlay recording. */
        ImGuiBackend &imgui;
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
         * @param width          Output width in pixels.
         * @param height         Output height in pixels.
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
         * @brief Rebuilds resolution-dependent resources after a swapchain resize.
         *
         * Resizes and clears the accumulation buffers on the context's stream so the
         * clear completes before the next frame's optixLaunch; the pipeline and SBT
         * records are resolution-independent and stay.
         * @param cuda_context CUDA context owning the stream the clears run on.
         * @param width  New output width in pixels.
         * @param height New output height in pixels.
         */
        void resize(const optix::Context &cuda_context, uint32_t width, uint32_t height);

        /**
         * @brief Submits raygen and tonemap on two CUDA streams, then signals the semaphore.
         *
         * compute_stream: waits previous tonemap done → params upload + optixLaunch →
         * records raygen done. display_stream: waits previous raygen done → tonemap →
         * records tonemap done → signals external semaphore. The two streams run in
         * parallel; CUDA events enforce the ping-pong buffer dependencies across frames.
         * @param cuda_context CUDA context (surface, streams, external semaphores).
         * @param width        Display buffer width in pixels.
         * @param height       Display buffer height in pixels.
         * @param frame_index  Current frame-in-flight slot, indexing external_semaphores.
         */
        void submit_cuda(const optix::Context &cuda_context, uint32_t width, uint32_t height, uint32_t frame_index);

        /**
         * @brief Records the Vulkan command sequence (blit, ImGui, layout transitions).
         * @param input Per-frame handles (see RenderInput). cmd must be begun by caller.
         */
        static void record_vulkan(const RenderInput &input);

    private:
        /** @brief OptiX pipeline (module, program groups, linked handle). */
        optix::Pipeline pipeline_;

        /** @brief Raygen SBT record buffer (single record, no user data). */
        optix::CudaBuffer<SbtRecord> sbt_raygen_;
        /** @brief Miss SBT record buffer (single record, no user data). */
        optix::CudaBuffer<SbtRecord> sbt_miss_;
        /** @brief Hit-group SBT record buffer (single record, no user data). */
        optix::CudaBuffer<SbtRecord> sbt_hit_;

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

        /** @brief Monotonic frame counter driving temporal animation. */
        uint32_t frame_counter_ = 0;

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