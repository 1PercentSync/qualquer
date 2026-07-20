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
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace fastgltf {
    struct Asset;
    struct Primitive;
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

        /** @brief Number of scene textures loaded from glTF (excluding 2 default textures). */
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

        /** @brief Source equirect width (0 when no env loaded). */
        [[nodiscard]] uint32_t env_source_width() const { return env_source_width_; }

        /** @brief Source equirect height (0 when no env loaded). */
        [[nodiscard]] uint32_t env_source_height() const { return env_source_height_; }

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

        /** @brief Device alias table (downsampled resolution). */
        optix::CudaBuffer<renderer::EnvAliasEntry> env_alias_table_;

        /** @brief Alias table width (downsampled; may be smaller than source equirect). */
        uint32_t env_alias_width_ = 0;

        /** @brief Alias table height (downsampled; may be smaller than source equirect). */
        uint32_t env_alias_height_ = 0;

        /** @brief Source equirect width (for UI display; unaffected by alias downsampling). */
        uint32_t env_source_width_ = 0;

        /** @brief Source equirect height (for UI display; unaffected by alias downsampling). */
        uint32_t env_source_height_ = 0;

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

        /** @brief CPU-side data for a single loaded primitive; retained for emissive collection. */
        struct PrimitiveCpuData {
            /** @brief Local-space AABB computed from vertex positions. */
            renderer::AABB local_bounds;
            /** @brief CPU vertex array. */
            std::vector<renderer::Vertex> vertices;
            /** @brief CPU index array. */
            std::vector<uint32_t> indices;
        };

        /** @brief Intermediate data from mesh loading, consumed by load(). */
        struct MeshLoadResult {
            /** @brief glTF mesh index → starting index in meshes_. Last entry = sentinel. */
            std::vector<uint32_t> prim_offsets;
            /** @brief Per loaded primitive (parallel to meshes_). */
            std::vector<PrimitiveCpuData> primitives;
        };

        /**
         * @brief Loads a single glTF primitive: decode attributes, sanitize,
         *        validate indices, generate flat normals / MikkTSpace tangents.
         * @return Loaded geometry on success; std::nullopt (with a warning)
         *         when the primitive is unsupported or empty.
         */
        [[nodiscard]] static std::optional<PrimitiveCpuData> load_primitive(
            const fastgltf::Asset &gltf,
            const fastgltf::Primitive &primitive,
            std::string_view mesh_name);

        /** @brief Loads mesh primitives for referenced meshes: GPU buffers and CPU data. */
        MeshLoadResult load_meshes(const fastgltf::Asset &gltf,
                                   cudaStream_t stream,
                                   const std::vector<bool> &referenced_meshes);

        /**
         * @brief Loads samplers, textures, and material structs for referenced materials only.
         *
         * Returns a remap table indexed by old glTF material index (with the
         * default material at gltf.materials.size()) mapping to the compacted
         * gpu_materials_ index. Caller must apply it to all material_id references.
         */
        std::vector<uint32_t> load_materials(const fastgltf::Asset &gltf,
                                             const optix::DefaultTextures &default_textures,
                                             cudaStream_t stream,
                                             const std::vector<bool> &referenced_materials);
    };
} // namespace qualquer::app
