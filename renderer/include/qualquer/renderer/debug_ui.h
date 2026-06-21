#pragma once

/**
 * @file debug_ui.h
 * @brief Debug UI panel module (renderer layer).
 */

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <qualquer/vulkan/context.h>
#include <qualquer/vulkan/swapchain.h>

namespace qualquer::renderer {
    /**
     * @brief Data passed into DebugUI::draw each frame.
     *
     * DebugUI receives everything it needs through this struct rather than holding
     * references to subsystems, keeping it stateless except for FrameStats
     * accumulation. Fields split into two kinds:
     *  - display-only values (const refs / values): GPU name, resolution, errors.
     *  - mutable refs: interactive controls write back directly (e.g. present mode).
     *
     * Side effects that require Application orchestration (swapchain recreate, log
     * level change) are NOT applied here; they are reported back via DebugUIActions
     * so Application controls the sequencing.
     */
    struct DebugUIContext {
        /** @brief Frame delta time in seconds (typically ImGui::GetIO().DeltaTime). */
        float delta_time = 0.0f;

        /** @brief Vulkan context for GPU name and (optional) VRAM queries. */
        const vulkan::Context &context;

        /** @brief Swapchain for resolution display and supported present-mode lookup. */
        const vulkan::Swapchain &swapchain;

        /**
         * @brief User-facing mirror of the effective present mode.
         *
         * The combo box reads it to mark the current selection and writes back when
         * the user picks another mode. Application reads DebugUIActions::present_mode_changed
         * and feeds this value into Swapchain::recreate, which reflects the
         * actually-effective mode into Swapchain::present_mode.
         *
         * After every Swapchain::recreate, Application copies Swapchain::present_mode —
         * the effective value, possibly fallen back to FIFO when the requested mode is
         * unsupported — back into this field. Without this mirror-back, the combo box
         * would keep showing the rejected mode while the swapchain runs the fallback,
         * and every subsequent resize-driven recreate would re-request the rejected
         * mode. Mirroring keeps the displayed selection honest and makes the next
         * recreate request the effective mode.
         */
        vulkan::PresentMode &user_present_mode;

        /** @brief Error message to show in the banner (empty = no error). */
        const std::string &error_message;
    };

    /**
     * @brief User actions surfaced from the debug panel during a frame.
     *
     * Application inspects these after each draw() to apply side effects that
     * DebugUI must not trigger directly. All fields default to "no action".
     */
    struct DebugUIActions {
        /** @brief True if the user changed the present-mode combo box. */
        bool present_mode_changed = false;

        /** @brief True if the log-level combo box was changed. */
        bool log_level_changed = false;

        /**
         * @brief New spdlog level enum value (valid only when log_level_changed).
         *
         * Note: spdlog::set_level is applied inside draw() directly (it is a
         * process-global toggle with no ordering concerns); this flag is kept so
         * Application can react if needed. Stored as int to avoid pulling spdlog
         * headers into this file.
         */
        int new_log_level = 0;

        /** @brief True if the user clicked the error-banner dismiss button. */
        bool error_dismissed = false;
    };

    /**
     * @brief ImGui debug panel: frame statistics, GPU info, and runtime controls.
     *
     * Stateless except for the FrameStats accumulator. All external data flows in
     * via DebugUIContext; side effects flow out via DebugUIActions.
     *
     * Lifetime is trivial (no GPU resources); construction/destruction need no
     * explicit init/destroy. Call draw() once per frame between
     * ImGuiBackend::begin_frame() and ImGuiBackend::render().
     */
    class DebugUI {
    public:
        /**
         * @brief Draws the debug panel and returns user-triggered actions.
         *
         * Updates frame-time statistics from ctx.delta_time, renders the panel
         * (FPS, 1% Low, GPU, resolution, VRAM, present-mode combo, log-level
         * combo, error banner), and collects any actions the user triggered.
         *
         * @param ctx Per-frame data needed by the panel.
         * @return Actions triggered by the user this frame.
         */
        DebugUIActions draw(DebugUIContext &ctx);

    private:
        /**
         * @brief Periodically computes frame-time statistics from sampled deltas.
         *
         * Accumulates per-frame delta times and every kUpdateInterval seconds
         * computes average FPS, average frame time, and 1% Low. Between updates
         * the displayed values stay stable (no flickering on every frame).
         */
        struct FrameStats {
            /** @brief Average FPS over the last window. */
            float avg_fps = 0.0f;

            /** @brief Average frame time over the last window, in milliseconds. */
            float avg_frame_time_ms = 0.0f;

            /** @brief 1% Low FPS (reciprocal of the worst-1% average frame time). */
            float low1_fps = 0.0f;

            /** @brief Average of the worst 1% of frame times, in milliseconds. */
            float low1_frame_time_ms = 0.0f;

            /**
             * @brief Feeds one frame's delta time in seconds.
             * @param delta_time Seconds elapsed since the previous frame.
             */
            void push(float delta_time);

        private:
            /** @brief Update interval: statistics recompute once per this many seconds. */
            static constexpr float kUpdateInterval = 1.0f;

            /** @brief Delta-time samples accumulated within the current window. */
            std::vector<float> samples_;

            /** @brief Elapsed time within the current window. */
            float elapsed_ = 0.0f;

            /** @brief Recomputes avg/1%-low stats from samples_ and clears the window. */
            void compute();
        };

        /** @brief Frame-time statistics accumulator (persists across frames). */
        FrameStats frame_stats_;
    };
} // namespace qualquer::renderer
