/**
 * @file dlss_rr.cpp
 * @brief DLSS Ray Reconstruction wrapper implementation.
 */

#include <qualquer/optix/dlss_rr.h>

#include <cuda.h>

#include <nvsdk_ngx.h>
#include <nvsdk_ngx_helpers_dlssd_cuda.h>

#include <spdlog/spdlog.h>

#include <filesystem>
#include <windows.h>

namespace qualquer::optix {

    /** @brief NGX project identifier (UUID format required by NGX). */
    constexpr char kNgxProjectId[] = "d8b2224f-2576-4814-92ec-53596756923e";

    /** @brief Quality mode names for logging, indexed by DlssQualityMode. */
    constexpr const char *kQualityModeNames[] = {
        "MaxPerf", "Balanced", "MaxQuality",
        "UltraPerf", "UltraQuality", "DLAA",
    };

    // NGX result string for logging. Covers the common error codes;
    // unknown codes fall back to the raw integer.
    static std::string ngx_result_string(NVSDK_NGX_Result result) {
        switch (result) {
            case NVSDK_NGX_Result_Success:                       return "Success";
            case NVSDK_NGX_Result_FAIL_FeatureNotSupported:      return "FeatureNotSupported";
            case NVSDK_NGX_Result_FAIL_PlatformError:            return "PlatformError";
            case NVSDK_NGX_Result_FAIL_FeatureAlreadyExists:     return "FeatureAlreadyExists";
            case NVSDK_NGX_Result_FAIL_FeatureNotFound:          return "FeatureNotFound";
            case NVSDK_NGX_Result_FAIL_InvalidParameter:         return "InvalidParameter";
            case NVSDK_NGX_Result_FAIL_ScratchBufferTooSmall:    return "ScratchBufferTooSmall";
            case NVSDK_NGX_Result_FAIL_NotInitialized:           return "NotInitialized";
            case NVSDK_NGX_Result_FAIL_UnsupportedInputFormat:   return "UnsupportedInputFormat";
            case NVSDK_NGX_Result_FAIL_RWFlagMissing:            return "RWFlagMissing";
            case NVSDK_NGX_Result_FAIL_MissingInput:             return "MissingInput";
            case NVSDK_NGX_Result_FAIL_UnableToInitializeFeature:return "UnableToInitializeFeature";
            case NVSDK_NGX_Result_FAIL_OutOfDate:                return "OutOfDate";
            case NVSDK_NGX_Result_FAIL_OutOfGPUMemory:           return "OutOfGPUMemory";
            case NVSDK_NGX_Result_FAIL_UnsupportedFormat:        return "UnsupportedFormat";
            case NVSDK_NGX_Result_FAIL_UnableToWriteToAppDataPath: return "UnableToWriteToAppDataPath";
            case NVSDK_NGX_Result_FAIL_UnsupportedParameter:     return "UnsupportedParameter";
            case NVSDK_NGX_Result_FAIL_Denied:                   return "Denied";
            case NVSDK_NGX_Result_FAIL_NotImplemented:           return "NotImplemented";
            default: return "Unknown(" + std::to_string(result) + ")";
        }
    }

    // NGX check macro: logs and returns false on failure (non-fatal path).
    #define NGX_CHECK_WARN(call)                                                    \
        do {                                                                        \
            NVSDK_NGX_Result ngx_result_ = (call);                                  \
            if (NVSDK_NGX_FAILED(ngx_result_)) {                                    \
                spdlog::warn("NGX call failed: {} returned {} at {}:{}",            \
                             #call, ngx_result_string(ngx_result_),                 \
                             __FILE__, __LINE__);                                    \
                return false;                                                       \
            }                                                                       \
        } while (0)

    // NGX check macro: logs and aborts on failure (fatal path).
    #define NGX_CHECK(call)                                                         \
        do {                                                                        \
            NVSDK_NGX_Result ngx_result_ = (call);                                  \
            if (NVSDK_NGX_FAILED(ngx_result_)) {                                    \
                spdlog::critical("NGX call failed: {} returned {} at {}:{}",        \
                                 #call, ngx_result_string(ngx_result_),             \
                                 __FILE__, __LINE__);                               \
                spdlog::default_logger()->flush();                                  \
                std::abort();                                                       \
            }                                                                       \
        } while (0)

