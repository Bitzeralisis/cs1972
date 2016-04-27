#pragma once

#include "game/entity/enemyentity.h"

namespace COG {

class DiamondaEnemy : public EnemyEntity {
public:
    DiamondaEnemy(float beat, glm::vec3 pos);
    virtual ~DiamondaEnemy() { }

private:
    bool m_shot = false;

public:
    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override;
    virtual csm::ellipsoid getEllipsoid() const override;
    virtual void hitEffect(float beat) override;
    virtual void deathEffect(float beat) override;
};

}
