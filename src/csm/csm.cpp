#include "csm.h"
#include <cmath>

using namespace csm;

int csm::solve_quadratic(float a, float b, float c, float &pos, float &neg) {
    if (a == 0.f) {
        if (b == 0.f) return 0;
        pos = neg = -c/b;
        return 1;
    }
    float det = b*b-4.f*a*c;
    if (det < 0.f) return 0;
    float sqrtdet = sqrt(det);
    pos = (-1.f*b + sqrtdet) / (2.f*a);
    neg = (-1.f*b - sqrtdet) / (2.f*a);
    if (det == 0.f) return 1;
    return 2;
}
