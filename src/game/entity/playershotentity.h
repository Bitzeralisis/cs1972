#pragma once

#include "cogentity.h"

namespace COG {

class EnemyEntity;
class PlayerEntity;

/*
 * A PlayerShotEntity takes care of attaching itself to an enemy and drawing itself.
 * Detatching from enemies and dealing damage is handled by the enemy itself.
 */
class PlayerShotEntity : public COGEntity {
public:
    PlayerShotEntity(PlayerEntity *parent, float beat, EnemyEntity *target, glm::vec3 back, glm::vec3 up);
    virtual ~PlayerShotEntity();

private:
    const float TARGET_SIZE_FACTOR = 1.f/6.f;
    const float BURST_SIZE_FACTOR = 5.f/3.f;

private:
    PlayerEntity *m_parent;
    float m_shotBeat;
    bool m_hit;
    EnemyEntity *m_target;
    glm::vec3 m_back;
    glm::vec3 m_up;

public:
    VALUE_ACCESSOR(float,shotBeat)

    void detatchShot(bool hit, glm::vec3 position);

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override;
};

}
