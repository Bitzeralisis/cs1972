#include "lightentity.h"
#include "game/gamescreen.h"
#include "engine/graphics.h"
#include "engine/graphics/deferredmodule.h"

using namespace COG;

LightEntity::LightEntity(float beat, glm::vec3 pos, glm::vec3 vel)
    : COGEntity(beat)
{
    m_position = pos;
    m_velocity = vel;
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
    graphics().deferred()->lightPoint(m_position, glm::vec3(m_life), glm::vec3(1.f, 2.f/1.f, 1.f/1.f));
}
