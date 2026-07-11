#pragma once

/**
 * @file camera_controller.h
 * @brief Free-roaming camera controller (app layer).
 */

struct GLFWwindow;

namespace qualquer::renderer {
    struct AABB;
    struct Camera;
}

namespace qualquer::app {
    /**
     * @brief Free-roaming camera controller: WASD movement + mouse rotation.
     *
     * Hold right mouse button to rotate (cursor hidden, raw motion enabled). WASD
     * moves along the camera's forward/right, Space ascends, Ctrl descends, Shift
     * sprints (3x). F frames the focus-target AABB, keeping the current viewing
     * direction. Call update() once per frame after glfwPollEvents().
     */
    class CameraController {
    public:
        /**
         * @brief Binds the controller to a window and camera.
         * @param window GLFW window for input polling.
         * @param camera Camera to control (must outlive the controller).
         */
        void init(GLFWwindow *window, renderer::Camera *camera);

        /**
         * @brief Processes input and updates camera matrices.
         * @param delta_time Frame delta time in seconds.
         */
        void update(float delta_time);

        /**
         * @brief Sets the AABB used by F-key focus (nullptr = focus disabled).
         *
         * The pointer must remain valid for the lifetime of the controller;
         * typically points to the scene bounds.
         */
        void set_focus_target(const renderer::AABB *bounds);

        /** @brief Movement speed in world units per second. */
        float move_speed = 5.0f;

        /** @brief Mouse rotation sensitivity in radians per pixel. */
        float mouse_sensitivity = 0.003f;

    private:
        /** @brief GLFW window for input polling. */
        GLFWwindow *window_ = nullptr;

        /** @brief Camera being controlled. */
        renderer::Camera *camera_ = nullptr;

        /** @brief Previous cursor X (for delta calculation). */
        double last_cursor_x_ = 0.0;

        /** @brief Previous cursor Y (for delta calculation). */
        double last_cursor_y_ = 0.0;

        /** @brief Whether the right mouse button was held last frame. */
        bool right_button_held_ = false;

        /** @brief Focus target AABB for F-key (nullptr = disabled). */
        const renderer::AABB *focus_target_ = nullptr;

    public:
        /**
         * @brief True on the frame F-key focus teleports the camera.
         *
         * Set by update(), consumed (cleared) by the caller after reading.
         * Used to signal DLSS InReset (teleport invalidates temporal history).
         */
        bool teleported = false;
    };
} // namespace qualquer::app
