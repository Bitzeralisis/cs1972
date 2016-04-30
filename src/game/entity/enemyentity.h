#pragma once

#include "cogentity.h"
#include <deque>

namespace COG {

class PlayerShotEntity;

class EnemyEntity : public COGEntity {
public:
    EnemyEntity(float beat, int health, int value);
    virtual ~EnemyEntity();

private:
    int m_health;
    int m_futureHealth;
    int m_scoreValue;
    bool m_targetable = true;
    std::deque<PlayerShotEntity *> m_attachedShots;

protected:
    MUTATOR(bool,targetable)

public:
    VALUE_ACCESSOR(int,health)
    VALUE_ACCESSOR(int,futureHealth)
    VALUE_ACCESSOR(bool,targetable)

    void shoot(float beat, float travelTime, glm::vec3 pos, glm::vec3 vel, int type, int lane);
    void attachShot(PlayerShotEntity *shot);

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override = 0;
    virtual csm::ellipsoid getEllipsoid() const override = 0;
    virtual void hitEffect(float beat) = 0;
    virtual void deathEffect(float beat) = 0;
};

}
