/**
 * @file scene_loader.cpp
 * @brief glTF scene loading implementation.
 */

#include <qualquer/app/scene_loader.h>

#include <qualquer/renderer/mesh.h>
#include <qualquer/renderer/texture.h>

#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>
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

    SceneLoader::MeshLoadResult SceneLoader::load_meshes(const fastgltf::Asset &gltf) {
        MeshLoadResult result;
        result.prim_offsets.reserve(gltf.meshes.size() + 1);

        for (size_t mesh_idx = 0; mesh_idx < gltf.meshes.size(); ++mesh_idx) {
            const auto &gltf_mesh = gltf.meshes[mesh_idx];
            result.prim_offsets.push_back(static_cast<uint32_t>(meshes_.size()));

            for (const auto &primitive : gltf_mesh.primitives) {
                // Position (required by glTF spec)
                const auto pos_it = primitive.findAttribute("POSITION");
                if (pos_it == primitive.attributes.end()) {
                    throw std::runtime_error("Mesh '"
                                             + std::string(gltf_mesh.name)
                                             + "' primitive missing POSITION attribute");
                }
                const auto &pos_accessor = gltf.accessors[pos_it->accessorIndex];
                const auto vertex_count = pos_accessor.count;

                std::vector<renderer::Vertex> vertices(vertex_count);

                // Compute local AABB from position data
                glm::vec3 local_min(std::numeric_limits<float>::max());
                glm::vec3 local_max(std::numeric_limits<float>::lowest());

                {
                    size_t i = 0;
                    for (auto p : fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf, pos_accessor)) {
                        vertices[i].position = {p.x(), p.y(), p.z()};
                        local_min = glm::min(local_min, vertices[i].position);
                        local_max = glm::max(local_max, vertices[i].position);
                        ++i;
                    }
                }

                // Normal (optional, default +Z)
                bool has_normals = false;
                if (const auto it = primitive.findAttribute("NORMAL");
                    it != primitive.attributes.end()) {
                    has_normals = true;
                    const auto &accessor = gltf.accessors[it->accessorIndex];
                    size_t i = 0;
                    for (auto n : fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf, accessor)) {
                        vertices[i].normal = {n.x(), n.y(), n.z()};
                        ++i;
                    }
                } else {
                    for (auto &v : vertices) {
                        v.normal = {0.0f, 0.0f, 1.0f};
                    }
                }

                // TEXCOORD_0 (optional, zero-initialized default is fine)
                bool has_uv0 = false;
                if (const auto it = primitive.findAttribute("TEXCOORD_0");
                    it != primitive.attributes.end()) {
                    has_uv0 = true;
                    const auto &accessor = gltf.accessors[it->accessorIndex];
                    size_t i = 0;
                    for (auto uv : fastgltf::iterateAccessor<fastgltf::math::fvec2>(gltf, accessor)) {
                        vertices[i].uv0 = {uv.x(), uv.y()};
                        ++i;
                    }
                }

                // TANGENT (optional)
                bool has_tangent = false;
                if (const auto it = primitive.findAttribute("TANGENT");
                    it != primitive.attributes.end()) {
                    has_tangent = true;
                    const auto &accessor = gltf.accessors[it->accessorIndex];
                    size_t i = 0;
                    for (auto t : fastgltf::iterateAccessor<fastgltf::math::fvec4>(gltf, accessor)) {
                        vertices[i].tangent = {t.x(), t.y(), t.z(), t.w()};
                        ++i;
                    }
                }

                // Indices (generate sequential if non-indexed)
                std::vector<uint32_t> indices;
                if (primitive.indicesAccessor.has_value()) {
                    const auto &accessor = gltf.accessors[*primitive.indicesAccessor];
                    indices.reserve(accessor.count);
                    for (auto idx : fastgltf::iterateAccessor<uint32_t>(gltf, accessor)) {
                        indices.push_back(idx);
                    }
                } else {
                    indices.resize(vertex_count);
                    for (size_t j = 0; j < vertex_count; ++j) {
                        indices[j] = static_cast<uint32_t>(j);
                    }
                }

                // Generate tangents via MikkTSpace if missing (needs normal + uv0)
                if (!has_tangent && has_normals && has_uv0) {
                    renderer::generate_tangents(vertices, indices);
                }

                // Create CUDA vertex and index buffers
                optix::CudaBuffer<renderer::Vertex> vb;
                vb.alloc(vertices.size());
                vb.upload(vertices.data(), vertices.size(), nullptr);

                optix::CudaBuffer<uint32_t> ib;
                ib.alloc(indices.size());
                ib.upload(indices.data(), indices.size(), nullptr);

                // Material index
                if (!primitive.materialIndex.has_value()) {
                    throw std::runtime_error("Mesh '" + std::string(gltf_mesh.name)
                                             + "' primitive has no material (required by renderer)");
                }
                const auto prim_material_id = static_cast<uint32_t>(*primitive.materialIndex);

                meshes_.push_back({
                    .vertex_buffer = std::move(vb),
                    .index_buffer = std::move(ib),
                    .vertex_count = static_cast<uint32_t>(vertices.size()),
                    .index_count = static_cast<uint32_t>(indices.size()),
                    .group_id = static_cast<uint32_t>(mesh_idx),
                    .material_id = prim_material_id,
                });

                result.material_ids.push_back(prim_material_id);
                result.local_bounds.push_back({local_min, local_max});
            }
        }

        // Sentinel for the last mesh's primitive range
        result.prim_offsets.push_back(static_cast<uint32_t>(meshes_.size()));

        spdlog::info("Loaded {} mesh primitives", meshes_.size());
        return result;
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
