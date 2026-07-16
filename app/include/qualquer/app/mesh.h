#pragma once

/**
 * @file mesh.h
 * @brief Host-side mesh utilities (app layer).
 */

#include <qualquer/renderer/vertex.h>

#include <cstdint>
#include <vector>

namespace qualquer::app {
    /**
     * @brief Generates tangent vectors via MikkTSpace for meshes lacking them.
     *
     * Requires valid normals and UV0 on every vertex. MikkTSpace outputs
     * per-face-corner tangents; shared vertices receiving different tangents
     * are split (new vertices appended, indices updated) to avoid the
     * incorrect overwrite that mikktspace.h explicitly forbids.
     *
     * @param vertices Mutable vertex vector — tangent fields are written;
     *                 new vertices may be appended for tangent-discontinuity splits.
     * @param indices  Mutable index vector — entries may be updated to reference
     *                 newly appended vertices at split points.
     */
    void generate_tangents(std::vector<renderer::Vertex> &vertices, std::vector<uint32_t> &indices);

} // namespace qualquer::app
