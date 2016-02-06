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
    Camera(glm::vec3 pos, float yaw, float pitch, float fovy, float aspect, float near, float far);

public:
    glm::vec3 position;
    float yaw; // Normalized to (-pi, pi] every time viewMatrix() is called
    float pitch; // Normalized to [-pi/2, pi/2] every time viewMatrix() is called

    float fovy;
    float aspect;
    float near;
    float far;

public:
    void walk(glm::vec3 dir);
    glm::mat4 viewMatrix();
    glm::mat4 perspectiveMatrix();
};

}
