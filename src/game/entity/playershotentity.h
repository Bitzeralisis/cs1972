#pragma once

#include "cogentity.h"

namespace COG {

class EnemyEntity;
class PlayerEntity;

class PlayerShotEntity : public COGEntity {
public:
    PlayerShotEntity(PlayerEntity *parent, float beat, EnemyEntity *target, glm::vec3 back, glm::vec3 up);
    virtual ~PlayerShotEntity();

private:
    const float TARGET_SIZE_FACTOR = 6.f;

private:
    PlayerEntity *m_parent;
    float m_shotBeat;
    EnemyEntity *m_target;
    glm::vec3 m_back;
    glm::vec3 m_up;

public:
    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override;
};

}
