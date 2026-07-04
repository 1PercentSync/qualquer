#pragma once

/**
 * @file config.h
 * @brief Application configuration persistence (app layer).
 */

#include <filesystem>
#include <string>

namespace qualquer::app {
    /**
     * @brief Persistent application configuration.
     *
     * Serialized as JSON to %LOCALAPPDATA%\qualquer\config.json. Fields are
     * optional — empty means no value configured, and the application falls
     * back to its built-in defaults.
     */
    struct AppConfig {
        /** @brief Absolute path to the scene file (.gltf / .glb). */
        std::string scene_path;
    };

    /**
     * @brief Returns the config file path: %LOCALAPPDATA%\qualquer\config.json.
     *
     * Creates the directory if it does not exist.
     */
    [[nodiscard]] std::filesystem::path config_file_path();

    /**
     * @brief Loads configuration from disk.
     *
     * Returns a default-constructed AppConfig (empty paths) if the file does not
     * exist, is unreadable, or contains invalid JSON. Never throws.
     */
    [[nodiscard]] AppConfig load_config();

    /**
     * @brief Saves configuration to disk.
     *
     * Creates parent directories if needed. Logs a warning on failure but does
     * not throw.
     */
    void save_config(const AppConfig &config);
} // namespace qualquer::app
