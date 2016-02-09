#pragma once

#include "warmupentity.h"

namespace WarmupGame {

class BossEntity : public WarmupEntity {
public:
    BossEntity(int period, int radius, int height);
    virtual ~BossEntity() {}

private:
    const glm::vec2 HITBOX = glm::vec2(1.5f, 5.f);
    const glm::vec3 SCALE_VECTOR = glm::vec3(3.f, 5.f, 3.f);

    float m_period;
    float m_radius;
    float m_height;
    float m_angle = 0.f;

    bool m_dead = false;

    int m_time = 0;

public:
    VALUE_ACCESSOR(bool,dead)

    virtual void tick() override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual void collide(glm::vec3 mtv, const WarmupEntity *other) override;
};

}
