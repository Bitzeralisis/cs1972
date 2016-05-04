#pragma once

#include "cogentity.h"

namespace COG {

class PlayerEntity;

class EnemyShotEntity : public COGEntity {
public:
    EnemyShotEntity(float beat, float travelTime, glm::vec3 pos, glm::vec3 vel, PlayerEntity *target, int type, int lane);
    virtual ~EnemyShotEntity() { }

private:
    const float HEXAGON_SIZE_FACTOR = 1.f/3.5f;

private:
    PlayerEntity *m_target;
    glm::vec3 m_visualPosition;
    glm::vec3 m_shotVel;
    float m_travelTime;
    float m_hitBeat;
    int m_shotType;
    int m_approachLane;

public:
    VALUE_ACCESSOR(glm::vec3,visualPosition)
    VALUE_ACCESSOR(float,hitBeat)
    VALUE_ACCESSOR(int,approachLane)

    void detatchShot();

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override;
};

}
