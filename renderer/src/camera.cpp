/**
 * @file camera.cpp
 * @brief Camera implementation (renderer layer).
 */

#include <qualquer/renderer/camera.h>
#include <qualquer/renderer/scene_types.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

namespace qualquer::renderer {

    void Camera::update_view() {
        const glm::vec3 fwd = forward();
        view = glm::lookAt(position, position + fwd, glm::vec3(0.0f, 1.0f, 0.0f));
        inv_view = glm::inverse(view);
    }

    void Camera::update_projection() {
        // Standard perspective, no reverse-Z: Qualquer is a pure path tracer with
        // no depth buffer, so reverse-Z protects nothing; raygen only unprojects
        // for direction, which is independent of the depth convention.
        projection = glm::perspective(fov, aspect, near_plane, far_plane);
        inv_projection = glm::inverse(projection);
    }

    void Camera::update_all() {
        update_view();
        update_projection();
    }

    glm::vec3 Camera::forward() const {
        // yaw=0, pitch=0 => (0, 0, -1), looking along -Z.
        return {
            std::sin(yaw) * std::cos(pitch),
            std::sin(pitch),
            -std::cos(yaw) * std::cos(pitch)
        };
    }

    glm::vec3 Camera::right() const {
        // Always horizontal (no roll); yaw=0 => (1, 0, 0).
        return {
            std::cos(yaw),
            0.0f,
            std::sin(yaw)
        };
    }

    glm::vec3 Camera::compute_focus_position(const AABB &bounds) const {
        const glm::vec3 center = (bounds.min + bounds.max) * 0.5f;
        const float diagonal = glm::length(bounds.max - bounds.min);

        if (constexpr float kEpsilon = 1e-4f; diagonal < kEpsilon) {
            return position;
        }

        const float radius = diagonal * 0.5f;
        // Use the tighter of vertical and horizontal FOV so the bounding sphere
        // fits in both dimensions (matters for narrow viewports).
        const float half_v = fov * 0.5f;
        const float half_h = std::atan(std::tan(half_v) * aspect);
        const float distance = radius / std::sin(std::min(half_v, half_h));
        return center - forward() * distance;
    }

} // namespace qualquer::renderer
