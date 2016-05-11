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
    for (auto it = m_attachedShots.begin(); it != m_attachedShots.end(); ++it)
        (*it)->detatchShot(false, m_position);
}

void EnemyEntity::attachShot(PlayerShotEntity *shot) {
    m_attachedShots.push_back(shot);
    --m_futureHealth;
}

void EnemyEntity::performAction(COGScriptAction *action) {
    switch (action->action) {
    case Action::SET_ATTRIBUTE: {
        COGScriptActionSetAttribute *act = (COGScriptActionSetAttribute *) action;
        switch (act->attr) {
        case Attribute::FLOAT: {
            COGScriptAttributeFloat *attr = (COGScriptAttributeFloat *) act;
            if (attr->key == "targetable") m_targetable = attr->value;
            else
                break;
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

    // Lose all attached shots if untargetable
    if (!targetable()) {
        for (auto it = m_attachedShots.begin(); it != m_attachedShots.end(); ++it)
            (*it)->detatchShot(false, m_position);
        m_attachedShots.clear();
    }

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
