#pragma once

#include "game/entity/enemyentity.h"

namespace COG {

class CubeaEnemy : public EnemyEntity {
public:
    CubeaEnemy(float beat);
    virtual ~CubeaEnemy() { }

public:
    virtual void draw(int pass, float beat) override;
    virtual csm::ellipsoid getEllipsoid() const override;
    virtual void hitEffect(float beat) override;
    virtual void deathEffect(float beat) override;
};

}
