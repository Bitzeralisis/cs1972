#pragma once

#include "game/entity/enemyentity.h"

namespace COG {

class SwimmeraEnemy : public EnemyEntity {
public:
    SwimmeraEnemy(float beat);
    virtual ~SwimmeraEnemy() { }

public:
    virtual void draw(int pass, float beat) override;
    virtual csm::ellipsoid getEllipsoid() const override;
    virtual void deathEffect(float beat) override;
};

}
