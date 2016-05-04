#include "controlledentity.h"
#include "enemyshotentity.h"
#include "enemy/cubeaenemy.h"
#include "enemy/diamondaenemy.h"
#include "game/cogscript.h"
#include "game/gamescreen.h"
#include "engine/sound.h"

using namespace COG;

ControlledEntity::ControlledEntity(float beat)
    : COGEntity(beat)
{ }

ControlledEntity::~ControlledEntity() { }

void ControlledEntity::performAction(COGScriptAction *action) {
    switch (action->action) {
    case Action::COMMAND: {
        COGScriptActionCommand *command = (COGScriptActionCommand *) action;
        if (command->command == "disappear") parent()->deleteEntity(this);
        else {
            std::cerr << "Warning: Attempted to perform command " << command->command << " on type missing that command" << std::endl;
            break;
        }
        break;
    }

    case Action::SET_ATTRIBUTE: {
        COGScriptActionSetAttribute *act = (COGScriptActionSetAttribute *) action;
        switch (act->attr) {
        case Attribute::POSITION:
        case Attribute::VELOCITY:
        case Attribute::ACCELERATION: {
            COGScriptAttributeSpatial *attr = (COGScriptAttributeSpatial *) act;
            glm::vec3 *space;
            if      (attr->attr == Attribute::POSITION)     space = &m_position;
            else if (attr->attr == Attribute::VELOCITY)     space = &m_velocity;
            else if (attr->attr == Attribute::ACCELERATION) space = &m_accel;
            if (!glm::isnan(attr->coord.x)) space->x = attr->coord.x + (attr->relative ? space->x : 0.f);
            if (!glm::isnan(attr->coord.y)) space->y = attr->coord.y + (attr->relative ? space->y : 0.f);
            if (!glm::isnan(attr->coord.z)) space->z = attr->coord.z + (attr->relative ? space->z : 0.f);
            break;
        }

        case Attribute::FLOAT: {
            COGScriptAttributeFloat *attr = (COGScriptAttributeFloat *) act;
            std::cerr << "Warning: Attempted to set attribute float " << attr->key << " on type missing that attribute" << std::endl;
            break;
        }

        default:
            std::cerr << "Warning: Attempted to set attribute " << (int) act->attr << " on type missing that attribute" << std::endl;
            break;
        }
        break;
    }

    case Action::SPAWN: {
        COGScriptActionSpawn *act = (COGScriptActionSpawn *) action;
        EnemyEntity *enemy;

        if      (act->type == "cubea")    enemy = new CubeaEnemy(act->beat + firstBeat());
        else if (act->type == "diamonda") enemy = new DiamondaEnemy(act->beat + firstBeat());
        else {
            std::cerr << "Warning: Attempted to spawn unrecognized enemy type " << act->type << std::endl;
            break;
        }

        enemy->m_position = m_position;
        enemy->giveBehavior(m_behavior->parent->behaviors[act->behavior]);
        parent()->addEntity(GameScreen::LAYER_ENEMIES, enemy);
        for (std::deque<COGScriptAction *>::iterator it = act->init->actions.begin(); it != act->init->actions.end(); ++it)
            enemy->performAction(*it);
        break;
    }

    case Action::PLAY_MUSIC: {
        COGScriptActionPlay *act = (COGScriptActionPlay *) action;
        CS1972Engine::Sound *s;
        if (audio().hasSound(act->sound))
            s = audio().getSound(act->sound);
        else {
            s = audio().createSoundStream(act->sound.data());
            audio().putSound(act->sound, s);
        }
        s->setMusicParams(act->bpm, act->offset);
        s->setLoop(true);
        s->setLoopBeats(act->loopStart, act->loopEnd);
        audio().queueBgmOnBeat(s, act->beat);
        break;
    }

    default:
        std::cerr << "Warning: Attempted to perform action " << (int) action->action << " on type missing that action" << std::endl;
        break;
    }
}

void ControlledEntity::tickBeats(float beats) {
    float startBeat = totalBeats()-beats;
    if (m_behavior) {
        while (m_step < m_behavior->actions.size() && (m_behavior->actions[m_step]->beat <= totalBeats() || m_behavior->actions[m_step]->queued)) {
            COGScriptAction *action = m_behavior->actions[m_step];
            if (action->action == Action::SET_ATTRIBUTE) {
                tickPhysicsContinuous(m_behavior->actions[m_step]->beat - startBeat);
                startBeat = m_behavior->actions[m_step]->beat;
            }
            performAction(m_behavior->actions[m_step]);
            ++m_step;
        }
    }
    tickPhysicsContinuous(totalBeats()-startBeat);
}
