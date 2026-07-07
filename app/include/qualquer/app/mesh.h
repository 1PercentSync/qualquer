#pragma once

/**
 * @file mesh.h
 * @brief Host-side mesh utilities (app layer).
 */

#include <qualquer/renderer/vertex.h>

#include <cstdint>
#include <span>

namespace qualquer::app {
    /**
     * @brief Generates tangent vectors via MikkTSpace for meshes lacking them.
     *
     * Requires valid normals and UV0 on every vertex; writes tangent (xyz)
     * and handedness (w) into the tangent field of each vertex referenced by
     * the index buffer.
     *
     * @param vertices Mutable vertex span — tangent fields are written in place.
     * @param indices  Triangle index buffer (size must be a multiple of 3).
     */
    void generate_tangents(std::span<renderer::Vertex> vertices, std::span<const uint32_t> indices);

} // namespace qualquer::app
