/**
 * @file mesh.cpp
 * @brief MikkTSpace tangent generation (renderer layer).
 */

#include <qualquer/renderer/mesh.h>

#include <mikktspace.h>

namespace qualquer::renderer {
    namespace {
        // Bundles vertex and index spans for MikkTSpace callbacks (pUserData).
        struct MikkUserData {
            std::span<Vertex> vertices;
            std::span<const uint32_t> indices;
        };

        int mikk_get_num_faces(const SMikkTSpaceContext *ctx) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            return static_cast<int>(data->indices.size() / 3);
        }

        int mikk_get_num_vertices_of_face(const SMikkTSpaceContext *, int) {
            return 3;
        }

        void mikk_get_position(const SMikkTSpaceContext *ctx, float out[], const int face, const int vert) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            const auto &pos = data->vertices[data->indices[face * 3 + vert]].position;
            out[0] = pos.x;
            out[1] = pos.y;
            out[2] = pos.z;
        }

        void mikk_get_normal(const SMikkTSpaceContext *ctx, float out[], const int face, const int vert) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            const auto &n = data->vertices[data->indices[face * 3 + vert]].normal;
            out[0] = n.x;
            out[1] = n.y;
            out[2] = n.z;
        }

        void mikk_get_tex_coord(const SMikkTSpaceContext *ctx, float out[], const int face, const int vert) {
            const auto *data = static_cast<const MikkUserData *>(ctx->m_pUserData);
            const auto &uv = data->vertices[data->indices[face * 3 + vert]].uv0;
            out[0] = uv.x;
            out[1] = uv.y;
        }

        void mikk_set_tspace_basic(const SMikkTSpaceContext *ctx,
                                   const float tangent[],
                                   const float sign,
                                   const int face,
                                   const int vert) {
            auto *data = static_cast<MikkUserData *>(ctx->m_pUserData);
            auto &t = data->vertices[data->indices[face * 3 + vert]].tangent;
            t.x = tangent[0];
            t.y = tangent[1];
            t.z = tangent[2];
            t.w = sign;
        }
    } // anonymous namespace

    void generate_tangents(const std::span<Vertex> vertices, const std::span<const uint32_t> indices) {
        MikkUserData user_data{vertices, indices};

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

        genTangSpaceDefault(&ctx);
    }
} // namespace qualquer::renderer
