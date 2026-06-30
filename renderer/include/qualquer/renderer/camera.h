#pragma once

/**
 * @file camera.h
 * @brief Camera state and view/projection matrices (renderer layer).
 */

#include <glm/glm.hpp>

namespace qualquer::renderer {
    struct AABB;

    /**
     * @brief Camera state: orientation/projection inputs and derived matrices.
     *
     * Stores input state (position, yaw/pitch, fov, near/far, aspect) and derived
     * state (view, projection, and their inverses). Modify input fields directly,
     * then call the update methods to recompute the matrices. The inverses are
     * what the raygen program consumes to unproject pixels into world-space rays.
     *
     * Coordinate system: right-handed, Y-up. yaw = 0 looks along -Z.
     */
    struct Camera {
        // --- Input state (modify directly, then call update methods) ---

        /** @brief World-space position. */
        glm::vec3 position{0.0f, 0.0f, 3.0f};

        /** @brief Horizontal rotation in radians (0 = looking along -Z). */
        float yaw = 0.0f;

        /** @brief Vertical rotation in radians (0 = horizontal, positive = up). */
        float pitch = 0.0f;

        /** @brief Vertical field of view in radians. */
        float fov = glm::radians(60.0f);

        /** @brief Distance to the near plane, in world units. */
        float near_plane = 0.1f;

        /** @brief Distance to the far plane, in world units. */
        float far_plane = 1000.0f;

        /** @brief Viewport aspect ratio (width / height). */
        float aspect = 16.0f / 9.0f;

        // --- Derived state (computed by update methods) ---

        /** @brief View matrix (world to view space). */
        glm::mat4 view{1.0f};

        /** @brief Inverse view matrix (view to world space). */
        glm::mat4 inv_view{1.0f};

        /** @brief Perspective projection matrix (view to clip space). */
        glm::mat4 projection{1.0f};

        /** @brief Inverse projection matrix (clip to view space). */
        glm::mat4 inv_projection{1.0f};

        // --- Update methods ---

        /** @brief Recomputes view and inv_view from position, yaw, and pitch. */
        void update_view();

        /** @brief Recomputes projection and inv_projection from fov, aspect, near, far. */
        void update_projection();

        /** @brief Convenience: update_view() + update_projection(). */
        void update_all();

        // --- Direction helpers ---

        /** @brief Forward direction derived from yaw and pitch. */
        [[nodiscard]] glm::vec3 forward() const;

        /** @brief Right direction (always horizontal, perpendicular to forward). */
        [[nodiscard]] glm::vec3 right() const;

        // --- Focus helpers ---

        /**
         * @brief Computes a position that frames the given AABB.
         *
         * Uses current yaw, pitch, and fov to find the distance fitting the AABB's
         * bounding sphere, taking the tighter of vertical/horizontal FOV. Does not
         * modify camera state. Returns the current position if the AABB is
         * degenerate (diagonal ~ 0).
         */
        [[nodiscard]] glm::vec3 compute_focus_position(const AABB &bounds) const;
    };
} // namespace qualquer::renderer
