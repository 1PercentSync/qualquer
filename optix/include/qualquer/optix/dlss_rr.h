#pragma once

/**
 * @file dlss_rr.h
 * @brief DLSS Ray Reconstruction wrapper: NGX lifecycle, feature management,
 *        and resolution resolver.
 */

#include <cstdint>
#include <map>

#include <cuda_runtime.h>

struct NVSDK_NGX_Parameter;
struct NVSDK_NGX_Handle;

namespace qualquer::optix {
    /**
     * @brief DLSS-RR quality mode (RR-supported subset only).
     *
     * Values match NVSDK_NGX_PerfQuality_Value for direct static_cast.
     * UltraQuality (4) is absent because DLSS Ray Reconstruction does
     * not support it; the gap is intentional.
     *
     * Render resolution is controlled by the user via UI slider; the
     * quality mode is auto-selected based on the actual upscale ratio.
     */
    enum class DlssQualityMode : uint32_t {
        MaxPerf = 0,
        Balanced = 1,
        MaxQuality = 2,
        UltraPerformance = 3,
        Dlaa = 5,
    };

    /**
     * @brief DLSS-RR render preset, selects the underlying neural network model.
     *
     * Values match NVSDK_NGX_RayReconstruction_Hint_Render_Preset.
     * Default lets NGX pick whatever model is current for the SDK build.
     */
    enum class DlssRenderPreset : uint32_t {
        Default = 0,
        D = 4,
        E = 5,
    };

    /**
     * @brief Optimal render resolution for one quality mode.
     */
    struct DlssOptimalSettings {
        uint32_t optimal_width = 0;
        uint32_t optimal_height = 0;
        uint32_t min_width = 0;
        uint32_t min_height = 0;
        uint32_t max_width = 0;
        uint32_t max_height = 0;
    };

    /**
     * @brief Result of DlssResolutionResolver::resolve: clamped height + selected mode.
     */
    struct DlssResolvedHeight {
        uint32_t render_height;
        DlssQualityMode mode;
    };

    /**
     * @brief Per-frame inputs for DLSS-RR evaluation.
     *
     * All texture objects point to render-resolution CUDA arrays (read via
     * CUtexObject* by NGX). The output surface is display-resolution.
     * Jitter is the raw [0,1) pixel offset from Sobol; evaluate() negates
     * and centers it to produce the de-jitter offset NGX expects.
     */
    struct DlssEvalInput {
        // ---- Color I/O ----
        cudaTextureObject_t color_tex; ///< Noisy HDR input (current frame).
        cudaSurfaceObject_t output_surf; ///< Denoised+upscaled HDR output.

        // ---- Aux G-buffer (render resolution) ----
        cudaTextureObject_t depth_tex;
        cudaTextureObject_t motion_vectors_tex;
        cudaTextureObject_t diffuse_albedo_tex;
        cudaTextureObject_t specular_albedo_tex;
        cudaTextureObject_t normal_roughness_tex; ///< RGBA16F: normal xyz + roughness w.

        // ---- Resolution ----
        uint32_t render_width;
        uint32_t render_height;

        // ---- Jitter (raw Sobol [0,1) pixel offset, NOT de-jitter) ----
        float jitter_x;
        float jitter_y;

        // ---- Matrices (GLM column-major, evaluate() transposes to row-major) ----
        const float *view_matrix; ///< glm::value_ptr(view), 16 floats.
        const float *projection_matrix; ///< glm::value_ptr(projection), 16 floats.

        // ---- Per-frame state ----
        bool reset; ///< Scene change / camera teleport.
        float frame_time_ms; ///< Delta time in milliseconds.
    };

    // -----------------------------------------------------------------------
    // DlssResolutionResolver — optimal settings cache + render height resolve
    // -----------------------------------------------------------------------

    /**
     * @brief Queries and caches DLSS-RR optimal render resolutions, then
     *        resolves user-requested heights to valid (clamped, mode-tagged)
     *        values.
     *
     * Stateless except for the cached optimal settings. The NGX
     * parameter interface is passed in at cache time, not stored.
     */
    class DlssResolutionResolver {
    public:
        /**
         * @brief Queries optimal render resolutions for all quality modes and caches results.
         *
         * Call once at startup and again when display resolution changes (window
         * resize). The cached results drive quality-mode auto-selection in the
         * UI layer.
         *
         * @param ngx_params   NGX parameter interface (from DlssRR::ngx_params()).
         * @param display_width  Target output width.
         * @param display_height Target output height.
         * @return true if all modes queried successfully, false otherwise.
         */
        bool cache(NVSDK_NGX_Parameter *ngx_params, uint32_t display_width, uint32_t display_height);

