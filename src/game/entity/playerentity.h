#pragma once

#include "../../engine/entity.h"

namespace WarmupGame{

class PlayerEntity : public CS1972Engine::Entity {
public:
    PlayerEntity();
    virtual ~PlayerEntity() {}

private:
    const float RUN_SPEED = 0.25f;
    const float DASH_SPEED = 0.5f;
    const float JUMP_SPEED = 0.33f;
    const float DASH_JUMP_SPEED = 0.66f;
    const float FALL_ACCEL = -0.017f;

    const glm::vec2 PLAYER_HITBOX = glm::vec2(1.5f, 5.f);
    const glm::vec3 SCALE_VECTOR = glm::vec3(3.f, 5.f, 3.f);

    bool m_onGround = true;
    int m_dash = 0;

public:
    const float HEAD_HEIGHT = 4.f;

public:
    VALACC_MUT(glm::vec3,position)
    VALACC_MUT(glm::vec3,velocity)
    VALACC_MUT(glm::vec3,accel)

    VALUE_ACCESSOR(int,dash)

    void move(glm::vec3 walk, bool jumping, bool dashing);

    virtual void tick() override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual void collide(glm::vec3 mtv, const Entity *other);
};

}