    /**
     * @brief NGX logging callback bridged to spdlog.
     *
     * NGX calls this from any thread; spdlog is thread-safe.
     * ON-level messages map to spdlog::info, VERBOSE to spdlog::debug.
     */
    static void NVSDK_CONV ngx_log_callback(
        const char *message,
        NVSDK_NGX_Logging_Level logging_level,
        NVSDK_NGX_Feature /*source_component*/) {
        if (logging_level == NVSDK_NGX_LOGGING_LEVEL_VERBOSE) {
            spdlog::debug("[NGX] {}", message);
        } else {
            spdlog::info("[NGX] {}", message);
        }
    }

    // Executable directory for NGX data path (logs, temp files).
    // NGX also searches for nvngx_dlssd.dll relative to this path.
    static std::wstring get_executable_directory() {
        char path[2048] = {};
        if (GetModuleFileNameA(nullptr, path, sizeof(path)) == 0) {
            return L".";
        }
        return std::filesystem::path(path).parent_path().wstring();
    }

    void DlssRR::init(int cuda_device) {
        if (initialized_) {
            return;
        }

        const std::wstring app_path = get_executable_directory();

        // Route NGX internal logs through spdlog and suppress NGX's own
        // file-based logging sinks.
        const NVSDK_NGX_FeatureCommonInfo feature_info{
            .PathListInfo = {},
            .InternalData = nullptr,
            .LoggingInfo = {
                .LoggingCallback = ngx_log_callback,
                .MinimumLoggingLevel = NVSDK_NGX_LOGGING_LEVEL_ON,
                .DisableOtherLoggingSinks = true,
            },
        };

        // Init NGX with project ID (no NVIDIA-issued app ID required).
        NVSDK_NGX_Result result = NVSDK_NGX_CUDA_Init_with_ProjectID(
            kNgxProjectId,
            NVSDK_NGX_ENGINE_TYPE_CUSTOM,
            "1.0.0",
            app_path.c_str(),
            &feature_info
        );

        if (NVSDK_NGX_FAILED(result)) {
            spdlog::warn("DLSS-RR: NGX init failed: {}", ngx_result_string(result));
            available_ = false;
            return;
        }

        initialized_ = true;

        // Check RayReconstruction hardware/driver support.
        constexpr NVSDK_NGX_ProjectIdDescription project_desc{kNgxProjectId,
                                                           NVSDK_NGX_ENGINE_TYPE_CUSTOM, "1.0.0"};
        constexpr NVSDK_NGX_Application_Identifier app_id{
            NVSDK_NGX_Application_Identifier_Type_Project_Id, {.ProjectDesc = project_desc}};
        const NVSDK_NGX_FeatureDiscoveryInfo discovery_info{
            NVSDK_NGX_Version_API,
            NVSDK_NGX_Feature_RayReconstruction,
            app_id,
            app_path.c_str(),
            nullptr};
        NVSDK_NGX_FeatureRequirement requirement{};
        result = NVSDK_NGX_CUDA_GetFeatureRequirements(cuda_device, &discovery_info, &requirement);

        if (NVSDK_NGX_FAILED(result)
            || requirement.FeatureSupported != NVSDK_NGX_FeatureSupportResult_Supported) {
            spdlog::warn("DLSS-RR: RayReconstruction not supported on this system");
            available_ = false;
            return;
        }

        // Obtain capability parameters (shared across feature create / evaluate / optimal settings).
        result = NVSDK_NGX_CUDA_GetCapabilityParameters(&ngx_params_);
        if (NVSDK_NGX_FAILED(result)) {
            spdlog::warn("DLSS-RR: GetCapabilityParameters failed: {}", ngx_result_string(result));
            available_ = false;
            return;
        }

        available_ = true;
        spdlog::info("DLSS-RR: initialized successfully");
    }

