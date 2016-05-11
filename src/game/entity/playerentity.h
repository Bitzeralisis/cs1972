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

    float m_minFog = 0.f;
    float m_maxFog = 0.f;
    int m_cameraBehavior = 0;
    float m_baseYaw = 0.f;
    float m_basePitch = 0.f;
    float m_yawLimit = 0.25f;
    float m_pitchLimit = 0.25f;

    AmbientEntity *m_ambience;
    std::deque<EnemyShotEntity *> m_attachedShots;

public:
    VALUE_ACCESSOR(AmbientEntity *,ambience)
    VALUE_ACCESSOR(float,maxFog)
    VALUE_ACCESSOR(float,baseYaw)
    VALUE_ACCESSOR(float,basePitch)

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
