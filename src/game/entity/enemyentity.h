#pragma once

#include "playerentity.h"

namespace Minecraft {

class EnemyEntity : public MinecraftEntity {
public:
    EnemyEntity(PlayerEntity *target, glm::vec3 pos);
    virtual ~EnemyEntity() { }

private:
    const float WALK_VELOCITY = 4.f;
    const float JUMP_VELOCITY = 6.9f;

    PlayerEntity *m_target;

    bool m_standing;
    bool m_attacking = true;
    bool m_dead = false;

public:
    VALACC_MUT(glm::vec3,position)
    VALUE_ACCESSOR(bool,dead)

    void attack();
    void runAway();

    virtual void tick(float seconds) override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual csm::aabb getAabb() const override;
    virtual void collideTerrain(glm::vec3 tv) override;
    virtual void collide(glm::vec3 mtv, const MinecraftEntity *other) override;
};

}
