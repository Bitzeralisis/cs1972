#pragma once

#include "cogentity.h"

namespace COG {

class LightEntity : public COGEntity {
public:
    LightEntity(float beat, glm::vec3 pos, glm::vec3 vel);
    virtual ~LightEntity() { }

private:
    float m_life = 4.f;

public:
    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override;
};

}
