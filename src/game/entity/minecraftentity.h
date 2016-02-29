#pragma once

#include "../../engine/entity.h"

namespace Minecraft {

class MinecraftEntity : public CS1972Engine::Entity {
public:
    enum struct Type : int {
        PLAYER, PLAYER_ATTACK,
        ENEMY
    };

    explicit MinecraftEntity(Type type) : m_type(type) { }
    virtual ~MinecraftEntity() { }


private:
    virtual void collide(glm::vec3 mtv, const Entity *other) override { collide(mtv, (MinecraftEntity *)other); }

protected:
    const float GRAVITY_ACCEL = -20.f;

    const Type m_type;

public:
    VALUE_ACCESSOR(Type,type)

    virtual void tick(float seconds) = 0;
    virtual void draw() = 0;
    virtual glm::vec2 getCylinder() const = 0;
    virtual csm::aabb getAabb() const = 0;
    virtual void collideTerrain(glm::vec3 tv) = 0;
    virtual void collide(glm::vec3 mtv, const MinecraftEntity *other) = 0;
};

}
