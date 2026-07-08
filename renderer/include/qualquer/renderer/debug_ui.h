#pragma once

/**
 * @file debug_ui.h
 * @brief Debug UI panel module (renderer layer).
 */

#include <string>
#include <vector>

#include <qualquer/renderer/camera.h>
#include <qualquer/renderer/render_settings.h>
#include <qualquer/renderer/scene_stats.h>
#include <qualquer/vulkan/context.h>
#include <qualquer/vulkan/swapchain.h>

namespace qualquer::renderer {
    /**
     * @brief Data passed into DebugUI::draw each frame.
     *
     * DebugUI receives everything it needs through this struct rather than holding
     * references to subsystems, keeping it stateless except for FrameStats
     * accumulation. context/error_message are read-only; swapchain is mutable because
     * the present-mode combo writes the user's selection back into swapchain.present_mode
     * (the single source of truth, which Application then recreates from).
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

        /**
         * @brief Swapchain providing resolution, supported present modes, and the
         *        present-mode field the combo box reads/writes.
         *
         * Mutable because the present-mode combo writes the new selection back into
         * swapchain.present_mode; Application observes present_mode_changed and
         * recreates the swapchain, which reflects the effective mode (possibly a FIFO
         * fallback) back into present_mode.
         */
        vulkan::Swapchain &swapchain;

        /** @brief Error message to show in the banner (empty = no error). */
        const std::string &error_message;

        /**
         * @brief Currently loaded scene file path (empty = no scene).
         *
         * Display-only: the Scene section shows the filename and a tooltip with
         * the full path. Loading is triggered via a native file dialog and
         * surfaced through DebugUIActions::scene_load_requested.
         */
        const std::string &scene_path;

        /**
         * @brief Currently loaded HDR environment map path (empty = none).
         *
         * Display-only: the Env Map section shows the filename and a tooltip
         * with the full path. Loading is triggered via a native file dialog and
         * surfaced through DebugUIActions::env_map_load_requested.
         */
        const std::string &env_map_path;

        // ---- Step 8: PT parameters ----

        /**
         * @brief Runtime render settings (mutable for slider write-back).
         *
         * Sliders modify fields directly; the renderer detects changes and
         * resets accumulation automatically.
         */
        RenderSettings &settings;

        /** @brief Camera state (mutable for FOV slider write-back). */
        Camera &camera;

        /** @brief Number of samples accumulated in the displayed buffer. */
        uint32_t accumulated_samples = 0;

        /** @brief Scene asset statistics (read-only snapshot). */
        const SceneStats &scene_stats;
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

        /** @brief True if the user picked a scene file via the Load dialog. */
        bool scene_load_requested = false;

        /** @brief Scene path picked by the user (valid only when scene_load_requested). */
        std::string new_scene_path;

        /** @brief True if the user picked an HDR env map via the Load dialog. */
        bool env_map_load_requested = false;

        /** @brief Env map path picked by the user (valid only when env_map_load_requested). */
        std::string new_env_map_path;

        /** @brief True if the user clicked the manual accumulation reset button. */
        bool accum_reset_requested = false;
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
        DebugUIActions draw(const DebugUIContext &ctx);

    private:
        /**
         * @brief Periodically computes frame-time statistics from sampled deltas.
         *
         * Accumulates per-frame delta times and every kUpdateInterval seconds
         * computes average FPS, average frame time, and 1% Low. Between updates
         * the displayed values stay stable (no flickering on every frame).
         *
         * Defined before the section helpers so its name is complete when
         * draw_frame_stats references it.
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

            /** @brief Recomputes avg/1%-low stats from samples_ (caller clears the window). */
            void compute();
        };

        /** @brief Frame-time statistics accumulator (persists across frames). */
        FrameStats frame_stats_;

        /**
         * @brief Renders the FPS and 1% Low lines.
         *
         * @param stats Current frame-time statistics to display.
         */
        static void draw_frame_stats(const FrameStats &stats);

        /**
         * @brief Renders the GPU name, resolution, and VRAM usage lines.
         *
         * VRAM line falls back to "VRAM: N/A" when query_vram_usage() returns
         * nullopt (VK_EXT_memory_budget unsupported).
         *
         * @param ctx Vulkan context (gpu name, VRAM query) and swapchain (extent).
         */
        static void draw_gpu_info(const DebugUIContext &ctx);

        /**
         * @brief Renders the present-mode combo box.
         *
         * Options are the entries of ctx.swapchain.supported_modes that the renderer
         * surfaces (FIFO/MAILBOX/IMMEDIATE); labels come from vulkan::to_label. On
         * user selection, writes the chosen mode back into ctx.swapchain.present_mode
         * and sets actions.present_mode_changed; Application recreates the swapchain,
         * which reflects the effective mode back. Disables the combo when only FIFO
         * is available.
         *
         * @param ctx    Provides supported_modes and the present_mode field.
         * @param action Receives present_mode_changed on user interaction.
         */
        static void draw_present_mode(const DebugUIContext &ctx, DebugUIActions &action);

        /**
         * @brief Renders the dismissable error banner.
         *
         * No-op when ctx.error_message is empty. Otherwise shows the message (red)
         * and an "X" button that sets actions.error_dismissed.
         *
         * @param ctx    Provides the error message.
         * @param action Receives error_dismissed when the user clicks X.
         */
        static void draw_error_banner(const DebugUIContext &ctx, DebugUIActions &action);

        /**
         * @brief Renders the log-level combo box.
         *
         * Applies spdlog::set_level immediately on change and sets
         * actions.log_level_changed / new_log_level.
         *
         * @param action Receives log_level_changed and new_log_level on change.
         */
        static void draw_log_level(DebugUIActions &action);

        /**
         * @brief Renders the Path Tracing section: parameter sliders and sample count.
         *
         * @param ctx    Provides settings (mutable), camera (mutable for FOV),
         *               and accumulated_samples.
         * @param action Receives accum_reset_requested on Reset button click.
         * @param stats  Frame-time statistics (avg_fps for samples/s display).
         */
        static void draw_path_tracing(const DebugUIContext &ctx, DebugUIActions &action,
                                      const FrameStats &stats);

        /**
         * @brief Renders camera position, orientation, and scene asset statistics.
         *
         * @param ctx Provides camera (read-only pos/yaw/pitch) and scene_stats.
         */
        static void draw_scene_info(const DebugUIContext &ctx);

        /**
         * @brief Renders the Scene section: current file name and a Load button.
         *
         * Shows "No scene loaded" when ctx.scene_path is empty, otherwise the
         * filename with a full-path tooltip. The Load button opens a native file
         * dialog (glTF/GlB); on a non-empty selection sets
         * action.scene_load_requested and action.new_scene_path.
         *
         * @param ctx    Provides the current scene path.
         * @param action Receives the load request and the picked path.
         */
        static void draw_scene(const DebugUIContext &ctx, DebugUIActions &action);

        /**
         * @brief Renders the Env Map section: current file name and a Load button.
         *
         * Shows "No env map loaded" when ctx.env_map_path is empty, otherwise
         * the filename with a full-path tooltip. The Load button opens a native
         * file dialog (HDR); on a non-empty selection sets
         * action.env_map_load_requested and action.new_env_map_path.
         *
         * @param ctx    Provides the current env map path.
         * @param action Receives the load request and the picked path.
         */
        static void draw_env_map(const DebugUIContext &ctx, DebugUIActions &action);
    };
} // namespace qualquer::renderer
