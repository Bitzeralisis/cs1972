#pragma once

#include <glm/common.hpp>

namespace csm {

struct aabb {
    glm::vec3 pos;
    glm::vec3 size;
    aabb() : pos(glm::vec3()), size(glm::vec3()) { }
    aabb(glm::vec3 v, glm::vec3 s) : pos(v), size(s) { }
    bool intersects(const aabb &rhs) const {
        glm::vec3 max = pos+size;
        glm::vec3 rhsmax = rhs.pos+rhs.size;
        return
            max[0] > rhs.pos[0] &&
            max[1] > rhs.pos[1] &&
            max[2] > rhs.pos[2] &&
            pos[0] < rhsmax[0] &&
            pos[1] < rhsmax[1] &&
            pos[2] < rhsmax[2] ;
    }
};

}
