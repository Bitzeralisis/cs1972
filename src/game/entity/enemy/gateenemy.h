#pragma once

#include "game/entity/enemyentity.h"

namespace COG {

class GateSubEnemy;

class GateEnemy : public EnemyEntity {
    friend class GateSubEnemy;

public:
    GateEnemy(float beat);
    virtual ~GateEnemy() { }

private:
    GateSubEnemy *children[8];
    bool m_opened = false;
    bool m_open[8] = { false, false, false, false, false, false, false, false };

public:
    virtual void performAction(COGScriptAction *action) override;

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override;
    virtual csm::ellipsoid getEllipsoid() const override;
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
    virtual void deathEffect(float beat) override;
};

}