        /**
         * @brief Resolves a user-requested render height to a valid value and quality mode.
         *
         * Picks the mode whose optimal height is closest to the request, then
         * clamps to that mode's [min, max]. If the request falls outside the
         * selected mode's range, compares adjacent modes and picks the one
         * with the smaller clamp distance. If the request exceeds all modes,
         * clamps to the nearest bound. render >= display returns DLAA at
         * display height.
         *
         * Valid after a successful cache() call.
         *
         * @param requested_height User-requested render height from UI slider.
         * @param display_height   Current display (output) height.
         * @return Clamped render height and the selected quality mode.
         */
        [[nodiscard]] DlssResolvedHeight resolve(uint32_t requested_height, uint32_t display_height) const;

    private:
        std::map<DlssQualityMode, DlssOptimalSettings> optimal_settings_;
    };

    // -----------------------------------------------------------------------
    // DlssRR — NGX lifecycle + feature create / evaluate / release
    // -----------------------------------------------------------------------

    /**
     * @brief DLSS Ray Reconstruction wrapper.
     *
     * Manages the NGX SDK lifecycle (init / shutdown) and DLSS-RR feature
     * lifecycle (create / release / evaluate). The CUDA API variant is used
     * exclusively; no Vulkan dependency.
     *
     * Resolution management (optimal settings query, render height resolve)
     * lives in DlssResolutionResolver, which is independent of this class.
     *
     * Ownership: Renderer holds a single instance. NGX init/shutdown bracket
     * the renderer lifetime. Feature create/release/evaluate are driven from
     * Renderer::submit_cuda on enable, resolution, and preset changes.
     */
    class DlssRR {
    public:
        // ---- Lifecycle ----

        /**
         * @brief Initializes the NGX SDK and queries DLSS-RR availability.
         *
         * Calls NVSDK_NGX_CUDA_Init_with_ProjectID, then
         * NVSDK_NGX_CUDA_GetCapabilityParameters to verify driver support.
         * Sets available() to true on success, false on unsupported hardware
         * or outdated driver (logs a warning, does not abort).
         *
         * @param cuda_device CUDA device index (from Context device selection).
         */
        void init(int cuda_device);

        /**
         * @brief Releases all resources and shuts down the NGX SDK.
         *
         * Safe to call if init() was never called or failed.
         */
        void destroy();

        // ---- Feature management ----

        /**
         * @brief Creates the DLSS-RR feature for the given resolution pair.
         *
         * Releases any existing feature first. Uses the default CUDA stream
         * for NGX execution.
         *
         * @param render_width   Input (render) resolution width.
         * @param render_height  Input (render) resolution height.
         * @param display_width  Output (display) resolution width.
         * @param display_height Output (display) resolution height.
         * @param preset         Render preset (neural network model selection).
         * @param quality_mode   Quality mode (from DlssResolutionResolver::resolve).
         */
        void create_feature(uint32_t render_width,
                            uint32_t render_height,
                            uint32_t display_width,
                            uint32_t display_height,
                            DlssRenderPreset preset,
                            DlssQualityMode quality_mode);

        /**
         * @brief Releases the current DLSS-RR feature, freeing its VRAM.
         *
         * Called internally by create_feature (before recreating), destroy,
         * and externally when the user disables DLSS (immediate VRAM release).
         * Idempotent: no-op when no feature is active.
         */
        void release_feature();

        // ---- Per-frame ----

        /**
         * @brief Evaluates DLSS-RR for one frame on the feature's stream.
         *
         * Fills NVSDK_NGX_CUDA_DLSSD_Eval_Params and calls
         * NGX_CUDA_EVALUATE_DLSSD_EXT. The feature must be active
         * (feature_active() == true). The call is enqueued on the stream
         * passed at create_feature time.
         *
         * @param input Per-frame evaluation inputs.
         */
        void evaluate(const DlssEvalInput &input);

        // ---- Queries ----

        /** @brief Whether DLSS-RR is available on this system (valid after init). */
        [[nodiscard]] bool available() const {
            return available_;
        }

        /** @brief Whether a DLSS-RR feature is currently created. */
        [[nodiscard]] bool feature_active() const {
            return ngx_handle_ != nullptr;
        }

        /**
         * @brief Returns VRAM allocated by the active DLSS-RR feature.
         *
         * Cached at feature creation time; no per-frame NGX query.
         * @return Bytes allocated, or 0 if no feature is active.
         */
        [[nodiscard]] uint64_t vram_allocated_bytes() const {
            return cached_vram_bytes_;
        }

        /**
         * @brief NGX parameter interface for DlssResolutionResolver::cache().
         *
         * Null before init or after destroy.
         */
        [[nodiscard]] NVSDK_NGX_Parameter *ngx_params() const {
            return ngx_params_;
        }

    private:
        bool available_ = false;
        bool initialized_ = false;
        NVSDK_NGX_Parameter *ngx_params_ = nullptr;
        NVSDK_NGX_Handle *ngx_handle_ = nullptr;
        uint64_t cached_vram_bytes_ = 0;
    };
} // namespace qualquer::optix