    void DlssRR::destroy() {
        release_feature();

        if (ngx_params_) {
            NVSDK_NGX_CUDA_DestroyParameters(ngx_params_);
            ngx_params_ = nullptr;
        }

        if (initialized_) {
            NVSDK_NGX_CUDA_Shutdown();
            initialized_ = false;
        }

        available_ = false;
    }

    void DlssRR::create_feature(uint32_t render_width, uint32_t render_height,
                                uint32_t display_width, uint32_t display_height,
                                // ReSharper disable once CppParameterMayBeConst
                                DlssRenderPreset preset, cudaStream_t display_stream) {
        release_feature();

        if (!available_ || !ngx_params_) {
            spdlog::error("DLSS-RR: cannot create feature, not available");
            return;
        }

        // The caller is responsible for resolving the render height via
        // resolve_render_height() and passing the clamped dimensions here.
        // This ensures buffers, raygen launch, and NGX all use the same size.
        const auto resolved = resolve_render_height(render_height, display_height);
        const DlssQualityMode quality = resolved.mode;

        // Feature creation flags.
        constexpr int create_flags = NVSDK_NGX_DLSS_Feature_Flags_MVLowRes
                                     | NVSDK_NGX_DLSS_Feature_Flags_IsHDR;

        NVSDK_NGX_DLSSD_Create_Params dlss_params{};
        dlss_params.InDenoiseMode = NVSDK_NGX_DLSS_Denoise_Mode_DLUnified;
        dlss_params.InWidth = render_width;
        dlss_params.InHeight = render_height;
        dlss_params.InTargetWidth = display_width;
        dlss_params.InTargetHeight = display_height;
        dlss_params.InPerfQualityValue = static_cast<NVSDK_NGX_PerfQuality_Value>(quality);
        dlss_params.InFeatureCreateFlags = create_flags;
        dlss_params.InUseHWDepth = NVSDK_NGX_DLSS_Depth_Type_Linear;
        dlss_params.InRoughnessMode = NVSDK_NGX_DLSS_Roughness_Mode_Unpacked;

        // Render preset for all quality modes.
        const auto ngx_preset = static_cast<NVSDK_NGX_RayReconstruction_Hint_Render_Preset>(
            static_cast<uint32_t>(preset));
        ngx_params_->Set(NVSDK_NGX_Parameter_RayReconstruction_Hint_Render_Preset_DLAA, ngx_preset);
        ngx_params_->Set(NVSDK_NGX_Parameter_RayReconstruction_Hint_Render_Preset_Quality, ngx_preset);
        ngx_params_->Set(NVSDK_NGX_Parameter_RayReconstruction_Hint_Render_Preset_Balanced, ngx_preset);
        ngx_params_->Set(NVSDK_NGX_Parameter_RayReconstruction_Hint_Render_Preset_Performance, ngx_preset);
        ngx_params_->Set(NVSDK_NGX_Parameter_RayReconstruction_Hint_Render_Preset_UltraPerformance, ngx_preset);

        // Release VRAM immediately on feature release (no lazy deallocation).
        ngx_params_->Set(NVSDK_NGX_Parameter_FreeMemOnReleaseFeature, 1);

        // CUDA context and stream for NGX execution.
        CUcontext cu_context = nullptr;
        cuCtxGetCurrent(&cu_context);

        NVSDK_NGX_CUDA_DLSSD_Create_Params cuda_params{};
        cuda_params.Feature = dlss_params;
        cuda_params.InCUContext = static_cast<void *>(cu_context);
        cuda_params.InCUStream = static_cast<void *>(display_stream);

        NGX_CHECK(NGX_CUDA_CREATE_DLSSD_EXT(&ngx_handle_, ngx_params_, &cuda_params));

        spdlog::info("DLSS-RR: feature created (render {}x{} -> display {}x{}, mode {})",
                     render_width, render_height, display_width, display_height,
                     kQualityModeNames[static_cast<uint32_t>(quality)]);
    }

