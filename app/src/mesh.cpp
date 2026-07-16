/**
 * @file mesh.cpp
 * @brief MikkTSpace tangent generation (app layer).
 */

#include <qualquer/app/mesh.h>

#include <mikktspace.h>
#include <spdlog/spdlog.h>

#include <unordered_map>
#include <vector>

namespace qualquer::app {
    using renderer::Vertex;

    namespace {
        // MikkTSpace callback context: reads from vertices/indices, writes
        // per-face-corner tangents to a flat array (not back to indexed vertices).
        struct MikkUserData {
            const std::vector<Vertex> *vertices;
            const std::vector<uint32_t> *indices;
            std::vector<glm::vec4> *corner_tangents;  // size = indices.size()
        };

        int mikk_get_num_faces(const SMikkTSpaceContext *ctx) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            return static_cast<int>(data->indices->size() / 3);
        }

        int mikk_get_num_vertices_of_face(const SMikkTSpaceContext *, int) {
            return 3;
        }

        void mikk_get_position(const SMikkTSpaceContext *ctx, float out[], const int face, const int vert) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            const auto &pos = (*data->vertices)[(*data->indices)[face * 3 + vert]].position;
            out[0] = pos.x;
            out[1] = pos.y;
            out[2] = pos.z;
        }

        void mikk_get_normal(const SMikkTSpaceContext *ctx, float out[], const int face, const int vert) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            const auto &n = (*data->vertices)[(*data->indices)[face * 3 + vert]].normal;
            out[0] = n.x;
            out[1] = n.y;
            out[2] = n.z;
        }

        void mikk_get_tex_coord(const SMikkTSpaceContext *ctx, float out[], const int face, const int vert) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            const auto &uv = (*data->vertices)[(*data->indices)[face * 3 + vert]].uv0;
            out[0] = uv.x;
            out[1] = uv.y;
        }

        // Write to per-face-corner array, NOT back to indexed vertex.
        // mikktspace.h:87-88: "averaging/overwriting tangent spaces by using
        // an already existing index list WILL produce INCRORRECT results.
        // DO NOT! use an already existing index list."
        void mikk_set_tspace_basic(const SMikkTSpaceContext *ctx,
                                   const float tangent[],
                                   const float sign,
                                   const int face,
                                   const int vert) {
            auto *data = static_cast<MikkUserData *>(ctx->m_pUserData);
            (*data->corner_tangents)[face * 3 + vert] = {tangent[0], tangent[1], tangent[2], sign};
        }
    } // anonymous namespace

    void generate_tangents(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
        const auto num_corners = indices.size();
        std::vector<glm::vec4> corner_tangents(num_corners);

        MikkUserData user_data{&vertices, &indices, &corner_tangents};

        SMikkTSpaceInterface iface{};
        iface.m_getNumFaces = mikk_get_num_faces;
        iface.m_getNumVerticesOfFace = mikk_get_num_vertices_of_face;
        iface.m_getPosition = mikk_get_position;
        iface.m_getNormal = mikk_get_normal;
        iface.m_getTexCoord = mikk_get_tex_coord;
        iface.m_setTSpaceBasic = mikk_set_tspace_basic;

        SMikkTSpaceContext ctx{};
        ctx.m_pInterface = &iface;
        ctx.m_pUserData = &user_data;

        if (!genTangSpaceDefault(&ctx)) {
            spdlog::warn("MikkTSpace: genTangSpaceDefault failed, tangents left at default");
            return;
        }

        // Assign per-face-corner tangents back to vertices, splitting shared
        // vertices that received different tangents from different triangles.
        //
        // For each original vertex index, track the first tangent assigned.
        // Subsequent face-corners sharing the same index with a matching tangent
        // reuse it; mismatches duplicate the vertex with the new tangent.

        // Maps original_index → list of (tangent, new_vertex_index) pairs.
        std::unordered_map<uint32_t, std::vector<std::pair<glm::vec4, uint32_t>>> assigned;
        uint32_t splits = 0;

        for (size_t c = 0; c < num_corners; ++c) {
            const uint32_t orig_idx = indices[c];
            const glm::vec4 &tang = corner_tangents[c];

            auto &entries = assigned[orig_idx];

            // Check if this tangent already exists for this vertex.
            uint32_t target_idx = UINT32_MAX;
            for (const auto &[t, idx] : entries) {
                if (t == tang) {
                    target_idx = idx;
                    break;
                }
            }

            if (target_idx == UINT32_MAX) {
                if (entries.empty()) {
                    // First assignment: write tangent to existing vertex.
                    vertices[orig_idx].tangent = tang;
                    entries.push_back({tang, orig_idx});
                    target_idx = orig_idx;
                } else {
                    // Different tangent on shared vertex: duplicate.
                    target_idx = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertices[orig_idx]);
                    vertices.back().tangent = tang;
                    entries.push_back({tang, target_idx});
                    ++splits;
                }
            }

            indices[c] = target_idx;
        }

        if (splits > 0) {
            spdlog::info("MikkTSpace: split {} vertices at tangent discontinuities "
                         "({} → {} vertices)", splits, vertices.size() - splits, vertices.size());
        }
    }
} // namespace qualquer::app
