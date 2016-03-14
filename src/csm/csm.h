#pragma once

#include <cmath>

namespace csm {

int solve_quadratic(float a, float b, float c, float &pos, float &neg) {
    float det = b*b-4*a*c;
    if (det < 0.f) return 0;
    float sqrtdet = sqrt(det);
    pos = (-1.f*b + sqrtdet) / (2.f*a);
    neg = (-1.f*b - sqrtdet) / (2.f*a);
    if (det == 0.f) return 1;
    return 2;
}

}
