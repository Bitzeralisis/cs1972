#pragma once

#include "cogentity.h"

namespace COG {

class PlayerEntity : public COGEntity {
public:
    PlayerEntity();
    virtual ~PlayerEntity() { }

private:
    int m_score = 0;
    int m_combo;

public:
    VALACC_MUT(int,score)
    VALACC_MUT(int,combo)

    void gainScoreValue(int score);
    void makeParticles(glm::vec3 position, glm::vec3 velocity);

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override { }
};

}
