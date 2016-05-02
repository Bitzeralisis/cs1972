#include "enemyentity.h"
#include "enemyshotentity.h"
#include "playerentity.h"
#include "playershotentity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "game/cogscript.h"

using namespace COG;

EnemyEntity::EnemyEntity(float beat, int health, int value)
    : ControlledEntity(beat)
    , m_health(health)
    , m_futureHealth(health)
    , m_scoreValue(value)
{ }

EnemyEntity::~EnemyEntity() {
    for (std::deque<PlayerShotEntity *>::iterator it = m_attachedShots.begin(); it != m_attachedShots.end(); ++it)
        (*it)->detatchShot(false, m_position);
}

void EnemyEntity::shoot(float beat, float travelTime, glm::vec3 pos, glm::vec3 vel, int type, int lane) {
    EnemyShotEntity *shot = new EnemyShotEntity(beat, travelTime, pos, vel, GAME->controller(), type, lane);
    parent()->addEntity(GameScreen::LAYER_ENEMY_PROJECTILES, shot);
    GAME->controller()->attachShot(shot);
}

void EnemyEntity::attachShot(PlayerShotEntity *shot) {
    m_attachedShots.push_back(shot);
    --m_futureHealth;
}

void EnemyEntity::performAction(COGScriptAction *action) {
    switch (action->action) {
    case Action::SHOOT: {
        COGScriptActionShoot *act = (COGScriptActionShoot *) action;
        glm::vec3 pos = act->pos.coord + (act->pos.relative ? m_position : glm::vec3(0.f));
        glm::vec3 vel = act->vel.coord + (act->vel.relative ? m_velocity : glm::vec3(0.f));
        shoot(act->beat, act->travel, pos, vel, act->type, act->lane);
        return;
    }

    case Action::SET_ATTRIBUTE: {
        COGScriptActionSetAttribute *act = (COGScriptActionSetAttribute *) action;
        switch (act->attr) {
        case Attribute::TARGETABLE: {
            COGScriptAttributeFloat *attr = (COGScriptAttributeFloat *) act;
            m_targetable = attr->value;
            return;
        }

        default:
            break;
        }
    }

    default:
        break;
    }

    ControlledEntity::performAction(action);
}

void EnemyEntity::tickBeats(float beats) {
    // Do controlled actions
    ControlledEntity::tickBeats(beats);

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
}
