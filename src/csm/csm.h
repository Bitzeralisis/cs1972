#pragma once

#include <glm/glm.hpp>

namespace csm {

int solve_quadratic(float a, float b, float c, float &pos, float &neg);
glm::vec3 bezier_curve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t);

}
