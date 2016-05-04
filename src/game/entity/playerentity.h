#pragma once

#include "controlledentity.h"

namespace COG {

class AmbientEntity;
class EnemyShotEntity;
class GameScreen;

class PlayerEntity : public ControlledEntity {
    friend class GameScreen;

public:
    PlayerEntity();
    virtual ~PlayerEntity() { }

private:
    bool m_win = false;
    int m_score = 0;
    int m_combo;
    int m_dealtDamage = 0;

    AmbientEntity *m_ambience;
    std::deque<EnemyShotEntity *> m_attachedShots;

public:
    VALUE_ACCESSOR(AmbientEntity *,ambience)

    void dealDamage() { ++m_dealtDamage; }
    std::deque<EnemyShotEntity *> *attachedShots() { return &m_attachedShots; }

    void gainScoreValue(int score);
    void makeParticles(int amount, glm::vec3 position, float width, glm::vec3 velocity, glm::vec3 color, glm::vec2 life);
    void makeParticles(int amount, glm::vec3 position, glm::vec3 size, glm::vec3 velocity, glm::vec3 color, glm::vec2 life);

    void attachShot(EnemyShotEntity *shot);

    virtual void performAction(COGScriptAction *action) override;

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override { }
};

}
