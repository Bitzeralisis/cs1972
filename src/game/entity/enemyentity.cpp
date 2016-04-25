#include "enemyentity.h"
#include "playerentity.h"
#include "playershotentity.h"
#include "game/coggame.h"

using namespace COG;

EnemyEntity::EnemyEntity(float beat, int health, int value)
    : COGEntity(beat)
    , m_health(health)
    , m_futureHealth(health)
    , m_scoreValue(value)
{ }

EnemyEntity::~EnemyEntity() {
    for (std::deque<PlayerShotEntity *>::iterator it = m_attachedShots.begin(); it != m_attachedShots.end(); ++it)
        (*it)->detatchShot(false, m_position);
}

void EnemyEntity::attachShot(PlayerShotEntity *shot) {
    m_attachedShots.push_back(shot);
    --m_futureHealth;
}

void EnemyEntity::tickBeats(float beats) {
    // Take damage from shots
    while (!m_attachedShots.empty() && m_attachedShots.front()->shotBeat()+1.f <= beat()) {
        float beat = m_attachedShots.front()->shotBeat()+1.f;

        m_attachedShots.front()->detatchShot(true, m_position);
        m_attachedShots.pop_front();
        --m_health;

        // Die
        if (m_health <= 0) {
            GAME->controller()->gainScoreValue(m_scoreValue);
            deathEffect(beat);
            for (std::deque<PlayerShotEntity *>::iterator it = m_attachedShots.begin(); it != m_attachedShots.end(); ++it)
                (*it)->detatchShot(false, m_position);
            parent()->deleteEntity(this);
        } else {
            hitEffect(beat);
        }
    }

    tickPhysicsContinuous(beats);
}
