/**
 * @file config.cpp
 * @brief Application configuration persistence implementation.
 */

#include <qualquer/app/config.h>

#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace qualquer::app {
    std::filesystem::path config_file_path() {
        std::filesystem::path dir;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
        // ReSharper disable once CppDeprecatedEntity
        if (const char *local_app_data = std::getenv("LOCALAPPDATA")) {
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
            dir = std::filesystem::path(local_app_data) / "qualquer";
        } else {
            spdlog::warn("LOCALAPPDATA not set, using current directory for config");
            dir = std::filesystem::current_path() / "qualquer_config";
        }

        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            spdlog::warn("Failed to create config directory {}: {}", dir.string(), ec.message());
        }

        return dir / "config.json";
    }

    AppConfig load_config() {
        AppConfig config;

        try {
            const auto path = config_file_path();
            if (!std::filesystem::exists(path)) {
                spdlog::info("No config file found at {}, using defaults", path.string());
                return config;
            }

            std::ifstream file(path);
            if (!file.is_open()) {
                spdlog::warn("Failed to open config file {}", path.string());
                return config;
            }

            if (const auto json = nlohmann::json::parse(file);
                json.is_object()) {
                if (json.contains("scene_path") && json["scene_path"].is_string()) {
                    config.scene_path = json["scene_path"].get<std::string>();
                }
                if (json.contains("env_map_path") && json["env_map_path"].is_string()) {
                    config.env_map_path = json["env_map_path"].get<std::string>();
                }
            }
            spdlog::info("Loaded config from {}", path.string());
        } catch (const std::exception &e) {
            spdlog::warn("Failed to load config: {}", e.what());
        }

        return config;
    }

    void save_config(const AppConfig &config) {
        try {
            const auto path = config_file_path();
            const auto tmp = path.parent_path() / "config.json.tmp";

            // Write to a temp file then atomically rename (rename is atomic on NTFS),
            // so a crash mid-write never leaves a truncated config.
            {
                std::ofstream file(tmp);
                if (!file.is_open()) {
                    spdlog::warn("Failed to open temp config file for writing: {}", tmp.string());
                    return;
                }

                nlohmann::json j;
                j["scene_path"] = config.scene_path;
                j["env_map_path"] = config.env_map_path;
                file << j.dump(2);
            }

            std::error_code ec;
            std::filesystem::rename(tmp, path, ec);
            if (ec) {
                spdlog::warn("Failed to rename config file: {}", ec.message());
                std::filesystem::remove(tmp, ec);
                return;
            }

            spdlog::info("Saved config to {}", path.string());
        } catch (const std::exception &e) {
            spdlog::warn("Failed to save config: {}", e.what());
        }
    }
} // namespace qualquer::app
