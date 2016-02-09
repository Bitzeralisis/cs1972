#pragma once

#include "warmupentity.h"

namespace WarmupGame {

class ObstacleEntity : public WarmupEntity {
public:
    ObstacleEntity(glm::vec3 pos, glm::vec2 size);
    virtual ~ObstacleEntity() {}

private:
    glm::vec2 m_hitbox;

public:
    virtual void tick() override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual void collide(glm::vec3 mtv, const WarmupEntity *other) override;
};

}
