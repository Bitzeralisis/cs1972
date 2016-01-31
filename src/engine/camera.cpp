#include "camera.h"

using namespace CS1972Engine;

Camera::Camera() { }

Camera::Camera(float fov, float ar, float n, float f)
    : fovy(fov)
    , aspect(ar)
    , near(n)
    , far(f)
{ }

/**
 * @brief Camera::walk
 * Move the camera by dir, but only in the xz (floor) plane, and rotated such that (1, 0, 0) is in the direction of the eye.
 * @param dir
 */
void Camera::walk(glm::vec3 dir) {
    // Take xz components of dir as a vector in a 2d plane
    glm::vec2 v(dir.x, dir.z);

    // Build rotation matrix in xz plane based on yaw
    glm::vec2 eye(glm::cos(yaw), glm::sin(yaw));
    glm::mat2 m(eye.x, eye.y, -1.f*eye.y, eye.x);

    // Rotate v using m
    glm::vec2 w = m*v;

    // Move
    position += glm::vec3(w.x, 0.f, w.y);
}

glm::mat4 Camera::viewMatrix() {
    // Normalize yaw and pitch
    while (yaw > glm::pi<float>()) yaw -= glm::pi<float>();
    while (yaw <= -1.f*glm::pi<float>()) yaw += glm::pi<float>();
    if (pitch > glm::half_pi<float>()) pitch = glm::half_pi<float>();
    else if (pitch < -1.f*glm::half_pi<float>()) pitch = -1.f*glm::half_pi<float>();

    // Make center and up vectors
    glm::vec3 look(
        glm::cos(yaw) * glm::cos(pitch),
        glm::sin(pitch),
        glm::sin(yaw) * glm::cos(pitch)
    );
    look = glm::normalize(look);
    glm::vec3 center = position + look;
    glm::vec3 up(0.f, 1.f, 0.f);
    if (pitch == glm::half_pi<float>() || pitch == -1.f*glm::half_pi<float>()) {
        up = glm::vec3(look.x, 1.f, look.z);
        up = glm::normalize(up);
    }

    return glm::lookAt(position, center, up);
}

glm::mat4 Camera::perspectiveMatrix() {
    return glm::perspective(fovy, aspect, near, far);
}
