#pragma once

#include "platformerentity.h"

namespace Platformer {

class PlayerEntity;

class EnemyBossEntity : public PlatformerEntity {
public:
    EnemyBossEntity(PlayerEntity *target);
    virtual ~EnemyBossEntity() { }

public:
    const float MAX_HEALTH = 10.f;

    const float SHOT_FREQ = 2.f;
    const float SHOT_SPEED = 15.f;
    const float SHOT_CD = 0.25f;
    const int SHOT_COUNT = 3;

    const float SUMMON_FREQ = 5.f;
    const int MAX_MINIONS = 4;

private:
    float m_health = MAX_HEALTH;

    int m_currentAttack = 0;
    float m_nextAttack = 10.f;

    int m_shotCount = 0;
    float m_shotCd = 0.f;

    int m_minionCount = 0;

    PlayerEntity *m_target;
    float m_time = 0.f;

public:
    VALUE_ACCESSOR(float,health)
    inline void loseMinion() { --m_minionCount; }

    virtual void tick(float seconds) override;
    virtual void draw(int pass) override;
    virtual glm::vec2 getCylinder() const override { return glm::vec2(0.5f, 2.f); }
    virtual csm::aabb getAabb() const override { return csm::aabb(); }
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.5f, 1.f, 0.5f)); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) override;
    virtual void collide(glm::vec3 mtv, const PlatformerEntity *other) override;
};

}
