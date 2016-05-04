#pragma once

#include "game/entity/enemyentity.h"

namespace COG {

class GateEnemy : public EnemyEntity {
public:
    GateEnemy(float beat);
    virtual ~GateEnemy() { }

private:
    bool m_open[8] = { false, false, false, false, false, false, false, false };

public:
    inline void notifyOpening(int id) { m_open[id] = true; }

    virtual void performAction(COGScriptAction *action) override;

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override;
    virtual csm::ellipsoid getEllipsoid() const override;
    virtual void hitEffect(float beat) override;
    virtual void deathEffect(float beat) override;
};

class GateSubEnemy : public EnemyEntity {
public:
    GateSubEnemy(float beat, GateEnemy *parent, int id);
    virtual ~GateSubEnemy() { }

private:
    GateEnemy *m_parent;
    int m_id;
    float m_rotation;

public:
    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override;
    virtual csm::ellipsoid getEllipsoid() const override;
    virtual void hitEffect(float beat) override;
    virtual void deathEffect(float beat) override;
};

}
