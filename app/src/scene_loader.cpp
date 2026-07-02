/**
 * @file scene_loader.cpp
 * @brief glTF scene loading implementation.
 */

#include <qualquer/app/scene_loader.h>

#include <qualquer/renderer/texture.h>

#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/types.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <limits>
#include <map>
#include <stdexcept>

namespace qualquer::app {
    namespace {
        renderer::AABB transform_aabb(const renderer::AABB &local, const glm::mat4 &transform) {
            glm::vec3 new_min(std::numeric_limits<float>::max());
            glm::vec3 new_max(std::numeric_limits<float>::lowest());

            for (int i = 0; i < 8; ++i) {
                const glm::vec3 corner(
                    (i & 1) ? local.max.x : local.min.x,
                    (i & 2) ? local.max.y : local.min.y,
                    (i & 4) ? local.max.z : local.min.z
                );
                const auto world = glm::vec3(transform * glm::vec4(corner, 1.0f));
                new_min = glm::min(new_min, world);
                new_max = glm::max(new_max, world);
            }

            return {new_min, new_max};
        }

        // Converts a fastgltf 4x4 matrix to glm::mat4.
        // Both use column-major layout with 16 contiguous floats.
        glm::mat4 convert_matrix(const fastgltf::math::fmat4x4 &m) {
            glm::mat4 result;
            static_assert(sizeof(result) == sizeof(m), "Matrix size mismatch");
            std::memcpy(&result, &m, sizeof(result));
            return result;
        }

        renderer::AlphaMode convert_alpha_mode(const fastgltf::AlphaMode mode) {
            switch (mode) {
                case fastgltf::AlphaMode::Opaque: return renderer::AlphaMode::Opaque;
                case fastgltf::AlphaMode::Mask: return renderer::AlphaMode::Mask;
                case fastgltf::AlphaMode::Blend: return renderer::AlphaMode::Blend;
            }
            return renderer::AlphaMode::Opaque;
        }
    } // anonymous namespace

    bool SceneLoader::load(const std::string &path,
                           const renderer::DefaultTextures &default_textures) {
        spdlog::info("Loading scene: {}", path);

        try {
            const auto file_path = std::filesystem::path(path);
            constexpr auto options = fastgltf::Options::LoadExternalBuffers
                                     | fastgltf::Options::LoadExternalImages;

            auto gltf_data = fastgltf::GltfDataBuffer::FromPath(file_path);
            if (gltf_data.error() != fastgltf::Error::None) {
                throw std::runtime_error("Failed to read glTF file: " + path);
            }

            fastgltf::Parser parser;
            auto asset = parser.loadGltf(
                gltf_data.get(), file_path.parent_path(), options);
            if (asset.error() != fastgltf::Error::None) {
                throw std::runtime_error("Failed to parse glTF '" + path
                                         + "' (error " + std::to_string(static_cast<int>(asset.error())) + ")");
            }

            auto &gltf = asset.get();

            spdlog::info("glTF parsed: {} meshes, {} materials, {} textures, {} nodes",
                         gltf.meshes.size(),
                         gltf.materials.size(),
                         gltf.textures.size(),
                         gltf.nodes.size());

            const auto mesh_data = load_meshes(gltf);
            load_materials(gltf, file_path.parent_path().string(), default_textures);
            build_mesh_instances(gltf, mesh_data);
            return true;
        } catch (const std::exception &e) {
            spdlog::error("Scene loading failed: {}", e.what());
            destroy();
            return false;
        }
    }

    SceneLoader::MeshLoadResult SceneLoader::load_meshes(const fastgltf::Asset &) {
        // Implemented in a subsequent task item.
        return {};
    }

    void SceneLoader::load_materials(const fastgltf::Asset &,
                                     const std::string &,
                                     const renderer::DefaultTextures &) {
        // Implemented in a subsequent task item.
    }

    void SceneLoader::build_mesh_instances(fastgltf::Asset &,
                                           const MeshLoadResult &) {
        // Implemented in a subsequent task item.
    }

    void SceneLoader::destroy() {
        // Destroy scene textures
        for (auto &tex: textures_) {
            tex.destroy();
        }
        textures_.clear();

        // Destroy device buffers
        material_buffer_.free();
        texture_objects_buffer_.free();

        // Clear scene data (Mesh contains CudaBuffer which frees on destruction)
        meshes_.clear();
        mesh_instances_.clear();
        gpu_materials_.clear();
        texture_objects_.clear();
        scene_bounds_ = {glm::vec3(0.0f), glm::vec3(0.0f)};
    }

    std::span<const renderer::Mesh> SceneLoader::meshes() const {
        return meshes_;
    }

    std::span<const renderer::MeshInstance> SceneLoader::mesh_instances() const {
        return mesh_instances_;
    }

    const optix::CudaBuffer<renderer::Material> &SceneLoader::material_buffer() const {
        return material_buffer_;
    }

    const optix::CudaBuffer<cudaTextureObject_t> &SceneLoader::texture_objects_buffer() const {
        return texture_objects_buffer_;
    }

    const renderer::AABB &SceneLoader::scene_bounds() const {
        return scene_bounds_;
    }
} // namespace qualquer::app
