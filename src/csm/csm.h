#pragma once

#include <glm/glm.hpp>
#include <cstdlib>

namespace csm {

template<typename NUMERICAL_TYPE> inline NUMERICAL_TYPE rand(NUMERICAL_TYPE max) { return max*(NUMERICAL_TYPE)std::rand()/RAND_MAX; }
template<typename NUMERICAL_TYPE> inline NUMERICAL_TYPE rand(NUMERICAL_TYPE min, NUMERICAL_TYPE max) { return (max-min)*(NUMERICAL_TYPE)std::rand()/RAND_MAX+min; }
inline glm::vec3 bezier_curve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t) { return (1.f-t)*((1.f-t)*p0+t*p1)+t*((1.f-t)*p1+t*p2); }

int solve_quadratic(float a, float b, float c, float &pos, float &neg);

}
