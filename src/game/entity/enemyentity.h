#pragma once

#include "controlledentity.h"
#include <deque>

namespace COG {

class PlayerShotEntity;

class EnemyEntity : public ControlledEntity {
public:
    EnemyEntity(float beat, int health, int value);
    virtual ~EnemyEntity();

private:
    int m_futureHealth;
    std::deque<PlayerShotEntity *> m_attachedShots;

protected:
    int m_health;
    int m_scoreValue;
    bool m_targetable = true;

public:
    VALUE_ACCESSOR(int,health)
    VALUE_ACCESSOR(int,futureHealth)
    VALUE_ACCESSOR(bool,targetable)

    void attachShot(PlayerShotEntity *shot);

    virtual void performAction(COGScriptAction *action) override;

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override = 0;
    virtual csm::ellipsoid getEllipsoid() const override = 0;
    virtual void hitEffect(float beat) { }
    virtual void deathEffect(float beat) { }
};

}
