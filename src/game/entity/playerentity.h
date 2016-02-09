#pragma once

#include "warmupentity.h"

namespace WarmupGame {

class PlayerEntity : public WarmupEntity {
public:
    explicit PlayerEntity(glm::vec3);
    virtual ~PlayerEntity() {}

public:
    const int MAX_DASHBAR = 1200;
    const int MAX_HEALTH = 4;

private:
    const int START_DASHBAR = 1200;
    const int DASHBAR_COST = 10;
    const int DASHBAR_REGEN = 3;
    const float RUN_SPEED = 0.25f;
    const float DASH_SPEED = 0.5f;
    const float JUMP_SPEED = 0.33f;
    const float DASH_JUMP_SPEED = 0.66f;
    const float FALL_ACCEL = -0.017f;

    const glm::vec2 PLAYER_HITBOX = glm::vec2(1.5f, 5.f);
    const glm::vec3 SCALE_VECTOR = glm::vec3(3.f, 5.f, 3.f);

    int m_dashBar = START_DASHBAR;
    int m_health = MAX_HEALTH;
    bool m_dead = false;

    bool m_onGround = true;
    int m_dash = 0;

public:
    const float HEAD_HEIGHT = 4.f;

public:
    VALUE_ACCESSOR(int,dashBar)
    VALUE_ACCESSOR(int,health)
    void suicide() { m_health = 0; }
    VALUE_ACCESSOR(bool,dead)
    VALUE_ACCESSOR(int,dash)

    void move(glm::vec3 walk, bool jumping, bool dashing);

    virtual void tick() override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual void collide(glm::vec3 mtv, const WarmupEntity *other) override;
};

}
