/**
 * @file scene_loader.cpp
 * @brief glTF scene loading implementation.
 */

#include <qualquer/app/scene_loader.h>

#include <qualquer/renderer/cache.h>
#include <qualquer/renderer/mesh.h>
#include <qualquer/renderer/texture.h>

#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <spdlog/spdlog.h>

#include <cassert>
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
        // Visits the raw encoded bytes (JPEG/PNG) of a glTF image and
        // invokes the callback with (const uint8_t* data, size_t size).
        // Handles all fastgltf source types used with LoadExternalImages.
        template<typename Fn>
        void visit_gltf_image_bytes(const fastgltf::Asset &gltf,
                                    const fastgltf::Image &image,
                                    Fn &&callback) {
            auto invoke = [&](const auto *data, const size_t size) {
                callback(reinterpret_cast<const uint8_t *>(data), size);
            };

            std::visit(fastgltf::visitor{
                           [](const fastgltf::sources::URI &) {
                               assert(false && "URI source should not appear with LoadExternalImages");
                           },
                           [&](const fastgltf::sources::Array &array) {
                               invoke(array.bytes.data(), array.bytes.size_bytes());
                           },
                           [&](const fastgltf::sources::Vector &vec) {
                               invoke(vec.bytes.data(), vec.bytes.size());
                           },
                           [&](const fastgltf::sources::BufferView &bv) {
                               const auto &view = gltf.bufferViews[bv.bufferViewIndex];
                               const auto &buffer = gltf.buffers[view.bufferIndex];
                               std::visit(fastgltf::visitor{
                                              [&](const fastgltf::sources::Array &arr) {
                                                  invoke(arr.bytes.data() + view.byteOffset,
                                                         view.byteLength);
                                              },
                                              [&](const fastgltf::sources::Vector &v) {
                                                  invoke(v.bytes.data() + view.byteOffset,
                                                         view.byteLength);
                                              },
                                              [&](const fastgltf::sources::ByteView &bytes) {
                                                  invoke(bytes.bytes.data() + view.byteOffset,
                                                         view.byteLength);
                                              },
                                              [](auto &&) {
                                                  throw std::runtime_error(
                                                      "Unsupported buffer data source for image");
                                              }
                                          }, buffer.data);
                           },
                           [&](const fastgltf::sources::ByteView &bytes) {
                               invoke(bytes.bytes.data(), bytes.bytes.size());
                           },
                           [](auto &&) {
                               throw std::runtime_error("Unsupported image source type");
                           }
                       }, image.data);
        }

        /// Decodes a glTF image into CPU RGBA8 pixel data.
        renderer::ImageData decode_gltf_image(const fastgltf::Asset &gltf,
                                              const fastgltf::Image &image) {
            renderer::ImageData result;
            visit_gltf_image_bytes(gltf, image, [&](const uint8_t *data, const size_t size) {
                result = renderer::load_image_from_memory(data, size);
            });

            if (!result.valid()) {
                throw std::runtime_error("Failed to decode glTF image '"
                                         + std::string(image.name) + "'");
            }
            return result;
        }

        /// Computes a content hash of the raw source bytes without decoding.
        std::string hash_gltf_image(const fastgltf::Asset &gltf,
                                    const fastgltf::Image &image) {
            std::string hash;
            visit_gltf_image_bytes(gltf, image, [&](const uint8_t *data, const size_t size) {
                hash = renderer::content_hash(data, size);
            });
            return hash;
        }

        /// Converts a fastgltf wrap mode to CUDA texture address mode.
        cudaTextureAddressMode convert_wrap(const fastgltf::Wrap wrap) {
            switch (wrap) {
                case fastgltf::Wrap::Repeat: return cudaAddressModeWrap;
                case fastgltf::Wrap::ClampToEdge: return cudaAddressModeClamp;
                case fastgltf::Wrap::MirroredRepeat: return cudaAddressModeMirror;
            }
            return cudaAddressModeWrap;
        }

        /// Converts a glTF sampler to a CUDA SamplerDesc.
        /// CUDA has a single filterMode (shared by mag/min); the minFilter's
        /// base part is used (more impactful for mipmapped textures).
        renderer::SamplerDesc convert_gltf_sampler(const fastgltf::Sampler &sampler) {
            renderer::SamplerDesc desc{
                .filter_mode = cudaFilterModeLinear,
                .mipmap_filter_mode = cudaFilterModeLinear,
                .address_mode_u = convert_wrap(sampler.wrapS),
                .address_mode_v = convert_wrap(sampler.wrapT),
            };

            if (sampler.minFilter.has_value()) {
                switch (*sampler.minFilter) {
                    case fastgltf::Filter::Nearest:
                    case fastgltf::Filter::NearestMipMapNearest:
                    case fastgltf::Filter::NearestMipMapLinear:
                        desc.filter_mode = cudaFilterModePoint;
                        break;
                    default:
                        break;
                }

                switch (*sampler.minFilter) {
                    case fastgltf::Filter::NearestMipMapLinear:
                    case fastgltf::Filter::LinearMipMapLinear:
                        desc.mipmap_filter_mode = cudaFilterModeLinear;
                        break;
                    default:
                        desc.mipmap_filter_mode = cudaFilterModePoint;
                        break;
                }
            }

            return desc;
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

                optix::CudaBuffer<renderer::Vertex> vb;
                vb.alloc(vertices.size());
                vb.upload(vertices.data(), vertices.size(), nullptr);

                optix::CudaBuffer<uint32_t> ib;
                ib.alloc(indices.size());
                ib.upload(indices.data(), indices.size(), nullptr);

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

    void SceneLoader::load_materials(const fastgltf::Asset &gltf,
                                     const std::string &,
                                     const renderer::DefaultTextures &default_textures) {
        // ---- Default texture indices (reserved at the front of texture_objects_) ----

        texture_objects_.push_back(default_textures.white.texture_object);
        texture_objects_.push_back(default_textures.flat_normal.texture_object);
        texture_objects_.push_back(default_textures.black.texture_object);

        // ---- Collect unique (texture_index, role) pairs ----
        using TexKey = std::pair<size_t, renderer::TextureRole>;
        std::map<TexKey, size_t> unique_tex_map;

        struct TexEntry {
            size_t texture_index;
            renderer::TextureRole role;
        };
        std::vector<TexEntry> unique_entries;

        for (const auto &mat : gltf.materials) {
            const auto &pbr = mat.pbrData;
            auto collect = [&](const auto &opt_tex, const renderer::TextureRole role) {
                if (!opt_tex.has_value()) { return; }
                const auto key = std::make_pair(opt_tex->textureIndex, role);
                if (unique_tex_map.contains(key)) { return; }
                unique_tex_map[key] = unique_entries.size();
                unique_entries.push_back({opt_tex->textureIndex, role});
            };
            collect(pbr.baseColorTexture, renderer::TextureRole::Color);
            collect(pbr.metallicRoughnessTexture, renderer::TextureRole::Linear);
            collect(mat.normalTexture, renderer::TextureRole::Normal);
            collect(mat.occlusionTexture, renderer::TextureRole::Linear);
            collect(mat.emissiveTexture, renderer::TextureRole::Color);
        }

        // ---- Hash source bytes + cache check (serial, fast) ----
        const auto tex_count = static_cast<int>(unique_entries.size());

        std::vector<std::string> source_hashes(tex_count);
        std::vector<renderer::PreparedTexture> prepared_textures(tex_count);
        std::vector cache_hit(tex_count, false);

        for (int i = 0; i < tex_count; ++i) {
            const auto &tex = gltf.textures[unique_entries[i].texture_index];
            assert(tex.imageIndex.has_value() && "glTF texture must have an image source");
            source_hashes[i] = hash_gltf_image(gltf, gltf.images[*tex.imageIndex]);
            if (auto cached = renderer::load_cached_texture(
                    source_hashes[i], unique_entries[i].role)) {
                prepared_textures[i] = std::move(*cached);
                cache_hit[i] = true;
            }
        }

        // ---- Decode cache-miss images (serial) ----
        std::vector<renderer::ImageData> decoded_images(tex_count);
        for (int i = 0; i < tex_count; ++i) {
            if (cache_hit[i]) { continue; }
            const auto &tex = gltf.textures[unique_entries[i].texture_index];
            decoded_images[i] = decode_gltf_image(gltf, gltf.images[*tex.imageIndex]);
        }

        // ---- Parallel BC compression for cache misses only ----
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < tex_count; ++i) {
            if (cache_hit[i]) { continue; }
            prepared_textures[i] = renderer::compress_texture(
                decoded_images[i], unique_entries[i].role, source_hashes[i]);
        }

        decoded_images.clear();

        // ---- Serial GPU upload ----
        constexpr renderer::SamplerDesc default_sampler{
            .filter_mode = cudaFilterModeLinear,
            .mipmap_filter_mode = cudaFilterModeLinear,
            .address_mode_u = cudaAddressModeWrap,
            .address_mode_v = cudaAddressModeWrap,
        };

        std::map<TexKey, uint32_t> tex_index_cache;

        for (size_t i = 0; i < unique_entries.size(); ++i) {
            const auto &entry = unique_entries[i];
            const auto &tex = gltf.textures[entry.texture_index];
            const auto sampler = tex.samplerIndex.has_value()
                                     ? convert_gltf_sampler(gltf.samplers[*tex.samplerIndex])
                                     : default_sampler;

            auto cuda_texture = renderer::finalize_texture(prepared_textures[i], sampler);
            const auto obj_index = static_cast<uint32_t>(texture_objects_.size());
            texture_objects_.push_back(cuda_texture.texture_object);
            textures_.push_back(std::move(cuda_texture));
            tex_index_cache[{entry.texture_index, entry.role}] = obj_index;
        }
        prepared_textures.clear();

        // ---- Fill materials ----
        auto resolve_texture = [&](const size_t texture_index,
                                   const renderer::TextureRole role) -> uint32_t {
            const auto it = tex_index_cache.find({texture_index, role});
            assert(it != tex_index_cache.end() && "Texture must have been prepared");
            return it->second;
        };

        gpu_materials_.reserve(gltf.materials.size());

        for (const auto &mat : gltf.materials) {
            constexpr uint32_t kDefaultWhiteIdx = 0;
            renderer::Material data{};
            // ReSharper disable once CppUseStructuredBinding
            const auto &pbr = mat.pbrData;

            data.base_color_factor = {
                pbr.baseColorFactor[0], pbr.baseColorFactor[1],
                pbr.baseColorFactor[2], pbr.baseColorFactor[3]
            };
            data.emissive_factor = {
                mat.emissiveFactor[0], mat.emissiveFactor[1],
                mat.emissiveFactor[2], 0.0f
            };
            data.metallic_factor = pbr.metallicFactor;
            data.roughness_factor = pbr.roughnessFactor;
            data.normal_scale = mat.normalTexture.has_value()
                                    ? mat.normalTexture->scale : 1.0f;
            data.occlusion_strength = mat.occlusionTexture.has_value()
                                          ? mat.occlusionTexture->strength : 1.0f;
            data.alpha_cutoff = mat.alphaCutoff;
            data.alpha_mode = static_cast<uint32_t>(convert_alpha_mode(mat.alphaMode));
            data.double_sided = mat.doubleSided ? 1u : 0u;

            // Texture references (UINT32_MAX → default fallback below)
            data.base_color_tex = pbr.baseColorTexture.has_value()
                                      ? resolve_texture(pbr.baseColorTexture->textureIndex,
                                                        renderer::TextureRole::Color)
                                      : UINT32_MAX;
            data.metallic_roughness_tex = pbr.metallicRoughnessTexture.has_value()
                                              ? resolve_texture(
                                                    pbr.metallicRoughnessTexture->textureIndex,
                                                    renderer::TextureRole::Linear)
                                              : UINT32_MAX;
            data.normal_tex = mat.normalTexture.has_value()
                                  ? resolve_texture(mat.normalTexture->textureIndex,
                                                    renderer::TextureRole::Normal)
                                  : UINT32_MAX;
            data.occlusion_tex = mat.occlusionTexture.has_value()
                                     ? resolve_texture(mat.occlusionTexture->textureIndex,
                                                       renderer::TextureRole::Linear)
                                     : UINT32_MAX;
            data.emissive_tex = mat.emissiveTexture.has_value()
                                    ? resolve_texture(mat.emissiveTexture->textureIndex,
                                                      renderer::TextureRole::Color)
                                    : UINT32_MAX;

            if (data.base_color_tex == UINT32_MAX) { data.base_color_tex = kDefaultWhiteIdx; }
            if (data.metallic_roughness_tex == UINT32_MAX) { data.metallic_roughness_tex = kDefaultWhiteIdx; }
            if (data.normal_tex == UINT32_MAX) {
                constexpr uint32_t kDefaultFlatNormalIdx = 1;
                data.normal_tex = kDefaultFlatNormalIdx;
            }
            if (data.occlusion_tex == UINT32_MAX) { data.occlusion_tex = kDefaultWhiteIdx; }
            if (data.emissive_tex == UINT32_MAX) {
                constexpr uint32_t kDefaultBlackIdx = 2;
                data.emissive_tex = kDefaultBlackIdx;
            }

            gpu_materials_.push_back(data);
        }

        if (!gpu_materials_.empty()) {
            material_buffer_.alloc(gpu_materials_.size());
            material_buffer_.upload(gpu_materials_.data(), gpu_materials_.size(), nullptr);
        }

        if (!texture_objects_.empty()) {
            texture_objects_buffer_.alloc(texture_objects_.size());
            texture_objects_buffer_.upload(texture_objects_.data(), texture_objects_.size(), nullptr);
        }

        spdlog::info("Loaded {} materials, {} scene textures (+ 3 defaults)",
                     gpu_materials_.size(), textures_.size());
    }

    void SceneLoader::build_mesh_instances(fastgltf::Asset &gltf,
                                           const MeshLoadResult &mesh_data) {
        if (gltf.scenes.empty()) {
            spdlog::warn("No scenes in glTF file, no mesh instances created");
            return;
        }

        const auto scene_index = gltf.defaultScene.value_or(0);

        fastgltf::iterateSceneNodes(
            gltf, scene_index, fastgltf::math::fmat4x4(1.0f),
            [&](fastgltf::Node &node, const fastgltf::math::fmat4x4 &world_transform) {
                if (!node.meshIndex.has_value()) { return; }

                const auto world_mat = convert_matrix(world_transform);
                const auto gltf_mesh_idx = *node.meshIndex;
                const uint32_t prim_start = mesh_data.prim_offsets[gltf_mesh_idx];
                const uint32_t prim_end = mesh_data.prim_offsets[gltf_mesh_idx + 1];

                for (uint32_t i = prim_start; i < prim_end; ++i) {
                    mesh_instances_.push_back({
                        .mesh_id = i,
                        .material_id = mesh_data.material_ids[i],
                        .transform = world_mat,
                        .world_bounds = transform_aabb(mesh_data.local_bounds[i], world_mat),
                    });
                }
            });

        spdlog::info("Created {} mesh instances from {} nodes",
                     mesh_instances_.size(), gltf.nodes.size());

        if (!mesh_instances_.empty()) {
            scene_bounds_ = mesh_instances_[0].world_bounds;
            for (size_t i = 1; i < mesh_instances_.size(); ++i) {
                scene_bounds_.min = glm::min(scene_bounds_.min, mesh_instances_[i].world_bounds.min);
                scene_bounds_.max = glm::max(scene_bounds_.max, mesh_instances_[i].world_bounds.max);
            }
        }
    }

    void SceneLoader::destroy() {
        for (auto &tex: textures_) {
            tex.destroy();
        }
        textures_.clear();

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
