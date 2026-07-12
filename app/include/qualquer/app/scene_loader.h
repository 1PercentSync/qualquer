#pragma once

/**
 * @file scene_loader.h
 * @brief glTF scene loading: meshes, materials, textures, and scene graph (app layer).
 */

#include <qualquer/optix/cuda_buffer.h>
#include <qualquer/optix/cuda_texture.h>
#include <qualquer/renderer/launch_params.h>
#include <qualquer/renderer/material.h>
#include <qualquer/renderer/scene_types.h>
#include <qualquer/optix/cuda_texture_upload.h>

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
         * @param stream           CUDA stream for GPU uploads (must be the same
         *                         stream used by downstream consumers like AS build).
         * @return true on success, false on failure (scene remains empty).
         */
        bool load(const std::string &path,
                  const optix::DefaultTextures &default_textures,
                  cudaStream_t stream);

        /**
         * @brief Loads an HDR environment map and prepares GPU resources.
         *
         * Pipeline: stb_image decode → equirect-to-cubemap CUDA kernel →
         * BC6H compression (with KTX2 disk cache) → finalize_texture upload →
         * alias table construction → CudaBuffer upload.
         *
         * Safe to call multiple times (destroys previous env map first).
         * On failure, logs the error and leaves env resources empty (no env
         * lighting, but rendering continues).
         *
         * @param path   Path to the .hdr environment map file.
         * @param stream CUDA stream for GPU uploads.
         * @return true on success, false on failure.
         */
        bool load_env_map(const std::string &path, cudaStream_t stream);

        /**
         * @brief Destroys environment map GPU resources only.
         *
         * Called by destroy() and before load_env_map replaces resources.
         */
        void destroy_env_map();

        /**
         * @brief Destroys all loaded resources (CUDA buffers, textures, env map).
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

        /** @brief Number of scene textures loaded from glTF (excluding 3 default textures). */
        [[nodiscard]] uint32_t scene_texture_count() const;

        /**
         * @brief Scene AABB (union of all mesh instance world_bounds).
         *
         * Available after load() completes. Empty scene returns a degenerate
         * AABB (min = max = 0).
         */
        [[nodiscard]] const renderer::AABB &scene_bounds() const;

        /**
         * @brief Packed environment-map light resources for SceneRenderInput / LaunchParams.
         *
         * Device pointers borrow from owned CudaTexture / CudaBuffer members;
         * valid until destroy_env_map() or destroy().
         */
        [[nodiscard]] renderer::EnvLightData env_light() const;

        /**
         * @brief Packed emissive-triangle light resources for SceneRenderInput / LaunchParams.
         *
         * Device pointers borrow from owned CudaBuffer members; valid until the
         * next load() or destroy().
         */
        [[nodiscard]] renderer::EmissiveLightData emissive_light() const;

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

        // ---- Environment map resources ----

        /** @brief Env cubemap GPU texture (owned; destroyed in destroy_env_map()). */
        optix::CudaTexture env_cubemap_texture_;

        /** @brief Device alias table (one EnvAliasEntry per equirect pixel). */
        optix::CudaBuffer<renderer::EnvAliasEntry> env_alias_table_;

        /** @brief Equirect source width used for alias table dimensions. */
        uint32_t env_equirect_width_ = 0;

        /** @brief Equirect source height used for alias table dimensions. */
        uint32_t env_equirect_height_ = 0;

        /** @brief Sin-weighted total luminance (alias table normalization constant). */
        float env_total_luminance_ = 0.0f;

        // ---- Emissive triangle resources ----

        /** @brief Device emissive triangle array. */
        optix::CudaBuffer<renderer::EmissiveTriangle> emissive_triangles_;

        /** @brief Device alias table (one AliasEntry per emissive triangle). */
        optix::CudaBuffer<renderer::AliasEntry> emissive_alias_table_;

        /** @brief Total radiant power across all emissive triangles. */
        float emissive_total_power_ = 0.0f;

        // ---- Private loading stages ----

        /** @brief Intermediate data from mesh loading, consumed by build_mesh_instances(). */
        struct MeshLoadResult {
            /** @brief glTF mesh index → starting index in meshes_. Last entry = sentinel. */
            std::vector<uint32_t> prim_offsets;

            /** @brief Per-primitive material index (parallel to meshes_). */
            std::vector<uint32_t> material_ids;

            /** @brief Per-primitive local-space AABB (parallel to meshes_). */
            std::vector<renderer::AABB> local_bounds;

            /** @brief Per-primitive CPU vertex data (parallel to meshes_); retained for emissive collection. */
            std::vector<std::vector<renderer::Vertex>> cpu_vertices;

            /** @brief Per-primitive CPU index data (parallel to meshes_); retained for emissive collection. */
            std::vector<std::vector<uint32_t>> cpu_indices;
        };

        /** @brief Loads all mesh primitives: vertex/index buffers, local AABBs, material IDs. */
        MeshLoadResult load_meshes(const fastgltf::Asset &gltf, cudaStream_t stream);

        /** @brief Loads samplers, textures, and materials from the glTF asset. */
        void load_materials(const fastgltf::Asset &gltf,
                            const optix::DefaultTextures &default_textures,
                            cudaStream_t stream);

        /** @brief Traverses the scene graph and creates MeshInstances with world transforms. */
        void build_mesh_instances(fastgltf::Asset &gltf,
                                  const MeshLoadResult &mesh_data);
    };
} // namespace qualquer::app
