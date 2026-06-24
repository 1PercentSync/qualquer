#pragma once

/**
 * @file renderer.h
 * @brief Renderer: single-frame render content (CUDA submit + Vulkan recording).
 */

#include <vulkan/vulkan.h>

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
     * Application keeps only the frame-loop timing skeleton. Owns no handles it
     * does not create; the frame counter driving temporal animation is the sole
     * owned state.
     */
    class Renderer {
    public:
        /**
         * @brief Launches the test kernel and signals the frame's external semaphore.
         *
         * Both run on the CUDA context's explicit stream, in submission order, so the
         * signal is posted after the kernel completes. Intended to be called before
         * acquiring the swapchain image so the CUDA engine starts computing while
         * the CPU waits on acquire.
         * @param cuda_context CUDA context (surface, stream, external semaphores).
         * @param width        Display buffer width in pixels.
         * @param height       Display buffer height in pixels.
         * @param frame_index  Current frame-in-flight slot, indexing external_semaphores.
         */
        void submit_cuda(const optix::Context &cuda_context, uint32_t width, uint32_t height, uint32_t frame_index);

        /**
         * @brief Records the Vulkan command sequence (blit, ImGui, layout transitions).
         * @param input Per-frame handles (see RenderInput). cmd must be begun by caller.
         */
        void record_vulkan(const RenderInput &input);

    private:
        /** @brief Monotonic frame counter driving temporal animation. */
        uint32_t frame_counter_ = 0;
    };
} // namespace qualquer::renderer
