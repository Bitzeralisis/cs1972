#pragma once

#include "game/entity/enemyentity.h"

namespace COG {

class CubeaEnemyEntity : public EnemyEntity {
public:
    CubeaEnemyEntity(float beat, glm::vec3 pos);
    virtual ~CubeaEnemyEntity() { }

public:
    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override;
    virtual csm::ellipsoid getEllipsoid() const override;
    virtual void hitEffect(float beat) override;
    virtual void deathEffect(float beat) override;
};

}
