#pragma once

#include "../../engine/entity.h"

namespace WarmupGame {

class ObstacleEntity : public CS1972Engine::Entity {
public:
    ObstacleEntity();
    virtual ~ObstacleEntity() {}

private:
    glm::vec2 m_hitbox;

public:
    virtual void tick() override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual void collide(glm::vec3 mtv, const Entity *other);
};

}
