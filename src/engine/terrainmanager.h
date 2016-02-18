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
};

}
