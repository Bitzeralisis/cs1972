#pragma once

#include "game.h"
#include "world.h"
#include "../util/CommonIncludes.h"
#include "../util/HeaderUtils.h"

namespace CS1972Engine {

class Graphics;

class Entity {
    friend class World;

public:
    virtual ~Entity() {}

private:
    World *m_parent;

    bool m_removeFlag = false;
    bool m_deleteFlag = false;

protected:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_accel;

protected:
    inline REFERENCE_ACCESSOR_DEEP(Graphics,graphics,parent()->parent->g)
    void tickPhysicsDiscrete();
    void tickPhysicsContinuous(); // NYI
    void checkCollide(Entity *other);

public:
    VALUE_ACCESSOR(World *,parent)

    virtual void tick() = 0;
    virtual void draw() = 0;
    virtual glm::vec2 getCylinder() const = 0; // Return a hitbox defined as a cylinder with radius x and height y, with the center-bottom of the cylinder at m_position
    virtual void collide(glm::vec3 mtv, const Entity *other) = 0; // Response callback for collisions
};

}
