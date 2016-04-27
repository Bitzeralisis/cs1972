#pragma once

#include "cogentity.h"

namespace COG {

class EnemyShotEntity;

class PlayerEntity : public COGEntity {
public:
    PlayerEntity();
    virtual ~PlayerEntity() { }

private:
    int m_score = 0;
    int m_combo;

    std::deque<EnemyShotEntity *> m_attachedShots;

public:
    VALACC_MUT(int,score)
    VALACC_MUT(int,combo)
    std::deque<EnemyShotEntity *> *attachedShots() { return &m_attachedShots; }

    void gainScoreValue(int score);
    void makeParticles(glm::vec3 position, glm::vec3 velocity);

    void attachShot(EnemyShotEntity *shot);

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override { }
};

}
