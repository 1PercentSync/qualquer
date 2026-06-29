#pragma once

/**
 * @file vertex.h
 * @brief Unified vertex format shared by host and device (renderer layer).
 */

#include <cstddef>

#include <glm/glm.hpp>

namespace qualquer::renderer {
    /**
     * @brief Fixed-layout vertex shared by all meshes.
     *
     * The same definition is included by host code (.cpp) and device code (.cu):
     * host fills it at load time, device reads it during RT intersection, and the
     * raw bytes cross the host/device boundary via cudaMemcpy — hence the layout
     * must stay identical on both sides. Missing source attributes are filled
     * with defaults at load time (normal → +Z, uv0 → 0, tangent → MikkTSpace-
     * generated or +X), so every field is always valid and the device shader
     * can sample unconditionally.
     */
    struct Vertex {
        /** @brief Geometric position (local space in the mesh vertex buffer; world transform is applied via TLAS instance). */
        glm::vec3 position;

        /** @brief Surface normal (normalized). */
        glm::vec3 normal;

        /** @brief Primary texture coordinates. */
        glm::vec2 uv0;

        /** @brief Tangent vector with handedness in w (MikkTSpace convention). */
        glm::vec4 tangent;
    };

    static_assert(sizeof(Vertex) == 48, "Vertex must stay 48 bytes (host/device layout lock)");
    static_assert(
        offsetof(Vertex, position) == 0,
        "position must sit at offset 0 — OptiX vertex fetch reads FLOAT3 at stride = sizeof(Vertex) from the buffer base")
    ;
} // namespace qualquer::renderer
