/**
 * @file debug_ui.cpp
 * @brief DebugUI implementation: frame-stats computation and ImGui panel drawing.
 */

#include <qualquer/renderer/debug_ui.h>

#include <qualquer/optix/dlss_rr.h>

#include <algorithm>
#include <filesystem>
#include <ranges>
#include <string>

#include <glm/trigonometric.hpp>

#include <imgui.h>
#include <spdlog/spdlog.h>

// WIN32_LEAN_AND_MEAN trims windows.h to the dialog API; NOMINMAX prevents the
// min/max macros from corrupting std::max used in FrameStats::compute.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>

namespace qualquer::renderer {
    namespace {
        /** @brief Log-level names for the combo box, indexed by spdlog::level::level_enum. */
        constexpr const char *kLogLevelNames[] = {
            "Trace", "Debug", "Info", "Warn", "Error", "Critical", "Off",
        };

        /**
         * @brief Opens a native file dialog and returns the selected path.
         * @param filter GetOpenFileNameW double-null-terminated filter string.
         * @param title  Dialog title.
         * @return Selected path (UTF-8), or empty if the user cancelled.
         */
        std::string open_file_dialog(const wchar_t *filter, const wchar_t *title) {
            wchar_t file_path[MAX_PATH] = {};
            OPENFILENAMEW ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFilter = filter;
            ofn.lpstrFile = file_path;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrTitle = title;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

            if (GetOpenFileNameW(&ofn)) {
                return std::filesystem::path(file_path).string();
            }
            return {};
        }

        /**
         * SliderFloat that applies immediately during mouse drag but defers
         * Ctrl+Click text-input changes until Enter / click-away / Tab.
         *
         * The text-input value is persisted in ImGui StateStorage so the
         * deactivation frame can retrieve it even if SliderBehavior does not
         * re-apply the value when the widget becomes inactive.
         */
        bool slider_float_deferred(const char *label,
                                   float *v,
                                   const float v_min,
                                   const float v_max,
                                   const char *format,
                                   const ImGuiSliderFlags flags = 0) {
            const float original = *v;
            ImGui::SliderFloat(label, v, v_min, v_max, format, flags);

            if (ImGui::IsItemActive()) {
                if (ImGui::GetIO().WantTextInput) {
                    // Persist the parsed text value before reverting, so the
                    // confirmation frame can retrieve it from storage.
                    ImGui::GetStateStorage()->SetFloat(ImGui::GetItemID(), *v);
                    *v = original;
                    return false;
                }
                // Drag mode: apply immediately.
                return *v != original;
            }

            // Deactivation frame: if ImGui did not write the confirmed text
            // value into *v (edge case), fall back to the persisted value.
            if (ImGui::IsItemDeactivatedAfterEdit() && *v == original) {
                *v = ImGui::GetStateStorage()->GetFloat(ImGui::GetItemID(), original);
            }

            return *v != original;
        }

        /** SliderAngle variant with the same deferred text-input behaviour. */
        bool slider_angle_deferred(const char *label,
                                   float *v_rad,
                                   const float v_degrees_min,
                                   const float v_degrees_max,
                                   const char *format,
                                   const ImGuiSliderFlags flags = 0) {
            const float original = *v_rad;
            ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max, format, flags);

            if (ImGui::IsItemActive()) {
                if (ImGui::GetIO().WantTextInput) {
                    ImGui::GetStateStorage()->SetFloat(ImGui::GetItemID(), *v_rad);
                    *v_rad = original;
                    return false;
                }
                return *v_rad != original;
            }

            if (ImGui::IsItemDeactivatedAfterEdit() && *v_rad == original) {
                *v_rad = ImGui::GetStateStorage()->GetFloat(ImGui::GetItemID(), original);
            }

            return *v_rad != original;
        }

