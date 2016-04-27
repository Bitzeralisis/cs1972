#pragma once

#include "cogentity.h"

namespace COG {

class PlayerEntity;

class EnemyShotEntity : public COGEntity {
public:
    EnemyShotEntity(float beat, glm::vec3 pos, PlayerEntity *target, int type, int lane);
    virtual ~EnemyShotEntity() { }

private:
    const float HEXAGON_SIZE_FACTOR = 1.f/3.5f;

private:
    PlayerEntity *m_target;
    float m_hitBeat;
    int m_shotType;
    int m_approachLane;

public:
    VALUE_ACCESSOR(float,hitBeat)
    VALUE_ACCESSOR(int,approachLane)

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override;
};

}
