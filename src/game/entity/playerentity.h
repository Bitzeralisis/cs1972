#pragma once

#include "controlledentity.h"

namespace COG {

class EnemyShotEntity;

class PlayerEntity : public ControlledEntity {
public:
    PlayerEntity();
    virtual ~PlayerEntity() { }

private:
    bool m_win = false;
    int m_score = 0;
    int m_combo;

    std::deque<EnemyShotEntity *> m_attachedShots;

public:
    VALUE_ACCESSOR(bool,win)
    VALACC_MUT(int,score)
    VALACC_MUT(int,combo)
    std::deque<EnemyShotEntity *> *attachedShots() { return &m_attachedShots; }

    void gainScoreValue(int score);
    void makeParticles(int amount, glm::vec3 position, float width, glm::vec3 velocity, glm::vec3 color, glm::vec2 life);

    void attachShot(EnemyShotEntity *shot);

    virtual void performAction(COGScriptAction *action) override;

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override { }
};

}
