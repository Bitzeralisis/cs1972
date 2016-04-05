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

void Camera::yaw(float yaw) {
    m_yaw = yaw;
    while (m_yaw > glm::pi<float>()) m_yaw -= 2.f*glm::pi<float>();
    while (m_yaw <= -1.f*glm::pi<float>()) m_yaw += 2.f*glm::pi<float>();
}

void Camera::pitch(float pitch) {
    m_pitch = pitch;
    if (m_pitch > glm::half_pi<float>()) m_pitch = glm::half_pi<float>();
    else if (m_pitch < -1.f*glm::half_pi<float>()) m_pitch = -1.f*glm::half_pi<float>();
}

/**
 * @brief Camera::walk
 * Move the camera by dir, but only in the xz (floor) plane, and rotated such that (1, 0, 0) is in the direction of the eye.
 * @param dir
 */
void Camera::walk(const glm::vec3& dir) {
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

glm::vec3 Camera::lookVector() const {
    glm::vec3 look(
        glm::cos(m_yaw) * glm::cos(m_pitch),
        glm::sin(m_pitch),
        glm::sin(m_yaw) * glm::cos(m_pitch)
    );
    return glm::normalize(look);
}

glm::mat4 Camera::viewMatrix() const {
    // Make center and up vectors
    glm::vec3 pos = m_position;
    glm::vec3 look = lookVector();
    if (m_mode == Mode::THIRD_PERSON) {
        pos -= m_tpdistance*look;
    }
    glm::vec3 center = pos + look;
    glm::vec3 up(0.f, 1.f, 0.f);
    if (m_pitch == glm::half_pi<float>()) {
        up = glm::vec3(-1.f*glm::cos(m_yaw), 0.f, -1.f*glm::sin(m_yaw));
        up = glm::normalize(up);
    } else if (m_pitch == -1.f*glm::half_pi<float>()) {
        up = glm::vec3(glm::cos(m_yaw), 0.f, glm::sin(m_yaw));
        up = glm::normalize(up);
    }

    return glm::lookAt(pos, center, up);
}

glm::mat4 Camera::perspectiveMatrix() const {
    return glm::perspective(m_fovy, m_aspect, m_near, m_far);
}

glm::vec3 Camera::orthoProject(glm::vec3 pos) const {
    glm::vec4 retval = perspectiveMatrix() * viewMatrix() * glm::vec4(pos.x, pos.y, pos.z, 1.f);
    return glm::vec3(retval.x, retval.y, retval.z) / retval.w;
}

bool Camera::frustumCullAABB(const csm::aabb& aabb) const {
    glm::mat4 frust = perspectiveMatrix() * viewMatrix();
    glm::vec4 rows[4] = {
        glm::vec4(frust[0][0], frust[1][0], frust[2][0], frust[3][0]),
        glm::vec4(frust[0][1], frust[1][1], frust[2][1], frust[3][1]),
        glm::vec4(frust[0][2], frust[1][2], frust[2][2], frust[3][2]),
        glm::vec4(frust[0][3], frust[1][3], frust[2][3], frust[3][3])
    };
    glm::vec4 p[6] = {
        rows[3]-rows[0], rows[3]-rows[1], rows[3]-rows[2],
        rows[3]+rows[0], rows[3]+rows[1], rows[3]+rows[2]
    };
    glm::vec3 v[8] = {
        aabb.pos,
        aabb.pos + glm::vec3(aabb.size.x, 0.f, 0.f),
        aabb.pos + glm::vec3(0.f, aabb.size.y, 0.f),
        aabb.pos + glm::vec3(0.f, 0.f, aabb.size.z),
        aabb.pos + glm::vec3(aabb.size.x, aabb.size.y, 0.f),
        aabb.pos + glm::vec3(aabb.size.x, 0.f, aabb.size.z),
        aabb.pos + glm::vec3(0.f, aabb.size.y, aabb.size.z),
        aabb.pos + aabb.size
    };

    // Cull aabb if there exists a plane that rejects every vertex
    bool cull;
    for (int i = 0; i < 6; ++i) {
        cull = true;
        for (int j = 0; j < 8; ++j) {
            // Test if this vertex is inside this plane; if it is, then we cannot reject the AABB using this plane
            if (p[i][0]*v[j][0] + p[i][1]*v[j][1] + p[i][2]*v[j][2] + p[i][3] >= 0.f) {
                cull = false;
                break;
            }
        }
        if (cull)
            break;
    }

    return cull;
}
