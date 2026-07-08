#pragma once

/**
 * @file emissive_alias_table.h
 * @brief Emissive triangle collection and alias table construction for NEE (app layer).
 */

#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/material.h>
#include <qualquer/renderer/scene_types.h>
#include <qualquer/renderer/vertex.h>

#include <cstdint>
#include <span>
#include <vector>

namespace qualquer::app {

    /**
     * @brief Result of collecting emissive triangles and building their alias table.
     *
     * Holds CPU-side data ready for GPU upload. Lifetime management and upload
     * are handled by the caller (SceneLoader).
     */
    struct EmissiveAliasTableResult {
        /** @brief World-space emissive triangles with per-vertex UV and emission data. */
        std::vector<renderer::EmissiveTriangle> triangles;

        /** @brief Power-weighted alias table (one entry per emissive triangle). */
        std::vector<renderer::AliasEntry> alias_table;

        /** @brief Sum of all triangle power weights (luminance(emissive_factor) × area). */
        float total_power;
    };

    /**
     * @brief Collects emissive triangles from scene meshes and builds a power-weighted alias table.
     *
     * Scans mesh instances for materials with emissive_factor > 0, transforms
     * vertices to world space, computes per-triangle area and power weight
     * (luminance(emissive_factor) × area), and builds a Vose's O(N) alias table
     * for O(1) importance sampling during NEE.
     *
     * @param meshes         Loaded mesh descriptors (parallel to cpu_vertices / cpu_indices).
     * @param instances      Scene mesh instances with world transforms.
     * @param materials      GPU material data (emissive_factor lookup).
     * @param cpu_vertices   Per-mesh CPU vertex arrays (parallel to meshes).
     * @param cpu_indices    Per-mesh CPU index arrays (parallel to meshes).
     * @return Emissive triangles, alias table, and total power; all empty if no emissive geometry.
     */
    [[nodiscard]] EmissiveAliasTableResult build_emissive_alias_table(
        std::span<const renderer::Mesh> meshes,
        std::span<const renderer::MeshInstance> instances,
        std::span<const renderer::Material> materials,
        std::span<const std::vector<renderer::Vertex>> cpu_vertices,
        std::span<const std::vector<uint32_t>> cpu_indices);

} // namespace qualquer::app
