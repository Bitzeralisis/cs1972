#include "playerentity.h"
#include "game/cogscript.h"
#include "game/entity/enemyshotentity.h"
#include "engine/graphics/particlemodule.h"

using namespace COG;

PlayerEntity::PlayerEntity()
    : ControlledEntity(0.f)
{ }

void PlayerEntity::gainScoreValue(int score) {
    m_score += m_combo*score;
}

void PlayerEntity::makeParticles(int amount, glm::vec3 position, float width, glm::vec3 velocity, glm::vec3 color, glm::vec2 life) {
    GLfloat *pos = new GLfloat[4*amount];
    GLfloat *vel = new GLfloat[3*amount];
    GLfloat *col = new GLfloat[3*amount];
    for (int i = 0; i < amount; ++i) {
        pos[4*i+0] = width * (float) rand() / RAND_MAX - width*0.5f + position.x;
        pos[4*i+1] = width * (float) rand() / RAND_MAX - width*0.5f + position.y;
        pos[4*i+2] = width * (float) rand() / RAND_MAX - width*0.5f + position.z;
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

    default:
        break;
    }

    ControlledEntity::performAction(action);
}

void PlayerEntity::tickBeats(float beats) {
    ControlledEntity::tickBeats(beats);
}
