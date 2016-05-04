#include "gateenemy.h"
#include "game/coggame.h"
#include "game/cogscript.h"
#include "game/gamescreen.h"
#include "game/entity/ambiententity.h"
#include "game/entity/playerentity.h"
#include "engine/primitive.h"
#include "engine/graphics/shadermodule.h"
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
                GAME->controller()->makeParticles(16*256, m_position, 3.f, GAME->controller()->velocity(), glm::vec3(1.f), glm::vec2(1.f, 8.f));
            } else {
                GAME->controller()->dealDamage();
                GAME->controller()->makeParticles(16*256, m_position, 3.f, GAME->controller()->velocity(), glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 8.f));
            }
        } else
            break;
        return;
    }
    }

    EnemyEntity::performAction(action);
}

void GateEnemy::tickBeats(float beats) {
    targetable(false);
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

void GateEnemy::hitEffect(float) { }

void GateEnemy::deathEffect(float beat) {
    //audio().queueSoundOnBeat("noisy1.aif", beat+0.25f);
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

void GateSubEnemy::hitEffect(float) { }

void GateSubEnemy::deathEffect(float beat) {
    //audio().queueSoundOnBeat("noisy1.aif", beat+0.25f);
    m_parent->notifyOpening(m_id);
    GAME->controller()->makeParticles(256, m_position, 0.25f, m_parent->velocity(), glm::vec3(1.f, 0.75f, 0.f), glm::vec2(1.f, 2.f));
}