        /**
         * SliderInt that applies only when the interaction ends (drag release or
         * Ctrl+Click text-input confirm), for parameters whose every change
         * triggers expensive reallocation. Ctrl+Click text input may exceed the
         * slider range; the committed value is clamped to [hard_min, hard_max].
         *
         * The drag value is persisted in ImGui StateStorage across frames.
         * Without this, the local `display` variable re-initialises from *v
         * every frame; on the release frame SliderBehavior does not apply
         * the drag position (widget inactive), so `display` bounces back to
         * the original *v.
         */
        bool slider_uint_on_release(const char *label,
                                    uint32_t *v,
                                    const int v_min,
                                    const int v_max,
                                    const int hard_min,
                                    const int hard_max) {
            int display = static_cast<int>(*v);
            ImGui::SliderInt(label, &display, v_min, v_max);

            const ImGuiID id = ImGui::GetItemID();
            auto *storage = ImGui::GetStateStorage();

            if (ImGui::IsItemActive()) {
                // Persist the drag/text-input value so the release frame
                // can retrieve it.
                storage->SetInt(id, display);
                return false;
            }

            if (!ImGui::IsItemDeactivatedAfterEdit()) {
                return false;
            }

            // Release frame: prefer ImGui's value if it differs from the
            // original (text-input confirmation writes directly into
            // `display`); otherwise retrieve the persisted drag value.
            const int last = (display != static_cast<int>(*v))
                                 ? display
                                 : storage->GetInt(id, display);
            const auto committed = static_cast<uint32_t>(std::clamp(last, hard_min, hard_max));
            if (committed == *v) {
                return false;
            }
            *v = committed;
            return true;
        }
    } // namespace

    // ---- FrameStats ----

    void DebugUI::FrameStats::push(const float delta_time,
                                    [[maybe_unused]] const float pt_ms,
                                    [[maybe_unused]] const float cuda_display_ms,
                                    [[maybe_unused]] const float vk_display_ms,
                                    [[maybe_unused]] const float cpu_frame_ms) {
        samples_.push_back(delta_time);
#ifndef NDEBUG
        pt_samples_.push_back(pt_ms);
        cuda_display_samples_.push_back(cuda_display_ms);
        vk_display_samples_.push_back(vk_display_ms);
        cpu_frame_samples_.push_back(cpu_frame_ms);
#endif
        elapsed_ += delta_time;

        // Recompute once per kUpdateInterval and freeze the display between updates
        // so numbers don't flicker every frame.
        if (elapsed_ >= kUpdateInterval) {
            compute();
            samples_.clear();
#ifndef NDEBUG
            pt_samples_.clear();
            cuda_display_samples_.clear();
            vk_display_samples_.clear();
            cpu_frame_samples_.clear();
#endif
            elapsed_ = 0.0f;
        }
    }

    void DebugUI::FrameStats::compute() {
        const size_t n = samples_.size();
        if (n == 0) {
            return;
        }

        float total = 0.0f;
        for (const float s: samples_) {
            total += s;
        }

        avg_frame_time_ms = (total / static_cast<float>(n)) * 1000.0f;
        avg_fps = static_cast<float>(n) / total;

        // 1% Low = mean of the worst 1% frame times (the longest, after descending sort).
        std::ranges::sort(samples_, std::greater<>());
        const size_t low_count = std::max<size_t>(1, n / 100);

        float low_total = 0.0f;
        for (size_t i = 0; i < low_count; ++i) {
            low_total += samples_[i];
        }
        low1_frame_time_ms = (low_total / static_cast<float>(low_count)) * 1000.0f;
        low1_fps = 1000.0f / low1_frame_time_ms;

#ifndef NDEBUG
        const auto avg = [&](const std::vector<float> &v) {
            float sum = 0.0f;
            for (const float s : v) { sum += s; }
            return sum / static_cast<float>(n);
        };
        avg_pt_ms = avg(pt_samples_);
        avg_cuda_display_ms = avg(cuda_display_samples_);
        avg_vk_display_ms = avg(vk_display_samples_);
        avg_total_display_ms = avg_cuda_display_ms + avg_vk_display_ms;
        avg_cpu_frame_ms = avg(cpu_frame_samples_);
#endif
    }

    // ---- DebugUI ----

    DebugUIActions DebugUI::draw(const DebugUIContext &ctx) {
        DebugUIActions actions;

        frame_stats_.push(ctx.delta_time,
#ifndef NDEBUG
                          ctx.pt_ms, ctx.cuda_display_ms, ctx.vk_display_ms,
                          ctx.cpu_frame_ms
#else
                          0.0f, 0.0f, 0.0f, 0.0f
#endif
        );

        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Once);
        ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // Separators between sections are drawn here so each section stays a pure
        // content routine, unaware of its position in the sequence.
        draw_frame_stats(frame_stats_);

        ImGui::Separator();
        draw_gpu_info(ctx);

        ImGui::Separator();
        draw_present_mode(ctx, actions);

        // The banner only renders content when there is an error; a separator before
        // an absent section would leave a dangling line, so it is drawn inside the
        // section (guarded by the empty check).
        draw_error_banner(ctx, actions);

        ImGui::Separator();
        draw_path_tracing(ctx, actions, frame_stats_);

        ImGui::Separator();
        draw_dlss(ctx, actions);

        ImGui::Separator();
        draw_scene_info(ctx);

        ImGui::Separator();
        draw_log_level(actions);

        ImGui::Separator();
        draw_scene(ctx, actions);

        ImGui::Separator();
        draw_env_map(ctx, actions);

        ImGui::End();

        return actions;
    }

    void DebugUI::draw_frame_stats(const FrameStats &stats) {
        ImGui::Text("FPS: %.1f (%.2f ms)", stats.avg_fps, stats.avg_frame_time_ms);
        ImGui::Text("1%% Low: %.1f (%.2f ms)", stats.low1_fps, stats.low1_frame_time_ms);
#ifndef NDEBUG
        if (stats.avg_frame_time_ms > 0.0f) {
            const float ft = stats.avg_frame_time_ms;
            const auto line = [&](const char *label, const float ms) {
                ImGui::Text("  %s: %.2f ms (%4.1f%%) ~%.0f fps",
                            label, ms, ms / ft * 100.0f,
                            ms > 0.001f ? 1000.0f / ms : 0.0f);
            };
            line("PT", stats.avg_pt_ms);
            line("Display", stats.avg_total_display_ms);
            line("  CUDA", stats.avg_cuda_display_ms);
            line("  VK", stats.avg_vk_display_ms);
            line("CPU", stats.avg_cpu_frame_ms);
        }
#endif
    }

    void DebugUI::draw_gpu_info(const DebugUIContext &ctx) {
        ImGui::Text("GPU: %s", ctx.context.gpu_name.c_str());
        ImGui::Text("Resolution: %u x %u", ctx.swapchain.extent.width, ctx.swapchain.extent.height);

        // VRAM: query_vram_usage returns nullopt when VK_EXT_memory_budget is unavailable.
        if (const auto vram = ctx.context.query_vram_usage(); vram.has_value()) {
            ImGui::Text("VRAM: %.1f / %.1f MB",
                        static_cast<double>(vram->used) / (1024.0 * 1024.0),
                        static_cast<double>(vram->budget) / (1024.0 * 1024.0));
        } else {
            ImGui::TextDisabled("VRAM: N/A");
        }
    }

    void DebugUI::draw_present_mode(const DebugUIContext &ctx, DebugUIActions &action) {
        // The combo speaks integer indices; labels[]/values[] are parallel arrays
        // mapping each index to a (label, VkPresentModeKHR) pair. FIFO is guaranteed by
        // the spec; Mailbox/Immediate appear only when supported_modes has them.
        const auto &supported = ctx.swapchain.supported_modes;
        const char *labels[3];
        VkPresentModeKHR values[3];
        int count = 0;
        int current_idx = 0;

        labels[count] = vulkan::to_label(VK_PRESENT_MODE_FIFO_KHR);
        values[count] = VK_PRESENT_MODE_FIFO_KHR;
        if (ctx.swapchain.present_mode == VK_PRESENT_MODE_FIFO_KHR) {
            current_idx = count;
        }
        ++count;

        if (std::ranges::find(supported, VK_PRESENT_MODE_MAILBOX_KHR) != supported.end()) {
            labels[count] = vulkan::to_label(VK_PRESENT_MODE_MAILBOX_KHR);
            values[count] = VK_PRESENT_MODE_MAILBOX_KHR;
            if (ctx.swapchain.present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                current_idx = count;
            }
            ++count;
        }
        if (std::ranges::find(supported, VK_PRESENT_MODE_IMMEDIATE_KHR) != supported.end()) {
            labels[count] = vulkan::to_label(VK_PRESENT_MODE_IMMEDIATE_KHR);
            values[count] = VK_PRESENT_MODE_IMMEDIATE_KHR;
            if (ctx.swapchain.present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                current_idx = count;
            }
            ++count;
        }

        // Only FIFO available: disable the combo so the selection cannot change.
        const bool only_fifo = (count == 1);
        if (only_fifo) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Combo("Present Mode", &current_idx, labels, count)) {
            ctx.swapchain.present_mode = values[current_idx];
            action.present_mode_changed = true;
        }

        if (only_fifo) {
            ImGui::EndDisabled();
        }
    }

    void DebugUI::draw_error_banner(const DebugUIContext &ctx, DebugUIActions &action) {
        if (ctx.error_message.empty()) {
            return;
        }

        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        ImGui::TextWrapped("%s", ctx.error_message.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::SmallButton("X")) {
            action.error_dismissed = true;
        }
    }

    void DebugUI::draw_path_tracing(const DebugUIContext &ctx, DebugUIActions &action,
                                    const FrameStats &stats) {
        const float sps = stats.avg_fps * static_cast<float>(ctx.settings.samples_per_frame);
        if (!ctx.settings.dlss_enabled) {
            ImGui::Text("Samples: %u  (%.0f sps)", ctx.accumulated_samples, sps);
        } else {
            ImGui::Text("%.0f sps", sps);
        }

        auto bounces = static_cast<int>(ctx.settings.max_bounces);
        if (ImGui::SliderInt("Max Bounces", &bounces, 1, 32)) {
            ctx.settings.max_bounces = static_cast<uint32_t>(bounces);
        }

        auto spp = static_cast<int>(ctx.settings.samples_per_frame);
        if (ImGui::SliderInt("Samples/Frame", &spp, 1, 64)) {
            ctx.settings.samples_per_frame = static_cast<uint32_t>(spp);
        }

        // On-release commit: each render-height change reallocates the
        // accumulation buffers, so live application during a drag would
        // thrash device memory.
        slider_uint_on_release("Render Height", &ctx.settings.render_height,
                               240, 2160, 16, 8192);
        // When DLSS is on, clamp render_height to the resolved value so
        // the slider reflects what buffers and NGX actually use.
        if (ctx.settings.dlss_enabled && ctx.dlss_rr.available()) {
            ctx.settings.render_height = ctx.dlss_rr.resolve_render_height(
                ctx.settings.render_height,
                ctx.swapchain.extent.height).render_height;
        }
        ImGui::Text("Render Resolution: %u x %u",
                    compute_render_width(ctx.settings.render_height,
                                         ctx.swapchain.extent.width,
                                         ctx.swapchain.extent.height),
                    ctx.settings.render_height);

        slider_float_deferred("Exposure (EV)", &ctx.settings.exposure_ev,
                              -8.0f, 8.0f, "%.1f");

        slider_angle_deferred("FOV", &ctx.camera.fov,
                              30.0f, 120.0f, "%.1f\xC2\xB0");

        ImGui::Checkbox("Accumulate", &ctx.settings.accumulation_enabled);

        if (ImGui::Button("Reset")) {
            action.accum_reset_requested = true;
        }
    }

    void DebugUI::draw_scene_info(const DebugUIContext &ctx) {
        const auto &pos = ctx.camera.position;
        ImGui::Text("Pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        ImGui::Text("Yaw: %.1f%s  Pitch: %.1f%s",
                    glm::degrees(ctx.camera.yaw), "\xC2\xB0",
                    glm::degrees(ctx.camera.pitch), "\xC2\xB0");

        const auto &s = ctx.scene_stats;
        ImGui::Text("Meshes/BLAS: %u  Instances: %u", s.meshes, s.instances);
        ImGui::Text("TLAS Instances: %u", s.tlas_instances);
        ImGui::Text("Materials: %u  Textures: %u", s.materials, s.textures);
        ImGui::Text("Triangles: %u  Vertices: %u", s.triangles, s.vertices);
        ImGui::Text("Emissive Triangles: %u", s.emissive_triangles);
        if (s.env_map_width > 0) {
            ImGui::Text("Env Map: %u x %u", s.env_map_width, s.env_map_height);
        } else {
            ImGui::TextDisabled("Env Map: None");
        }
    }

    void DebugUI::draw_log_level(DebugUIActions &action) {
        // Applied here directly: spdlog level is a process-global toggle with no
        // ordering concerns. action flags mirror it for Application if ever needed.
        int current_log_level = spdlog::get_level();
        if (ImGui::Combo("Log Level",
                         &current_log_level,
                         kLogLevelNames,
                         IM_ARRAYSIZE(kLogLevelNames))) {
            spdlog::set_level(static_cast<spdlog::level::level_enum>(current_log_level));
            action.log_level_changed = true;
            action.new_log_level = current_log_level;
        }
    }

    void DebugUI::draw_scene(const DebugUIContext &ctx, DebugUIActions &action) {
        if (ctx.scene_path.empty()) {
            ImGui::TextDisabled("No scene loaded");
        } else {
            const auto filename = std::filesystem::path(ctx.scene_path).filename().string();
            ImGui::Text("Scene: %s", filename.c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", ctx.scene_path.c_str());
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Load...")) {
            auto path = open_file_dialog(
                L"glTF Files (*.gltf;*.glb)\0*.gltf;*.glb\0All Files (*.*)\0*.*\0",
                L"Load Scene");
            if (!path.empty()) {
                action.scene_load_requested = true;
                action.new_scene_path = std::move(path);
            }
        }
    }
    void DebugUI::draw_env_map(const DebugUIContext &ctx, DebugUIActions &action) {
        if (ctx.env_map_path.empty()) {
            ImGui::TextDisabled("No env map loaded");
        } else {
            const auto filename = std::filesystem::path(ctx.env_map_path).filename().string();
            ImGui::Text("Env: %s", filename.c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", ctx.env_map_path.c_str());
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Load HDR...")) {
            auto path = open_file_dialog(
                L"HDR Files (*.hdr)\0*.hdr\0All Files (*.*)\0*.*\0",
                L"Load Environment Map");
            if (!path.empty()) {
                action.env_map_load_requested = true;
                action.new_env_map_path = std::move(path);
            }
        }

        ImGui::Text("IBL Rotation: %.1f%s",
                     glm::degrees(ctx.settings.env_rotation), "\xC2\xB0");
        ImGui::TextDisabled("Left drag to rotate");
    }

    void DebugUI::draw_dlss(const DebugUIContext &ctx, DebugUIActions &action) {
        const auto &dlss = ctx.dlss_rr;
        const bool available = dlss.available();

        // Enable checkbox — grayed out when hardware/driver doesn't support DLSS-RR.
        if (!available) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("DLSS Ray Reconstruction", &ctx.settings.dlss_enabled);
        if (!available) {
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::TextDisabled("(not supported)");
            return;
        }

        if (!ctx.settings.dlss_enabled) {
            return;
        }

        // Render Preset combo (Default / D / E). Change triggers feature recreate.
        {
            constexpr const char *labels[] = {"Default", "D", "E"};
            constexpr optix::DlssRenderPreset values[] = {
                optix::DlssRenderPreset::Default,
                optix::DlssRenderPreset::D,
                optix::DlssRenderPreset::E,
            };

            int current = 0;
            for (int i = 0; i < 3; ++i) {
                if (values[i] == ctx.dlss_preset) {
                    current = i;
                }
            }

            if (ImGui::Combo("Render Preset", &current, labels, 3)) {
                ctx.dlss_preset = values[current];
            }
        }

        // Read-only info when feature is active.
        if (dlss.feature_active()) {
            const auto resolved = dlss.resolve_render_height(
                ctx.settings.render_height,
                ctx.swapchain.extent.height);

            constexpr const char *mode_names[] = {
                "Max Performance", "Balanced", "Max Quality",
                "Ultra Performance", "Ultra Quality", "DLAA",
            };
            const auto mode_idx = static_cast<uint32_t>(resolved.mode);
            ImGui::Text("Quality: %s", mode_idx < 6 ? mode_names[mode_idx] : "?");

            const uint32_t actual_rh = resolved.render_height;
            const uint32_t actual_rw = compute_render_width(
                actual_rh,
                ctx.swapchain.extent.width,
                ctx.swapchain.extent.height);
            const float ratio = static_cast<float>(ctx.swapchain.extent.height)
                              / static_cast<float>(actual_rh);
            ImGui::Text("Render: %ux%u -> %ux%u (%.2fx)",
                        actual_rw, actual_rh,
                        ctx.swapchain.extent.width, ctx.swapchain.extent.height,
                        ratio);

            const uint64_t vram = dlss.vram_allocated_bytes();
            if (vram > 0) {
                ImGui::Text("VRAM: %.1f MB", static_cast<double>(vram) / (1024.0 * 1024.0));
            }
        }
    }
} // namespace qualquer::renderer
