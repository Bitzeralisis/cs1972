#pragma once

#include "warmupentity.h"

namespace WarmupGame {

class ItemEntity : public WarmupEntity {
public:
    ItemEntity(bool isGood, glm::vec3 pos, glm::vec3 vel);
    virtual ~ItemEntity() {}

private:
    const glm::vec2 HITBOX = glm::vec2(0.75f, 1.5f);
    const glm::vec3 SCALE_VECTOR = glm::vec3(1.5f, 1.5f, 1.5f);

    const float DECAY_TIME = 3.f;

    float m_decay = -1.f;

public:
    virtual void tick(float seconds) override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual void collide(glm::vec3 mtv, const WarmupEntity *other) override;
};

}
