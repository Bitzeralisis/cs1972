#pragma once

#include "../util/HeaderUtils.h"
#include "game.h"
#include "world.h"

namespace CS1972Engine {

class Graphics;

class TerrainManager {
    friend class World;

public:
    virtual ~TerrainManager() { }

private:
    World *m_parent;

protected:
    REFERENCE_ACCESSOR_DEEP(Graphics,graphics,m_parent->parent()->g)

public:
    VALUE_ACCESSOR(World*,parent)

    virtual void tick(float seconds) = 0;
    virtual void draw() = 0;
    virtual glm::vec3 collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1) const = 0;
};

}
