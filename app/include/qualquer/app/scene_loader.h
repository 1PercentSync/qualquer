#pragma once

/**
 * @file scene_loader.h
 * @brief glTF scene loading: meshes, materials, textures, and scene graph (app layer).
 */

#include <qualquer/optix/cuda_buffer.h>
#include <qualquer/optix/cuda_texture.h>
#include <qualquer/renderer/material.h>
#include <qualquer/renderer/scene_types.h>
#include <qualquer/renderer/texture.h>

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace fastgltf {
    struct Asset;
}

namespace qualquer::app {
    /**
     * @brief Loads glTF scenes and manages all loaded GPU resources.
     *
     * Parses a glTF file using fastgltf, creates CUDA resources (vertex/index
     * buffers, textures), fills Material structs, and populates the scene
     * instance list. Owns all loaded resource handles; call destroy() to
     * release them.
     */
    class SceneLoader {
    public:
        /**
         * @brief Loads a glTF scene from the given file path.
         *
         * On failure, logs the error, cleans up any partial state, and returns
         * false (caller gets an empty scene).
         *
         * @param path             Path to the .gltf or .glb file.
         * @param default_textures Default textures for missing material slots.
         * @return true on success, false on failure (scene remains empty).
         */
        bool load(const std::string &path,
                  const renderer::DefaultTextures &default_textures);

        /**
         * @brief Destroys all loaded resources (CUDA buffers, textures).
         *
         * Safe to call even if load() was never called.
         */
        void destroy();

        /** @brief Returns the loaded meshes (CUDA buffer handles and counts). */
        [[nodiscard]] std::span<const renderer::Mesh> meshes() const;

        /** @brief Returns all scene mesh instances (one per node-primitive combination). */
        [[nodiscard]] std::span<const renderer::MeshInstance> mesh_instances() const;

        /** @brief Returns the device material buffer. */
        [[nodiscard]] const optix::CudaBuffer<renderer::Material> &material_buffer() const;

        /** @brief Returns the device texture-object array (indexed by Material tex fields). */
        [[nodiscard]] const optix::CudaBuffer<cudaTextureObject_t> &texture_objects_buffer() const;

        /**
         * @brief Scene AABB (union of all mesh instance world_bounds).
         *
         * Available after load() completes. Empty scene returns a degenerate
         * AABB (min = max = 0).
         */
        [[nodiscard]] const renderer::AABB &scene_bounds() const;

    private:
        // ---- Loaded scene data ----

        /** @brief GPU mesh resources (one per glTF primitive). */
        std::vector<renderer::Mesh> meshes_;

        /** @brief Scene mesh instances (one per node-primitive combination). */
        std::vector<renderer::MeshInstance> mesh_instances_;

        /** @brief CPU material array (source for material_buffer_ upload). */
        std::vector<renderer::Material> gpu_materials_;

        /** @brief Device-side material array. */
        optix::CudaBuffer<renderer::Material> material_buffer_;

        /** @brief Union AABB of all mesh instance world_bounds, computed at load time. */
        renderer::AABB scene_bounds_{};

        // ---- Texture resources ----

        /** @brief Loaded scene textures (owned; destroyed in destroy()). */
        std::vector<optix::CudaTexture> textures_;

        /** @brief All cudaTextureObject_t handles (scene textures + default textures). */
        std::vector<cudaTextureObject_t> texture_objects_;

        /** @brief Device-side texture-object array, indexed by Material tex fields. */
        optix::CudaBuffer<cudaTextureObject_t> texture_objects_buffer_;

        // ---- Private loading stages ----

        /** @brief Intermediate data from mesh loading, consumed by build_mesh_instances(). */
        struct MeshLoadResult {
            /** @brief glTF mesh index → starting index in meshes_. Last entry = sentinel. */
            std::vector<uint32_t> prim_offsets;

            /** @brief Per-primitive material index (parallel to meshes_). */
            std::vector<uint32_t> material_ids;

            /** @brief Per-primitive local-space AABB (parallel to meshes_). */
            std::vector<renderer::AABB> local_bounds;
        };

        /** @brief Loads all mesh primitives: vertex/index buffers, local AABBs, material IDs. */
        MeshLoadResult load_meshes(const fastgltf::Asset &gltf);

        /** @brief Loads samplers, textures, and materials from the glTF asset. */
        void load_materials(const fastgltf::Asset &gltf,
                            const renderer::DefaultTextures &default_textures);

        /** @brief Traverses the scene graph and creates MeshInstances with world transforms. */
        void build_mesh_instances(fastgltf::Asset &gltf,
                                  const MeshLoadResult &mesh_data);
    };
} // namespace qualquer::app
