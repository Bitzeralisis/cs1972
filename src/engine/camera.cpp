#include "camera.h"

using namespace CS1972Engine;

Camera::Camera(Mode m, glm::vec3 pos, float y, float p, float tpd, float fov, float ar, float n, float f)
    : m_mode(m)
    , m_position(pos)
    , m_yaw(y)
    , m_pitch(p)
    , m_tpdistance(tpd)
    , m_fovy(fov)
    , m_aspect(ar)
    , m_near(n)
    , m_far(f)

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
    glm::vec2 eye(glm::cos(m_yaw), glm::sin(m_yaw));
    glm::mat2 m(eye.x, eye.y, -1.f*eye.y, eye.x);

    // Rotate v using m
    glm::vec2 w = m*v;

    // Move
    m_position += glm::vec3(w.x, 0.f, w.y);
}

glm::mat4 Camera::viewMatrix() {
    // Normalize yaw and pitch
    while (m_yaw > glm::pi<float>()) m_yaw -= 2.f*glm::pi<float>();
    while (m_yaw <= -1.f*glm::pi<float>()) m_yaw += 2.f*glm::pi<float>();
    if (m_pitch > glm::half_pi<float>()) m_pitch = glm::half_pi<float>();
    else if (m_pitch < -1.f*glm::half_pi<float>()) m_pitch = -1.f*glm::half_pi<float>();

    // Make center and up vectors
    glm::vec3 pos = m_position;
    glm::vec3 look(
        glm::cos(m_yaw) * glm::cos(m_pitch),
        glm::sin(m_pitch),
        glm::sin(m_yaw) * glm::cos(m_pitch)
    );
    look = glm::normalize(look);
    if (m_mode == Mode::THIRD_PERSON) {
        pos -= m_tpdistance*look;
    }
    glm::vec3 center = pos + look;
    glm::vec3 up(0.f, 1.f, 0.f);
    if (m_pitch == glm::half_pi<float>()) {
        up = glm::vec3(-1.f*glm::cos(m_yaw), 0.f, 1.f*glm::sin(m_yaw));
        up = glm::normalize(up);
    } else if (m_pitch == -1.f*glm::half_pi<float>()) {
        up = glm::vec3(glm::cos(m_yaw), 0.f, glm::sin(m_yaw));
        up = glm::normalize(up);
    }

    return glm::lookAt(pos, center, up);
}

glm::mat4 Camera::perspectiveMatrix() {
    return glm::perspective(m_fovy, m_aspect, m_near, m_far);
}
