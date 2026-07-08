#pragma once

/**
 * @file scene_stats.h
 * @brief Read-only scene asset statistics snapshot (renderer layer).
 */

#include <cstdint>

namespace qualquer::renderer {
    /**
     * @brief Aggregate statistics of the currently loaded scene.
     *
     * Computed by Application after scene/env-map loading and cached until
     * the next load. Passed to DebugUI as a const reference for display only.
     */
    struct SceneStats {
        /** @brief Number of meshes / BLAS (one per glTF mesh, may contain multiple primitives). */
        uint32_t meshes = 0;

        /** @brief Number of scene mesh instances (node x primitive). */
        uint32_t instances = 0;

        /** @brief Number of TLAS instances (may differ from instances when grouped). */
        uint32_t tlas_instances = 0;

        /** @brief Number of materials. */
        uint32_t materials = 0;

        /** @brief Number of loaded textures (scene textures, excluding defaults). */
        uint32_t textures = 0;

        /** @brief Total triangle count across all mesh primitives. */
        uint32_t triangles = 0;

        /** @brief Total vertex count across all mesh primitives. */
        uint32_t vertices = 0;

        /** @brief Number of emissive triangles collected for NEE. */
        uint32_t emissive_triangles = 0;

        /** @brief Env map equirect source width (0 when no env map loaded). */
        uint32_t env_map_width = 0;

        /** @brief Env map equirect source height (0 when no env map loaded). */
        uint32_t env_map_height = 0;
    };
} // namespace qualquer::renderer
