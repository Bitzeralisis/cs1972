#include "lightentity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "game/entity/playerentity.h"
#include "engine/graphics.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/particlemodule.h"

using namespace COG;

LightEntity::LightEntity(float beat, glm::vec3 pos, glm::vec3 vel)
    : COGEntity(beat)
{
    m_position = pos;
    m_velocity = vel;
    GAME->controller()->makeParticles(pos, vel);
}

void LightEntity::tickBeats(float beats) {
    m_life -= beats;
    if (m_life <= 0.f) {
        parent()->deleteEntity(this);
    }
    tickPhysicsContinuous(beats);
}

void LightEntity::draw(int pass) {
    if (pass != GameScreen::DRAW_LIGHTS)
        return;
    graphics().deferred()->lightPoint(m_position, glm::vec3(m_life), 0.5f);
}
