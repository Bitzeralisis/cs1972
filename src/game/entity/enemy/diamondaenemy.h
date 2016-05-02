#pragma once

#include "game/entity/enemyentity.h"

namespace COG {

class DiamondaEnemy : public EnemyEntity {
public:
    DiamondaEnemy(float beat);
    virtual ~DiamondaEnemy() { }

public:
    virtual void draw(int pass, float beat) override;
    virtual csm::ellipsoid getEllipsoid() const override;
    virtual void hitEffect(float beat) override;
    virtual void deathEffect(float beat) override;
};

}
