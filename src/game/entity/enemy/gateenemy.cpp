#include "gateenemy.h"
#include "game/coggame.h"
#include "game/cogscript.h"
#include "game/gamescreen.h"
#include "game/entity/ambiententity.h"
#include "game/entity/playerentity.h"
#include "engine/primitive.h"
#include "engine/graphics/particlemodule.h"
#include "engine/graphics/shadermodule.h"
#include "csm/csm.h"
#include <glm/gtx/rotate_vector.hpp>

using namespace COG;

GateEnemy::GateEnemy(float beat)
    : EnemyEntity(beat, 1, 0)
{
    GAME->controller()->ambience()->doFade(true);
    for (int i = 0; i < 8; ++i)
        GAME->controller()->parent()->addEntity(GameScreen::LAYER_ENEMIES, new GateSubEnemy(beat, this, i));
}

void GateEnemy::performAction(COGScriptAction *action) {
    switch (action->action) {
    case Action::COMMAND: {
        COGScriptActionCommand *command = (COGScriptActionCommand *) action;
        if (command->command == "crash") {
            GAME->controller()->ambience()->doFade(false);
            GAME->controller()->ambience()->zone(GAME->controller()->ambience()->zone()+1);
            if (m_opened) {
                GAME->controller()->makeParticles(16*256, m_position, 3.f, glm::vec3(0.f), glm::vec3(1.f), glm::vec2(1.f, 8.f));
            } else {
                GAME->controller()->dealDamage();
                GAME->controller()->makeParticles(16*256, m_position, 3.f, glm::vec3(0.f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 8.f));
            }
        } else
            break;
        return;
    }
    }

    EnemyEntity::performAction(action);
}

void GateEnemy::tickBeats(float beats) {
    m_targetable = false;

    // Fancy particle shit
    if (totalBeats() < 2.f) {
        int amount = 256;
        glm::vec3 basePos = totalBeats()*0.5f*m_position;
        glm::vec3 baseDist = (6.f-2.f*totalBeats())*glm::vec3(0.f, 1.f, 0.f);
        float baseAng = totalBeats()*-0.25f*glm::pi<float>();

        GLfloat *pos = new GLfloat[4*amount];
        GLfloat *vel = new GLfloat[3*amount];
        GLfloat *col = new GLfloat[3*amount];
        for (int i = 0; i < amount; ++i) {
            float ang = csm::rand(0, 8)*0.25f * glm::pi<float>() + baseAng;
            glm::vec3 position = basePos + glm::rotate(baseDist, ang, glm::vec3(-1.f, 0.f, 0.f));
            pos[4*i+0] = position.x;
            pos[4*i+1] = position.y;
            pos[4*i+2] = position.z;
            pos[4*i+3] = (float) rand() / RAND_MAX + 1.f;
        }
        for (int i = 0; i < amount; ++i) {
            vel[3*i+0] = 0.8f * (float) rand() / RAND_MAX - 0.4f;
            vel[3*i+1] = 0.8f * (float) rand() / RAND_MAX - 0.4f;
            vel[3*i+2] = 0.8f * (float) rand() / RAND_MAX - 0.4f;
        }
        for (int i = 0; i < amount; ++i) {
            col[3*i+0] = 1.f;
            col[3*i+1] = 0.75f;
            col[3*i+2] = 0.f;
        }
        graphics().particle()->putParticles(amount, pos, vel, col);
        delete[] pos;
        delete[] vel;
        delete[] col;
    }

    EnemyEntity::tickBeats(beats);
}

void GateEnemy::draw(int pass, float beat) {
    if (pass != GameScreen::DRAW_ADDITIVE)
        return;

    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture("enemy2");
    bool allOpen = true;
    for (int i = 0; i < 8; ++i) {
        glm::mat4 m(1.f);
        m = glm::translate(m, m_position);
        m = glm::scale(m, glm::vec3(4.f));
        m = glm::rotate(m, 0.25f*i*glm::pi<float>(), glm::vec3(-1.f, 0.f, 0.f));
        m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
        if (m_open[i])
            graphics().shader()->color(glm::vec4(0.f, 1.f, 0.f, 1.f));
        else {
            graphics().shader()->color(glm::vec4(1.f, 0.f, 0.f, 1.f));
            allOpen = false;
        }
        graphics().shader()->mTransform(m);
        graphics().getPrimitive("gate3")->drawArray();
    }

    if (allOpen && !m_opened) {
        m_opened = true;
        GAME->controller()->makeParticles(4*256, m_position, glm::vec3(0.f, 1.f, 1.f), m_velocity, glm::vec3(1.f), glm::vec2(1.f, 4.f));
    }

    graphics().shader()->color(glm::vec4(1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(4.f));
    graphics().shader()->mTransform(m);
    if (m_opened)
        graphics().getPrimitive("gate2")->drawArray();
    else
        graphics().getPrimitive("gate1")->drawArray();
}

csm::ellipsoid GateEnemy::getEllipsoid() const {
    return csm::ellipsoid();
}

GateSubEnemy::GateSubEnemy(float beat, GateEnemy *parent, int id)
    : EnemyEntity(beat, 1, 0)
    , m_parent(parent)
    , m_id(id)
    , m_rotation(0.25f*m_id*glm::pi<float>())
{ }

void GateSubEnemy::tickBeats(float beats) {
    giveBehavior(m_parent->behavior());
    m_position = m_parent->position() + glm::rotate(glm::vec3(0.f, 2.f, 0.f), m_rotation, glm::vec3(-1.f, 0.f, 0.f));
    EnemyEntity::tickBeats(beats);
}

void GateSubEnemy::draw(int pass, float beat) {
    if (pass != GameScreen::DRAW_ADDITIVE)
        return;

    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture("enemy2");
    graphics().shader()->color(glm::vec4(1.f, 0.75f, 0.f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(4.f));
    m = glm::rotate(m, m_rotation, glm::vec3(-1.f, 0.f, 0.f));
    graphics().shader()->mTransform(m);
    graphics().getPrimitive("gate4")->drawArray();
}

csm::ellipsoid GateSubEnemy::getEllipsoid() const {
    return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.5f));
}

void GateSubEnemy::deathEffect(float beat) {
    int i = csm::rand(0, 2);
    if      (i == 0) audio().queueSoundOnBeat("xylophone-e3.wav", beat+0.25f);
    else if (i == 1) audio().queueSoundOnBeat("xylophone-g3.wav", beat+0.25f);
    m_parent->m_open[m_id] = true;
    GAME->controller()->makeParticles(256, m_position, 0.25f, m_parent->velocity(), glm::vec3(1.f, 0.75f, 0.f), glm::vec2(1.f, 2.f));
}
