#pragma once

#include "../../engine/entity.h"

namespace WarmupGame {

class WarmupEntity : public CS1972Engine::Entity {
public:
    enum struct Type : int {
        PLAYER, BOSS,
        OBSTACLE, GOOD_ITEM, BAD_ITEM
    };

    WarmupEntity(Type type);
    virtual ~WarmupEntity() {}

private:
    virtual void collide(glm::vec3 mtv, const Entity *other) override;

protected:
    const Type m_type;

public:
    VALUE_ACCESSOR(Type,type)

    virtual void tick(float seconds) override = 0;
    virtual void draw() override = 0;
    virtual glm::vec2 getCylinder() const override = 0;
    virtual void collide(glm::vec3 mtv, const WarmupEntity *other) = 0;
};

}
