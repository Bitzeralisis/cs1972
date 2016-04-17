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
    virtual ~Entity() { }

private:
    World *m_parent;

    glm::vec3 m_startPosition;
    bool m_removeFlag = false;
    bool m_deleteFlag = false;

protected:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_accel;
    bool m_collides = true;
    bool m_static = false; // Will not be moved by collisions if true
    bool m_collidesTerrain = true;

protected:
    inline REFERENCE_ACCESSOR_DEEP(Graphics,graphics,parent()->parent()->g)
    void tickPhysicsDiscrete(float seconds);
    void tickPhysicsContinuous(float seconds);
    void checkCollide(Entity *other);

public:
    VALUE_ACCESSOR(World *,parent)
    VALUE_ACCESSOR(glm::vec3,position)
    VALUE_ACCESSOR(glm::vec3,velocity)
    VALUE_ACCESSOR(glm::vec3,accel)
    VALUE_ACCESSOR(bool,collides)
    VALUE_ACCESSOR_DEEP(bool,is_static,m_static)
    VALUE_ACCESSOR(bool,collidesTerrain)

    virtual void tick(float seconds) = 0;
    virtual void draw(int pass) = 0;
    virtual glm::vec2 getCylinder() const = 0;
    virtual csm::aabb getAabb() const = 0;
    virtual csm::ellipsoid getEllipsoid() const = 0;
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) = 0;
    virtual void collide(glm::vec3 mtv, const Entity *other) = 0;
};

}
