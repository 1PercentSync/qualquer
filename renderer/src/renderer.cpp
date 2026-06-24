/**
 * @file renderer.cpp
 * @brief Renderer implementation.
 */

#include <qualquer/renderer/renderer.h>

namespace qualquer::renderer {
    void Renderer::render_frame(const RenderInput &input) {
        submit_cuda(input);
        record_vulkan(input);
        ++frame_counter_;
    }

    void Renderer::submit_cuda(const RenderInput &input) {
        (void) input;
    }

    void Renderer::record_vulkan(const RenderInput &input) {
        (void) input;
    }
} // namespace qualquer::renderer
