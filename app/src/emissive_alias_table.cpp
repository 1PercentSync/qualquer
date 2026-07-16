/**
 * @file emissive_alias_table.cpp
 * @brief Emissive triangle collection and alias table construction implementation (app layer).
 */

#include <qualquer/app/emissive_alias_table.h>

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <cmath>
#include <vector>

namespace qualquer::app {

    namespace {
        /** @brief ITU-R BT.709 luminance from linear RGB. */
        float luminance(const glm::vec3 &c) {
            return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;
        }
    } // namespace

    EmissiveAliasTableResult build_emissive_alias_table(
        const std::span<const renderer::Mesh> meshes,
        const std::span<const renderer::MeshInstance> instances,
        const std::span<const renderer::Material> materials,
        const std::span<const std::vector<renderer::Vertex>> cpu_vertices,
        const std::span<const std::vector<uint32_t>> cpu_indices) {

        std::vector<renderer::EmissiveTriangle> triangles;
        std::vector<float> powers;

        for (const auto &inst : instances) {
            const uint32_t mesh_id = inst.mesh_id;
            if (mesh_id >= meshes.size()) {
                continue;
            }

            const auto &mesh = meshes[mesh_id];
            if (mesh.material_id >= materials.size()) {
                continue;
            }

            const auto &mat = materials[mesh.material_id];
            const auto emissive_f = glm::vec3(mat.emissive_factor);

            // Skip non-emissive materials.
            if (emissive_f.r <= 0.0f && emissive_f.g <= 0.0f && emissive_f.b <= 0.0f) {
                continue;
            }

            const auto &verts = cpu_vertices[mesh_id];
            const auto &idx = cpu_indices[mesh_id];
            if (verts.empty() || idx.size() < 3) {
                continue;
            }

            const float emissive_lum = luminance(emissive_f);
            const glm::mat4 &xform = inst.transform;

            for (size_t i = 0; i + 2 < idx.size(); i += 3) {
                const uint32_t i0 = idx[i];
                const uint32_t i1 = idx[i + 1];
                const uint32_t i2 = idx[i + 2];

                if (i0 >= verts.size() || i1 >= verts.size() || i2 >= verts.size()) {
                    continue;
                }

                // Transform vertices to world space.
                const glm::vec3 v0 = glm::vec3(xform * glm::vec4(verts[i0].position, 1.0f));
                const glm::vec3 v1 = glm::vec3(xform * glm::vec4(verts[i1].position, 1.0f));
                const glm::vec3 v2 = glm::vec3(xform * glm::vec4(verts[i2].position, 1.0f));

                const glm::vec3 e1 = v1 - v0;
                const glm::vec3 e2 = v2 - v0;
                const float area = 0.5f * glm::length(glm::cross(e1, e2));

                // Skip degenerate triangles.
                if (area <= 0.0f) {
                    continue;
                }

                const glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

                triangles.push_back({
                    .v0 = {v0.x, v0.y, v0.z},
                    .normal_x = normal.x,
                    .edge1 = {e1.x, e1.y, e1.z},
                    .normal_y = normal.y,
                    .edge2 = {e2.x, e2.y, e2.z},
                    .normal_z = normal.z,
                    .emission = {emissive_f.x, emissive_f.y, emissive_f.z},
                    .emissive_tex = mat.emissive_tex,
                    .uv0 = {verts[i0].uv0.x, verts[i0].uv0.y},
                    .uv1 = {verts[i1].uv0.x, verts[i1].uv0.y},
                    .uv2 = {verts[i2].uv0.x, verts[i2].uv0.y},
                    .double_sided = mat.double_sided,
                    ._pad = 0,
                });

                powers.push_back(emissive_lum * area);
            }
        }

        const auto emissive_count = static_cast<uint32_t>(triangles.size());

        if (emissive_count == 0) {
            spdlog::info("Emissive alias table: no emissive triangles found");
            return {};
        }

        // --- Vose's alias table algorithm O(N) ---
        std::vector<renderer::AliasEntry> table(emissive_count);

        double power_sum = 0.0;
        for (const float p : powers) {
            power_sum += static_cast<double>(p);
        }

        const double avg = power_sum / static_cast<double>(emissive_count);
        std::vector<float> normalized(emissive_count);
        for (uint32_t i = 0; i < emissive_count; ++i) {
            normalized[i] = (avg > 0.0)
                                ? static_cast<float>(static_cast<double>(powers[i]) / avg)
                                : 1.0f;
        }

        std::vector<uint32_t> small;
        std::vector<uint32_t> large;
        small.reserve(emissive_count);
        large.reserve(emissive_count);
        for (uint32_t i = 0; i < emissive_count; ++i) {
            if (normalized[i] < 1.0f) {
                small.push_back(i);
            } else {
                large.push_back(i);
            }
        }

        while (!small.empty() && !large.empty()) {
            const uint32_t s = small.back();
            small.pop_back();
            const uint32_t l = large.back();
            large.pop_back();

            table[s].prob = normalized[s];
            table[s].alias = l;

            normalized[l] = (normalized[l] + normalized[s]) - 1.0f;

            if (normalized[l] < 1.0f) {
                small.push_back(l);
            } else {
                large.push_back(l);
            }
        }

        while (!large.empty()) {
            const uint32_t l = large.back();
            large.pop_back();
            table[l].prob = 1.0f;
            table[l].alias = l;
        }
        while (!small.empty()) {
            const uint32_t s = small.back();
            small.pop_back();
            table[s].prob = 1.0f;
            table[s].alias = s;
        }

        // emissive_factor is clamped to [0,1] (no KHR_materials_emissive_strength),
        // so per-triangle power = luminance(factor) * area <= area. Overflow
        // would require total world-space emissive area > 3.4e38 m².
        // NOTE: if KHR_materials_emissive_strength is added, factor can exceed 1
        // and this cast needs overflow protection (see env_alias_table.cpp scaling).
        const auto total_power = static_cast<float>(power_sum);

        spdlog::info("Emissive alias table built: {} triangles ({:.1f} KB, total_power={:.2f})",
                     emissive_count,
                     static_cast<double>(emissive_count) * sizeof(renderer::EmissiveTriangle)
                         / 1024.0,
                     total_power);

        return {
            .triangles = std::move(triangles),
            .alias_table = std::move(table),
            .total_power = total_power,
        };
    }

} // namespace qualquer::app
