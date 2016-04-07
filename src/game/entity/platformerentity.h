#pragma once

#include "../../engine/entity.h"

namespace Platformer {

class PlatformerEntity : public CS1972Engine::Entity {
public:
    enum struct Type : int {
        PLAYER, PLAYER_ATTACK,
        ENEMY, ENEMY_ATTACK
    };

    explicit PlatformerEntity(Type type) : m_type(type) { }
    virtual ~PlatformerEntity() { }

private:
    virtual void collide(glm::vec3 mtv, const Entity *other) override { collide(mtv, (PlatformerEntity *)other); }

protected:
    const glm::vec3 GRAVITY_ACCEL = glm::vec3(0.f, -15.f, 0.f);

    const Type m_type;

public:
    VALUE_ACCESSOR(Type,type)

    virtual void collide(glm::vec3 mtv, const PlatformerEntity *other) = 0;
};

}
