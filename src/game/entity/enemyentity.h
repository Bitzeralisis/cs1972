#pragma once

#include "cogentity.h"

namespace COG {

class EnemyEntity : public COGEntity {
public:
    EnemyEntity(float beat, glm::vec3 pos);
    virtual ~EnemyEntity() { }

public:
    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override;
    virtual csm::ellipsoid getEllipsoid() const override;
};

}
