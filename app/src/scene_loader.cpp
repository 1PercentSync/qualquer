/**
 * @file scene_loader.cpp
 * @brief glTF scene loading implementation.
 */

#include <qualquer/app/scene_loader.h>

#include <qualquer/app/cache.h>
#include <qualquer/app/emissive_alias_table.h>
#include <qualquer/app/env_alias_table.h>
#include <qualquer/app/equirect_to_cubemap.h>
#include <qualquer/app/mesh.h>
#include <qualquer/app/texture.h>

#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <limits>
#include <map>
#include <numeric>
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

        /// Clamps a float to [lo, hi] per glTF schema; non-finite values
        /// (NaN / Inf from malformed assets) fall back to the schema default.
        float spec_clamp(const float v, const float lo, const float hi, const float fallback) {
            return std::isfinite(v) ? std::clamp(v, lo, hi) : fallback;
        }

        /// Ensures a float is finite; non-finite values fall back to the default.
        float ensure_finite(const float v, const float fallback) {
            return std::isfinite(v) ? v : fallback;
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
        ImageData decode_gltf_image(const fastgltf::Asset &gltf,
                                              const fastgltf::Image &image) {
            ImageData result;
            visit_gltf_image_bytes(gltf, image, [&](const uint8_t *data, const size_t size) {
                result = load_image_from_memory(data, size);
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
                hash = content_hash(data, size);
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
        optix::SamplerDesc convert_gltf_sampler(const fastgltf::Sampler &sampler) {
            optix::SamplerDesc desc{
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
                           const optix::DefaultTextures &default_textures,
                           const cudaStream_t stream) {
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

            auto mesh_data = load_meshes(gltf, stream);
            load_materials(gltf, default_textures, stream);
            build_mesh_instances(gltf, mesh_data);

            // Collect emissive triangles and build alias table (needs materials
            // + world transforms, so runs after load_materials + build_mesh_instances).
            // CPU vertex/index data in mesh_data is freed when it goes out of scope.
            auto emissive_result = build_emissive_alias_table(
                meshes_, mesh_instances_, gpu_materials_,
                mesh_data.cpu_vertices, mesh_data.cpu_indices);

            emissive_total_power_ = emissive_result.total_power;

            if (!emissive_result.triangles.empty()) {
                emissive_triangles_.alloc(emissive_result.triangles.size());
                emissive_triangles_.upload(emissive_result.triangles.data(),
                                           emissive_result.triangles.size(), stream);

                emissive_alias_table_.alloc(emissive_result.alias_table.size());
                emissive_alias_table_.upload(emissive_result.alias_table.data(),
                                             emissive_result.alias_table.size(), stream);
            }

            // Ensure all async copies complete before local sources are
            // destroyed (mesh_data vertex/index vectors, emissive_result
            // triangles/alias_table). Persistent-member sources
            // (gpu_materials_, texture_objects_) need no sync.
            CUDA_CHECK(cudaStreamSynchronize(stream));

            return true;
        } catch (const std::exception &e) {
            spdlog::error("Scene loading failed: {}", e.what());
            destroy();
            return false;
        }
    }

    SceneLoader::MeshLoadResult SceneLoader::load_meshes(const fastgltf::Asset &gltf,
                                                          const cudaStream_t stream) {
        MeshLoadResult result;
        result.prim_offsets.reserve(gltf.meshes.size() + 1);

        for (size_t mesh_idx = 0; mesh_idx < gltf.meshes.size(); ++mesh_idx) {
            const auto &gltf_mesh = gltf.meshes[mesh_idx];
            result.prim_offsets.push_back(static_cast<uint32_t>(meshes_.size()));

            for (const auto &primitive : gltf_mesh.primitives) {
                // Only triangle primitives are supported (glTF default mode=4).
                if (primitive.type != fastgltf::PrimitiveType::Triangles) {
                    spdlog::warn("Mesh '{}' primitive skipped: unsupported type {}",
                                 std::string(gltf_mesh.name),
                                 static_cast<int>(primitive.type));
                    continue;
                }

                // Position (required by glTF spec)
                const auto pos_it = primitive.findAttribute("POSITION");
                if (pos_it == primitive.attributes.end()) {
                    throw std::runtime_error("Mesh '"
                                             + std::string(gltf_mesh.name)
                                             + "' primitive missing POSITION attribute");
                }
                const auto &pos_accessor = gltf.accessors[pos_it->accessorIndex];
                auto vertex_count = pos_accessor.count;
                if (vertex_count == 0) {
                    spdlog::warn("Mesh '{}' primitive skipped: empty (0 vertices)",
                                 std::string(gltf_mesh.name));
                    continue;
                }

                std::vector<renderer::Vertex> vertices(vertex_count);

                glm::vec3 local_min(std::numeric_limits<float>::max());
                glm::vec3 local_max(std::numeric_limits<float>::lowest());

                {
                    size_t i = 0;
                    for (auto p : fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf, pos_accessor)) {
                        glm::vec3 pos{p.x(), p.y(), p.z()};
                        if (!std::isfinite(pos.x) || !std::isfinite(pos.y) || !std::isfinite(pos.z)) {
                            pos = {0.0f, 0.0f, 0.0f};
                        }
                        vertices[i].position = pos;
                        local_min = glm::min(local_min, pos);
                        local_max = glm::max(local_max, pos);
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
                        glm::vec3 normal{n.x(), n.y(), n.z()};
                        if (!std::isfinite(normal.x) || !std::isfinite(normal.y)
                            || !std::isfinite(normal.z)
                            || glm::dot(normal, normal) < 1e-12f) {
                            normal = {0.0f, 0.0f, 1.0f};
                        }
                        vertices[i].normal = normal;
                        ++i;
                    }
                } else {
                    for (auto &v : vertices) {
                        v.normal = {0.0f, 0.0f, 1.0f};
                    }
                }

                // TEXCOORD_0 only. glTF allows per-texture texCoord set selection
                // (textureInfo.texCoord); currently all textures sample from set 0.
                // Assets using TEXCOORD_1+ will render with incorrect UVs for those
                // textures. Acceptable until such assets are encountered in practice.
                bool has_uv0 = false;
                if (const auto it = primitive.findAttribute("TEXCOORD_0");
                    it != primitive.attributes.end()) {
                    has_uv0 = true;
                    const auto &accessor = gltf.accessors[it->accessorIndex];
                    size_t i = 0;
                    for (auto uv : fastgltf::iterateAccessor<fastgltf::math::fvec2>(gltf, accessor)) {
                        const float u = std::isfinite(uv.x()) ? uv.x() : 0.0f;
                        const float v = std::isfinite(uv.y()) ? uv.y() : 0.0f;
                        vertices[i].uv0 = {u, v};
                        ++i;
                    }
                }

                // COLOR_0 (optional, default white — multiplied into base_color)
                if (const auto it = primitive.findAttribute("COLOR_0");
                    it != primitive.attributes.end()) {
                    const auto &accessor = gltf.accessors[it->accessorIndex];
                    size_t i = 0;
                    if (accessor.type == fastgltf::AccessorType::Vec4) {
                        for (auto c : fastgltf::iterateAccessor<fastgltf::math::fvec4>(gltf, accessor)) {
                            glm::vec4 color{c.x(), c.y(), c.z(), c.w()};
                            if (!std::isfinite(color.x) || !std::isfinite(color.y)
                                || !std::isfinite(color.z) || !std::isfinite(color.w)) {
                                color = {1.0f, 1.0f, 1.0f, 1.0f};
                            } else {
                                color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
                            }
                            vertices[i].color = color;
                            ++i;
                        }
                    } else {
                        for (auto c : fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf, accessor)) {
                            glm::vec3 rgb{c.x(), c.y(), c.z()};
                            if (!std::isfinite(rgb.x) || !std::isfinite(rgb.y)
                                || !std::isfinite(rgb.z)) {
                                rgb = {1.0f, 1.0f, 1.0f};
                            } else {
                                rgb = glm::clamp(rgb, glm::vec3(0.0f), glm::vec3(1.0f));
                            }
                            vertices[i].color = {rgb.x, rgb.y, rgb.z, 1.0f};
                            ++i;
                        }
                    }
                } else {
                    for (auto &v : vertices) {
                        v.color = {1.0f, 1.0f, 1.0f, 1.0f};
                    }
                }

                // TANGENT (optional)
                // Per glTF 2.0 spec:
                //   §1306: W MUST be 1.0 or -1.0
                //   §1289: XYZ portion is normalized (renormalize if off)
                //   §1382: MUST be ignored when NORMAL is absent
                //   §1384: same-triangle vertices SHOULD have same W
                // Invalid tangents trigger MikkTSpace regeneration.
                bool has_tangent = false;
                if (has_normals) {
                    if (const auto it = primitive.findAttribute("TANGENT");
                        it != primitive.attributes.end()) {
                        has_tangent = true;
                        const auto &accessor = gltf.accessors[it->accessorIndex];
                        size_t i = 0;
                        for (auto t : fastgltf::iterateAccessor<fastgltf::math::fvec4>(gltf, accessor)) {
                            glm::vec4 tan{t.x(), t.y(), t.z(), t.w()};
                            const glm::vec3 xyz{tan.x, tan.y, tan.z};
                            const float len2 = glm::dot(xyz, xyz);
                            if (!std::isfinite(tan.x) || !std::isfinite(tan.y)
                                || !std::isfinite(tan.z) || !std::isfinite(tan.w)
                                || len2 < 1e-12f
                                || (tan.w != 1.0f && tan.w != -1.0f)) {
                                has_tangent = false;
                                break;
                            }
                            const glm::vec3 normalized_xyz = xyz / std::sqrt(len2);
                            tan = {normalized_xyz.x, normalized_xyz.y, normalized_xyz.z, tan.w};
                            vertices[i].tangent = tan;
                            ++i;
                        }
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

                // Validate index buffer: count must be a multiple of 3 and
                // every index must be within the vertex buffer range.
                if (indices.size() % 3 != 0) {
                    spdlog::warn("Mesh '{}': index count {} not a multiple of 3, truncating",
                                 std::string(gltf_mesh.name), indices.size());
                    indices.resize(indices.size() - indices.size() % 3);
                }
                const auto last_vertex = static_cast<uint32_t>(vertex_count - 1);
                size_t invalid_index_count = 0;
                for (auto &idx : indices) {
                    if (idx >= vertex_count) {
                        ++invalid_index_count;
                        idx = last_vertex;
                    }
                }
                if (invalid_index_count > 0) {
                    spdlog::warn("Mesh '{}': {} invalid indices clamped to last vertex "
                                 "(vertex count {})",
                                 std::string(gltf_mesh.name), invalid_index_count,
                                 vertex_count);
                }
                if (indices.empty()) {
                    spdlog::warn("Mesh '{}' primitive skipped: empty (0 triangles)",
                                 std::string(gltf_mesh.name));
                    continue;
                }

                // §1382: when normals are absent, compute flat normals.
                // Unindex first so each triangle owns its vertices — shared
                // vertices would get overwritten by the last triangle's normal.
                if (!has_normals && !indices.empty()) {
                    std::vector<renderer::Vertex> unindexed(indices.size());
                    for (size_t i = 0; i < indices.size(); ++i) {
                        unindexed[i] = vertices[indices[i]];
                    }
                    vertices = std::move(unindexed);
                    vertex_count = vertices.size();
                    indices.resize(vertex_count);
                    std::iota(indices.begin(), indices.end(), 0u);

                    for (size_t tri = 0; tri + 2 < vertex_count; tri += 3) {
                        const glm::vec3 face_n = glm::cross(
                            vertices[tri + 1].position - vertices[tri].position,
                            vertices[tri + 2].position - vertices[tri].position);
                        const float len2 = glm::dot(face_n, face_n);
                        const glm::vec3 n = len2 > 1e-12f
                            ? face_n / std::sqrt(len2)
                            : glm::vec3(0.0f, 0.0f, 1.0f);
                        vertices[tri].normal = n;
                        vertices[tri + 1].normal = n;
                        vertices[tri + 2].normal = n;
                    }
                    has_normals = true;
                }

                // §1384: vertices of the same triangle SHOULD have the same
                // tangent.w. Mixed handedness makes interpolated w meaningless.
                if (has_tangent) {
                    for (size_t tri = 0; tri + 2 < indices.size(); tri += 3) {
                        const float w0 = vertices[indices[tri]].tangent.w;
                        const float w1 = vertices[indices[tri + 1]].tangent.w;
                        const float w2 = vertices[indices[tri + 2]].tangent.w;
                        if (w0 != w1 || w1 != w2) {
                            has_tangent = false;
                            break;
                        }
                    }
                }

                // Generate tangents via MikkTSpace if missing or invalid
                // (needs normal + uv0).
                if (!has_tangent && has_normals && has_uv0) {
                    generate_tangents(vertices, indices);
                }

                optix::CudaBuffer<renderer::Vertex> vb;
                vb.alloc(vertices.size());
                vb.upload(vertices.data(), vertices.size(), stream);

                optix::CudaBuffer<uint32_t> ib;
                ib.alloc(indices.size());
                ib.upload(indices.data(), indices.size(), stream);

                // glTF spec: undefined material → default material (opaque, white,
                // metallic 1, roughness 1). The default entry is appended after the
                // explicit materials at index gltf.materials.size().
                const auto default_material_idx = static_cast<uint32_t>(gltf.materials.size());
                const auto prim_material_id = primitive.materialIndex.has_value()
                    ? static_cast<uint32_t>(*primitive.materialIndex)
                    : default_material_idx;

                const bool prim_opaque = !primitive.materialIndex.has_value()
                    || gltf.materials[prim_material_id].alphaMode == fastgltf::AlphaMode::Opaque;

                meshes_.push_back({
                    .vertex_buffer = std::move(vb),
                    .index_buffer = std::move(ib),
                    .vertex_count = static_cast<uint32_t>(vertices.size()),
                    .index_count = static_cast<uint32_t>(indices.size()),
                    .group_id = static_cast<uint32_t>(mesh_idx),
                    .material_id = prim_material_id,
                    .opaque = prim_opaque,
                });

                result.material_ids.push_back(prim_material_id);
                result.local_bounds.push_back({local_min, local_max});
                result.cpu_vertices.push_back(std::move(vertices));
                result.cpu_indices.push_back(std::move(indices));
            }
        }

        // Sentinel for the last mesh's primitive range
        result.prim_offsets.push_back(static_cast<uint32_t>(meshes_.size()));

        spdlog::info("Loaded {} mesh primitives", meshes_.size());
        return result;
    }

    void SceneLoader::load_materials(const fastgltf::Asset &gltf,
                                     const optix::DefaultTextures &default_textures,
                                     const cudaStream_t stream) {
        // ---- Default texture indices (reserved at the front of texture_objects_) ----

        texture_objects_.push_back(default_textures.white.texture_object);
        texture_objects_.push_back(default_textures.flat_normal.texture_object);
        texture_objects_.push_back(default_textures.black.texture_object);

        // ---- Collect unique (texture_index, role) pairs ----
        using TexKey = std::pair<size_t, TextureRole>;
        std::map<TexKey, size_t> unique_tex_map;

        struct TexEntry {
            size_t texture_index;
            TextureRole role;
        };
        std::vector<TexEntry> unique_entries;

        for (const auto &mat : gltf.materials) {
            const auto &pbr = mat.pbrData;
            auto collect = [&](const auto &opt_tex, const TextureRole role) {
                if (!opt_tex.has_value()) { return; }
                const auto key = std::make_pair(opt_tex->textureIndex, role);
                if (unique_tex_map.contains(key)) { return; }
                unique_tex_map[key] = unique_entries.size();
                unique_entries.push_back({opt_tex->textureIndex, role});
            };
            collect(pbr.baseColorTexture, TextureRole::Color);
            collect(pbr.metallicRoughnessTexture, TextureRole::Linear);
            collect(mat.normalTexture, TextureRole::Normal);
            collect(mat.emissiveTexture, TextureRole::Color);
        }

        // ---- Hash source bytes + cache check (serial, fast) ----
        const auto tex_count = static_cast<int>(unique_entries.size());

        std::vector<std::string> source_hashes(tex_count);
        std::vector<PreparedTexture> prepared_textures(tex_count);
        std::vector cache_hit(tex_count, false);

        for (int i = 0; i < tex_count; ++i) {
            const auto &tex = gltf.textures[unique_entries[i].texture_index];
            assert(tex.imageIndex.has_value() && "glTF texture must have an image source");
            source_hashes[i] = hash_gltf_image(gltf, gltf.images[*tex.imageIndex]);
            if (auto cached = load_cached_texture(
                    source_hashes[i], unique_entries[i].role)) {
                prepared_textures[i] = std::move(*cached);
                cache_hit[i] = true;
            }
        }

        // ---- Deduplicate entries sharing the same source image + role ----
        // Different glTF texture objects can reference the same image (different
        // samplers). Without dedup they would race on the same cache .tmp path
        // during parallel compression.
        std::vector<int> dedup_source(tex_count, -1);
        {
            std::map<std::pair<std::string, TextureRole>, int> seen;
            for (int i = 0; i < tex_count; ++i) {
                if (cache_hit[i]) { continue; }
                auto key = std::make_pair(source_hashes[i], unique_entries[i].role);
                if (auto it = seen.find(key); it != seen.end()) {
                    dedup_source[i] = it->second;
                } else {
                    seen[key] = i;
                }
            }
        }

        // ---- Decode cache-miss images (serial) ----
        std::vector<ImageData> decoded_images(tex_count);
        for (int i = 0; i < tex_count; ++i) {
            if (cache_hit[i] || dedup_source[i] >= 0) { continue; }
            const auto &tex = gltf.textures[unique_entries[i].texture_index];
            decoded_images[i] = decode_gltf_image(gltf, gltf.images[*tex.imageIndex]);
        }

        // ---- Parallel BC compression for cache misses only ----
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < tex_count; ++i) {
            if (cache_hit[i] || dedup_source[i] >= 0) { continue; }
            prepared_textures[i] = compress_texture(
                decoded_images[i], unique_entries[i].role, source_hashes[i]);
        }

        // Propagate results to duplicate entries.
        for (int i = 0; i < tex_count; ++i) {
            if (dedup_source[i] >= 0) {
                prepared_textures[i] = prepared_textures[dedup_source[i]];
            }
        }

        decoded_images.clear();

        // ---- Serial GPU upload ----
        constexpr optix::SamplerDesc default_sampler{
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

            auto cuda_texture = optix::finalize_texture(prepared_textures[i], sampler);
            const auto obj_index = static_cast<uint32_t>(texture_objects_.size());
            texture_objects_.push_back(cuda_texture.texture_object);
            textures_.push_back(std::move(cuda_texture));
            tex_index_cache[{entry.texture_index, entry.role}] = obj_index;
        }
        prepared_textures.clear();

        // ---- Fill materials ----
        auto resolve_texture = [&](const size_t texture_index,
                                   const TextureRole role) -> uint32_t {
            const auto it = tex_index_cache.find({texture_index, role});
            assert(it != tex_index_cache.end() && "Texture must have been prepared");
            return it->second;
        };

        gpu_materials_.reserve(gltf.materials.size() + 1);

        for (const auto &mat : gltf.materials) {
            constexpr uint32_t kDefaultWhiteIdx = 0;
            renderer::Material data{};
            // ReSharper disable once CppUseStructuredBinding
            const auto &pbr = mat.pbrData;

            // Clamp material factors to glTF 2.0 schema ranges.
            // Non-finite values fall back to the schema default.
            data.base_color_factor = {
                spec_clamp(pbr.baseColorFactor[0], 0.0f, 1.0f, 1.0f),
                spec_clamp(pbr.baseColorFactor[1], 0.0f, 1.0f, 1.0f),
                spec_clamp(pbr.baseColorFactor[2], 0.0f, 1.0f, 1.0f),
                spec_clamp(pbr.baseColorFactor[3], 0.0f, 1.0f, 1.0f),
            };
            // NOTE: if KHR_materials_emissive_strength is added, multiply
            // emissiveFactor by emissiveStrength here and widen the clamp range.
            // emissive_alias_table.cpp total_power cast will also need overflow
            // protection — see the NOTE there.
            data.emissive_factor = {
                spec_clamp(mat.emissiveFactor[0], 0.0f, 1.0f, 0.0f),
                spec_clamp(mat.emissiveFactor[1], 0.0f, 1.0f, 0.0f),
                spec_clamp(mat.emissiveFactor[2], 0.0f, 1.0f, 0.0f),
                0.0f
            };
            data.metallic_factor = spec_clamp(pbr.metallicFactor, 0.0f, 1.0f, 1.0f);
            data.roughness_factor = spec_clamp(pbr.roughnessFactor, 0.0f, 1.0f, 1.0f);
            data.normal_scale = mat.normalTexture.has_value()
                                    ? ensure_finite(mat.normalTexture->scale, 1.0f) : 1.0f;
            data.alpha_cutoff = std::max(ensure_finite(mat.alphaCutoff, 0.5f), 0.0f);
            data.alpha_mode = static_cast<uint32_t>(convert_alpha_mode(mat.alphaMode));
            data.double_sided = mat.doubleSided ? 1u : 0u;

            // Texture references (UINT32_MAX → default fallback below)
            data.base_color_tex = pbr.baseColorTexture.has_value()
                                      ? resolve_texture(pbr.baseColorTexture->textureIndex,
                                                        TextureRole::Color)
                                      : UINT32_MAX;
            data.metallic_roughness_tex = pbr.metallicRoughnessTexture.has_value()
                                              ? resolve_texture(
                                                    pbr.metallicRoughnessTexture->textureIndex,
                                                    TextureRole::Linear)
                                              : UINT32_MAX;
            data.normal_tex = mat.normalTexture.has_value()
                                  ? resolve_texture(mat.normalTexture->textureIndex,
                                                    TextureRole::Normal)
                                  : UINT32_MAX;
            data.emissive_tex = mat.emissiveTexture.has_value()
                                    ? resolve_texture(mat.emissiveTexture->textureIndex,
                                                      TextureRole::Color)
                                    : UINT32_MAX;

            if (data.base_color_tex == UINT32_MAX) { data.base_color_tex = kDefaultWhiteIdx; }
            if (data.metallic_roughness_tex == UINT32_MAX) { data.metallic_roughness_tex = kDefaultWhiteIdx; }
            if (data.normal_tex == UINT32_MAX) {
                constexpr uint32_t kDefaultFlatNormalIdx = 1;
                data.normal_tex = kDefaultFlatNormalIdx;
            }
            if (data.emissive_tex == UINT32_MAX) { data.emissive_tex = kDefaultWhiteIdx; }

            gpu_materials_.push_back(data);
        }

        // Append the glTF default material (spec §Default Material: all fields
        // at schema defaults). Used when a primitive omits materialIndex.
        {
            constexpr uint32_t kDefaultWhiteIdx = 0;
            constexpr uint32_t kDefaultFlatNormalIdx = 1;
            renderer::Material def{};
            def.base_color_factor = {1.0f, 1.0f, 1.0f, 1.0f};
            def.emissive_factor = {0.0f, 0.0f, 0.0f, 0.0f};
            def.metallic_factor = 1.0f;
            def.roughness_factor = 1.0f;
            def.normal_scale = 1.0f;
            def.base_color_tex = kDefaultWhiteIdx;
            def.emissive_tex = kDefaultWhiteIdx;
            def.metallic_roughness_tex = kDefaultWhiteIdx;
            def.normal_tex = kDefaultFlatNormalIdx;
            def.alpha_cutoff = 0.5f;
            def.alpha_mode = 0u;
            def.double_sided = 0u;
            gpu_materials_.push_back(def);
        }

        material_buffer_.alloc(gpu_materials_.size());
        material_buffer_.upload(gpu_materials_.data(), gpu_materials_.size(), stream);

        if (!texture_objects_.empty()) {
            texture_objects_buffer_.alloc(texture_objects_.size());
            texture_objects_buffer_.upload(texture_objects_.data(), texture_objects_.size(), stream);
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

                // Validate transform: all elements must be finite and the
                // matrix must be non-singular (invertible for normal transform).
                {
                    bool finite = true;
                    for (int col = 0; col < 4 && finite; ++col) {
                        for (int row = 0; row < 4 && finite; ++row) {
                            if (!std::isfinite(world_mat[col][row])) { finite = false; }
                        }
                    }
                    if (!finite || glm::abs(glm::determinant(world_mat)) < 1e-12f) {
                        spdlog::warn("Node '{}': non-finite or singular transform, skipping",
                                     std::string(node.name));
                        return;
                    }
                }

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

    // ---- Environment map ----

    bool SceneLoader::load_env_map(const std::string &path, const cudaStream_t stream) {
        destroy_env_map();

        if (path.empty()) {
            return false;
        }

        // --- Load HDR pixels (needed for both cubemap and alias table) ---
        auto hdr = load_hdr_image(std::filesystem::path(path));
        if (!hdr.valid()) {
            return false;
        }

        const auto equirect_w = hdr.width;
        const auto equirect_h = hdr.height;

        // --- Cubemap: cache check → equirect→cubemap → BC6H → finalize ---
        const auto source_hash = content_hash(std::filesystem::path(path));
        auto cached = source_hash.empty()
                          ? std::nullopt
                          : load_cached_texture_bc6h(source_hash);

        optix::CudaTexture cubemap;
        if (cached) {
            spdlog::info("Env cubemap loaded from cache");
            constexpr optix::SamplerDesc cubemap_sampler{
                .filter_mode = cudaFilterModeLinear,
                .mipmap_filter_mode = cudaFilterModePoint,
                .address_mode_u = cudaAddressModeClamp,
                .address_mode_v = cudaAddressModeClamp,
            };
            cubemap = optix::finalize_texture(*cached, cubemap_sampler);
        } else {
            // Equirect → cubemap (CUDA kernel, fp16 RGBA output)
            uint32_t face_size = 0;
            auto cubemap_pixels = equirect_to_cubemap(
                hdr.pixels.get(), equirect_w, equirect_h, face_size);
            if (cubemap_pixels.empty()) {
                spdlog::error("Env cubemap: equirect-to-cubemap conversion failed");
                return false;
            }

            // BC6H compression (CPU ISPC, writes KTX2 cache)
            auto prepared = compress_texture_bc6h(cubemap_pixels, face_size, source_hash);

            constexpr optix::SamplerDesc cubemap_sampler{
                .filter_mode = cudaFilterModeLinear,
                .mipmap_filter_mode = cudaFilterModePoint,
                .address_mode_u = cudaAddressModeClamp,
                .address_mode_v = cudaAddressModeClamp,
            };
            cubemap = optix::finalize_texture(prepared, cubemap_sampler);
        }

        // --- Alias table (always from raw HDR pixels, no disk cache) ---
        auto alias_result = build_env_alias_table(
            hdr.pixels.get(), equirect_w, equirect_h);
        if (alias_result.entries.empty()) {
            spdlog::error("Env alias table construction failed");
            cubemap.destroy();
            return false;
        }

        // Upload alias table to device
        env_alias_table_.alloc(alias_result.entries.size());
        env_alias_table_.upload(alias_result.entries.data(),
                                alias_result.entries.size(), stream);

        // Ensure the async copy completes before alias_result is destroyed.
        CUDA_CHECK(cudaStreamSynchronize(stream));

        // Commit all resources
        env_cubemap_texture_ = std::move(cubemap);
        env_alias_width_ = alias_result.alias_width;
        env_alias_height_ = alias_result.alias_height;
        env_source_width_ = equirect_w;
        env_source_height_ = equirect_h;
        env_total_luminance_ = alias_result.total_luminance;

        spdlog::info("Env map ready: cubemap tex={}, alias table {}x{} ({} entries)",
                     env_cubemap_texture_.texture_object,
                     alias_result.alias_width, alias_result.alias_height,
                     alias_result.entries.size());
        return true;
    }

    void SceneLoader::destroy_env_map() {
        env_cubemap_texture_.destroy();
        env_alias_table_.free();
        env_alias_width_ = 0;
        env_alias_height_ = 0;
        env_source_width_ = 0;
        env_source_height_ = 0;
        env_total_luminance_ = 0.0f;
    }

    // ---- Destroy + accessors ----

    void SceneLoader::destroy() {
        destroy_env_map();

        emissive_triangles_.free();
        emissive_alias_table_.free();
        emissive_total_power_ = 0.0f;

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

    uint32_t SceneLoader::scene_texture_count() const {
        // texture_objects_ contains 3 default textures at the front followed
        // by scene textures. textures_ holds the CudaTexture objects for
        // scene textures only.
        return static_cast<uint32_t>(textures_.size());
    }

    const renderer::AABB &SceneLoader::scene_bounds() const {
        return scene_bounds_;
    }

    renderer::EnvLightData SceneLoader::env_light() const {
        return renderer::EnvLightData{
            .cubemap = env_cubemap_texture_.texture_object,
            .alias_table = env_alias_table_.data(),
            .alias_count = static_cast<uint32_t>(env_alias_table_.count()),
            .alias_width = env_alias_width_,
            .alias_height = env_alias_height_,
            .total_luminance = env_total_luminance_,
        };
    }

    renderer::EmissiveLightData SceneLoader::emissive_light() const {
        return renderer::EmissiveLightData{
            .triangles = emissive_triangles_.data(),
            .alias_table = emissive_alias_table_.data(),
            .count = static_cast<uint32_t>(emissive_triangles_.count()),
            .total_power = emissive_total_power_,
        };
    }
} // namespace qualquer::app
