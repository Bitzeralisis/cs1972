#include "playerentity.h"
#include "game/cogscript.h"
#include "game/entity/ambiententity.h"
#include "game/entity/enemyshotentity.h"
#include "engine/graphics/particlemodule.h"

using namespace COG;

PlayerEntity::PlayerEntity()
    : ControlledEntity(0.f)
{ }

void PlayerEntity::gainScoreValue(int score) {
    m_score += m_combo*score;
}

void PlayerEntity::gainPotentialScore(int score) {
    m_potentialScore += score;
}

void PlayerEntity::gainShotDown(int shotDown) {
    m_shotDown += shotDown;
}

void PlayerEntity::gainPotentialShotDown(int shotDown) {
    m_potentialShotDown += shotDown;
}

void PlayerEntity::makeParticles(int amount, glm::vec3 position, float width, glm::vec3 velocity, glm::vec3 color, glm::vec2 life) {
    makeParticles(amount, position, glm::vec3(width), velocity, color, life);
}

void PlayerEntity::makeParticles(int amount, glm::vec3 position, glm::vec3 size, glm::vec3 velocity, glm::vec3 color, glm::vec2 life) {
    GLfloat *pos = new GLfloat[4*amount];
    GLfloat *vel = new GLfloat[3*amount];
    GLfloat *col = new GLfloat[3*amount];
    for (int i = 0; i < amount; ++i) {
        pos[4*i+0] = size.x * (float) rand() / RAND_MAX - size.x*0.5f + position.x;
        pos[4*i+1] = size.y * (float) rand() / RAND_MAX - size.y*0.5f + position.y;
        pos[4*i+2] = size.z * (float) rand() / RAND_MAX - size.z*0.5f + position.z;
        pos[4*i+3] = (life.y-life.x) * (float) rand() / RAND_MAX + life.x;
    }
    for (int i = 0; i < amount; ++i) {
        vel[3*i+0] = 0.8f * (float) rand() / RAND_MAX - 0.4f + velocity.x;
        vel[3*i+1] = 0.8f * (float) rand() / RAND_MAX - 0.4f + velocity.y;
        vel[3*i+2] = 0.8f * (float) rand() / RAND_MAX - 0.4f + velocity.z;
    }
    for (int i = 0; i < amount; ++i) {
        col[3*i+0] = color.x;
        col[3*i+1] = color.y;
        col[3*i+2] = color.z;
    }
    graphics().particle()->putParticles(amount, pos, vel, col);
    delete[] pos;
    delete[] vel;
    delete[] col;
}

void PlayerEntity::attachShot(EnemyShotEntity *shot) {
    if (m_attachedShots.size() && m_attachedShots.back()->hitBeat() == shot->hitBeat() && m_attachedShots.back()->approachLane() == shot->approachLane())
        shot->detatchShot();
    else
        m_attachedShots.push_back(shot);
}

void PlayerEntity::performAction(COGScriptAction *action) {
    switch (action->action) {
    case Action::COMMAND: {
        COGScriptActionCommand *command = (COGScriptActionCommand *) action;
        if (command->command == "win") m_win = true;
        else
            break;
        return;
    }

    case Action::SET_ATTRIBUTE: {
        COGScriptActionSetAttribute *act = (COGScriptActionSetAttribute *) action;
        switch (act->attr) {
        case Attribute::FLOAT: {
            COGScriptAttributeFloat *attr = (COGScriptAttributeFloat *) act;
            if      (attr->key == "fog.near")           m_minFog = attr->value;
            else if (attr->key == "fog.far")            m_maxFog = attr->value;
            else if (attr->key == "camera.special")     m_cameraBehavior = attr->value;
            else if (attr->key == "camera.yawbase")     m_baseYaw = attr->value;
            else if (attr->key == "camera.pitchbase")   m_basePitch = attr->value;
            else if (attr->key == "camera.yawlimit")    m_yawLimit = attr->value;
            else if (attr->key == "camera.pitchlimit")  m_pitchLimit = attr->value;
            else if (attr->key == "zone")               m_ambience->zone(attr->value);
            else
                break;
            return;
        }

        default:
            break;
        }

        break;
    }

    default:
        break;
    }

    ControlledEntity::performAction(action);
}

void PlayerEntity::tickBeats(float beats) {
    ControlledEntity::tickBeats(beats);
}