    void DlssRR::evaluate(const EvalInput &input) {
        if (!ngx_handle_ || !ngx_params_) {
            spdlog::error("DLSS-RR: evaluate called without active feature");
            return;
        }

        NVSDK_NGX_CUDA_DLSSD_Eval_Params eval{};

        // ---- Color I/O ----
        // CUDA API: void* fields point to the address of a CUtexObject (input)
        // or CUsurfObject (output). The SDK reads the object handle through
        // the pointer; the pointed-to value must survive the evaluate call.
        eval.pInColor = const_cast<cudaTextureObject_t *>(&input.color_tex);
        eval.pInOutput = const_cast<cudaSurfaceObject_t *>(&input.output_surf);

        // ---- Aux G-buffer ----
        eval.pInDepth = const_cast<cudaTextureObject_t *>(&input.depth_tex);
        eval.pInMotionVectors = const_cast<cudaTextureObject_t *>(&input.motion_vectors_tex);
        eval.pInDiffuseAlbedo = const_cast<cudaTextureObject_t *>(&input.diffuse_albedo_tex);
        eval.pInSpecularAlbedo = const_cast<cudaTextureObject_t *>(&input.specular_albedo_tex);
        eval.pInNormals = const_cast<cudaTextureObject_t *>(&input.normals_tex);
        eval.pInRoughness = const_cast<cudaTextureObject_t *>(&input.roughness_tex);

        // Specular hit distance: not provided (D32). nullptr tells the SDK
        // this signal is absent, which is more correct than feeding all-infinity.
        eval.pInSpecularHitDistance = nullptr;

        // ---- Jitter ----
        // Input is raw [0,1) Sobol pixel offset. NGX wants the de-jitter offset
        // (negated, centered at pixel center). All three NVIDIA reference
        // projects (optix-subd, vk_denoise_dlssrr, vk_gltf_renderer) negate.
        eval.InJitterOffsetX = -(input.jitter_x - 0.5f);
        eval.InJitterOffsetY = -(input.jitter_y - 0.5f);

        // ---- Motion vector scale ----
        // Our MVs are in pixel space: (prev_ndc - curr_ndc) * 0.5 * resolution.
        // GLM perspective produces NDC Y-up, but DLSS screen space is Y-down
        // (SDK §3.6). The NDC-to-pixel Y mapping is pixel_y = (1 - ndc_y) * 0.5
        // * height, so the correct pixel-space MV Y is (curr_ndc.y - prev_ndc.y)
        // * 0.5 * height — opposite sign to our formula. InMVScaleY = -1 lets
        // the SDK flip it without changing the MV buffer generation.
        eval.InMVScaleX = 1.0f;
        eval.InMVScaleY = -1.0f;

        // ---- Render subrect (full frame, no subrect offset) ----
        eval.InRenderSubrectDimensions = {input.render_width, input.render_height};

        // ---- Matrices ----
        // NGX expects row-major float[16]. GLM stores column-major; transpose
        // converts to row-major memory layout (vk_gltf_renderer approach,
        // equivalent to optix-subd's native row-major otk::Matrix4x4).
        // Temporary storage must outlive the evaluate call.
        float view_row_major[16];
        float proj_row_major[16];
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                // GLM column-major: input[col*4 + row] → row-major: out[row*4 + col]
                view_row_major[r * 4 + c] = input.view_matrix[c * 4 + r];
                proj_row_major[r * 4 + c] = input.projection_matrix[c * 4 + r];
            }
        }
        eval.pInWorldToViewMatrix = view_row_major;
        eval.pInViewToClipMatrix = proj_row_major;

        // ---- Debug overlay orientation ----
        // Controls the debug indicator's row rendering order, not DLSS
        // processing. Our CUDA surface is top-down (row 0 = top, Vulkan blit
        // preserves this). optix-subd/Blender set 1 because their GL interop
        // surfaces are bottom-up; we don't need the flip.
        eval.InIndicatorInvertYAxis = 0;

        // ---- Reset ----
        eval.InReset = input.reset ? 1 : 0;

        // ---- Frame time (optional, helps DLSS estimate motion blur) ----
        eval.InFrameTimeDeltaInMsec = input.frame_time_ms;

        // ---- Pre-exposure ----
        // Color is not pre-multiplied by exposure. SDK defaults 0→1.0.
        eval.InPreExposure = 1.0f;

        NGX_CHECK(NGX_CUDA_EVALUATE_DLSSD_EXT(ngx_handle_, ngx_params_, &eval));

        // VRAM allocation stabilizes after the first evaluate. Query once,
        // then stop — the value is constant for this feature's lifetime.
        if (cached_vram_bytes_ == 0) {
            unsigned long long vram = 0;
            if (NGX_DLSSD_GET_STATS(ngx_params_, &vram) == NVSDK_NGX_Result_Success
                && vram > 0) {
                cached_vram_bytes_ = vram;
            }
        }
    }

    void DlssRR::release_feature() {
        if (ngx_handle_) {
            NGX_CHECK(NVSDK_NGX_CUDA_ReleaseFeature(ngx_handle_));
            ngx_handle_ = nullptr;
        }
        cached_vram_bytes_ = 0;
    }

    bool DlssRR::cache_optimal_settings(uint32_t display_width, uint32_t display_height) {
        if (!ngx_params_) {
            return false;
        }

        bool all_ok = true;
        for (uint32_t i = 0; i < kDlssQualityModeCount; ++i) {
            auto &s = optimal_settings_[i];
            float sharpness = 0.0f;
            const NVSDK_NGX_Result result = NGX_DLSSD_GET_OPTIMAL_SETTINGS(
                ngx_params_,
                display_width, display_height,
                static_cast<NVSDK_NGX_PerfQuality_Value>(i),
                &s.optimal_width, &s.optimal_height,
                &s.max_width, &s.max_height,
                &s.min_width, &s.min_height,
                &sharpness
            );
            if (NVSDK_NGX_FAILED(result)) {
                spdlog::warn("DLSS-RR: {} optimal settings query failed: {}",
                             kQualityModeNames[i], ngx_result_string(result));
                s = {};
                all_ok = false;
                continue;
            }
            spdlog::info("DLSS-RR: {} optimal {}x{}, range [{}x{} .. {}x{}]",
                         kQualityModeNames[i],
                         s.optimal_width, s.optimal_height,
                         s.min_width, s.min_height,
                         s.max_width, s.max_height);
        }

        return all_ok;
    }

    DlssRR::ResolvedRenderHeight DlssRR::resolve_render_height(uint32_t requested_height,
                                                                uint32_t display_height) const {
        // render >= display → DLAA (no upscaling).
        if (requested_height >= display_height) {
            return {display_height, DlssQualityMode::Dlaa};
        }

        // Single-pass selection: for each mode, compute the clamped height
        // and how far it is from the request (clamp distance) and from the
        // mode's optimal (optimal distance).  Prefer smallest clamp distance
        // first; among ties (e.g. multiple modes whose [min,max] all contain
        // the request) pick the one with the closest optimal.
        DlssQualityMode best_mode = DlssQualityMode::Dlaa;
        uint32_t best_clamped = display_height;
        uint32_t best_clamp_dist = UINT32_MAX;
        uint32_t best_optimal_dist = UINT32_MAX;

        for (uint32_t i = 0; i < kDlssQualityModeCount; ++i) {
            const auto &s = optimal_settings_[i];
            if (s.optimal_height == 0) {
                continue;
            }

            const uint32_t clamped = std::clamp(requested_height,
                                                s.min_height, s.max_height);
            const uint32_t clamp_dist = clamped > requested_height
                                            ? clamped - requested_height
                                            : requested_height - clamped;
            const uint32_t optimal_dist = requested_height > s.optimal_height
                                              ? requested_height - s.optimal_height
                                              : s.optimal_height - requested_height;

            if (clamp_dist < best_clamp_dist
                || (clamp_dist == best_clamp_dist
                    && optimal_dist < best_optimal_dist)) {
                best_mode = static_cast<DlssQualityMode>(i);
                best_clamped = clamped;
                best_clamp_dist = clamp_dist;
                best_optimal_dist = optimal_dist;
            }
        }

        return {best_clamped, best_mode};
    }

} // namespace qualquer::optix
