#pragma once

#include "../util/CommonIncludes.h"
#include "../util/HeaderUtils.h"
#include "../csm/csm_primitive.h"

namespace CS1972Engine {

/**
 * @brief The Camera class
 * Basic helper class for moving, rotating, and generating matrices for a camera
 * for a first-person perspective 3D game.
 */
class Camera {
public:
    enum struct Mode : int {
        FIRST_PERSON,
        THIRD_PERSON
    };

    Camera(Mode mode = Mode::FIRST_PERSON, glm::vec3 pos = glm::vec3(), float yaw = 0.f, float pitch = 0.f, float tpdistance = 10.f, float fovy = 0.75f*glm::half_pi<float>(), float aspect = 4.f/3.f, float near = 1.f, float far = 200.f);

private:
    Mode m_mode;
    glm::vec3 m_position;
    float m_yaw; // Normalized to (-pi, pi] every time viewMatrix() is called
    float m_pitch; // Normalized to [-pi/2, pi/2] every time viewMatrix() is called
    float m_tpdistance;

    float m_fovy;
    float m_aspect;
    float m_near;
    float m_far;

public:
    VALACC_MUT(Mode, mode)
    VALACC_MUT(glm::vec3, position)
    MUTATOR_DEEP(float, position_x, m_position.x)
    MUTATOR_DEEP(float, position_y, m_position.y)
    MUTATOR_DEEP(float, position_z, m_position.z)
    VALUE_ACCESSOR(float, yaw)
    void yaw(float yaw);
    VALUE_ACCESSOR(float, pitch)
    void pitch(float pitch);
    VALACC_MUT(float, tpdistance)
    VALACC_MUT(float, fovy)
    VALACC_MUT(float, aspect)
    VALACC_MUT(float, near)
    VALACC_MUT(float, far)

    void walk(const glm::vec3& dir);
    void normalizeView();
    glm::vec3 lookVector() const;
    glm::mat4 viewMatrix() const;
    glm::mat4 perspectiveMatrix() const;

    glm::vec3 orthoProject(glm::vec3 pos) const;
    bool frustumCullAABB(const csm::aabb& aabb) const;
};

}
