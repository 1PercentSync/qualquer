#pragma once

/**
 * @file scene_types.h
 * @brief Scene data structures: AABB, Mesh, MeshInstance, AlphaMode (renderer layer).
 */

#include <cstdint>

#include <glm/glm.hpp>

#include <qualquer/optix/cuda_buffer.h>
#include <qualquer/renderer/vertex.h>

namespace qualquer::renderer {
    /**
     * @brief Axis-aligned bounding box.
     *
     * CPU-side scene metadata used for scene-bounds computation and camera
     * focus framing; not referenced on the device. Callers performing union
     * accumulation manage their own empty-box initialization (min = +inf,
     * max = -inf), so the struct carries no default that could mask that.
     */
    struct AABB {
        /** @brief Minimum corner (smallest x, y, z). */
        glm::vec3 min;

        /** @brief Maximum corner (largest x, y, z). */
        glm::vec3 max;
    };

    /**
     * @brief Alpha mode, mirroring the glTF alphaMode field.
     *
     * Drives BLAS anyhit flags (non-Opaque → REQUIRE_SINGLE_ANYHIT_CALL) and
     * anyhit shader behavior. The uint32_t underlying type lets the value be
     * stored directly in the uint32 alpha_mode slot of Material.
     */
    enum class AlphaMode : uint32_t {
        Opaque = 0, ///< Fully opaque; anyhit disabled on the BLAS geometry.
        Mask = 1,   ///< Alpha test in anyhit: ignore when alpha is below alpha_cutoff.
        Blend = 2,  ///< Stochastic alpha in anyhit (hash vs texel alpha).
    };

    /**
     * @brief GPU-resident mesh data for one glTF primitive.
     *
     * Owns the CUDA vertex and index buffers. Because CudaBuffer is move-only
     * (device memory has a single owner), Mesh is move-only as well — copying
     * would alias one device allocation under two lifetimes. group_id collapses
     * all primitives of the same glTF mesh into one multi-geometry BLAS at AS
     * build time.
     */
    struct Mesh {
        /** @brief Device vertex buffer (Vertex[]); owned. */
        optix::CudaBuffer<Vertex> vertex_buffer;

        /** @brief Device index buffer (uint32_t[]); owned. */
        optix::CudaBuffer<uint32_t> index_buffer;

        /** @brief Number of vertices in vertex_buffer. */
        uint32_t vertex_count = 0;

        /** @brief Number of indices in index_buffer. */
        uint32_t index_count = 0;

        /** @brief glTF source mesh index; groups primitives into one BLAS. */
        uint32_t group_id = 0;

        /**
         * @brief Material index for GPUGeometryInfo::material_buffer_offset.
         */
        uint32_t material_id = 0;

        /**
         * @brief True when the material's alpha_mode is Opaque.
         *
         * Controls the BLAS per-geometry flag: opaque geometries get
         * OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT (hardware skips any-hit),
         * non-opaque geometries get OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL.
         * Set by SceneLoader from the glTF material at load time.
         */
        bool opaque = true;
    };

    /**
     * @brief A renderable mesh placed in the scene.
     *
     * References a Mesh by index and carries the world transform plus a
     * world-space AABB. Material is accessed through Mesh::material_id.
     */
    struct MeshInstance {
        /** @brief Index into the loaded mesh array. */
        uint32_t mesh_id = 0;

        /** @brief World-space transform (local-to-world). */
        glm::mat4 transform{1.0f};

        /** @brief World-space AABB (local AABB transformed by this instance); set at load. */
        AABB world_bounds{};
    };
} // namespace qualquer::renderer
