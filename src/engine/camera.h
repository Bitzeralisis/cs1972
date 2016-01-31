#pragma once

#include "../util/CommonIncludes.h"

namespace CS1972Engine {

/**
 * @brief The Camera class
 * Basic helper class for moving, rotating, and generating matrices for a camera
 * for a first-person perspective 3D game.
 */
class Camera {
public:
    Camera();
    Camera(float fovy, float aspect, float near, float far);

public:
    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
    float yaw = 0.f; // Normalized to (-pi, pi] every time viewMatrix() is called
    float pitch = 0.f; // Normalized to [-pi/2, pi/2] every time viewMatrix() is called

    float fovy = glm::half_pi<float>();
    float aspect = 4.f/3.f;
    float near = 1.f;
    float far = 200.f;

public:
    void walk(glm::vec3 dir);
    glm::mat4 viewMatrix();
    glm::mat4 perspectiveMatrix();
};

}
