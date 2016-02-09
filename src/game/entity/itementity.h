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

    const int DECAY_TIME = 180;

    int m_decay = -1;

public:
    virtual void tick() override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual void collide(glm::vec3 mtv, const WarmupEntity *other) override;
};

}
