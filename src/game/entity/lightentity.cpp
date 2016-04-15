#include "lightentity.h"
#include "engine/graphics.h"
#include "engine/graphics/deferredmodule.h"

using namespace COG;

LightEntity::LightEntity(glm::vec3 pos, glm::vec3 vel) {
    m_position = pos;
    m_velocity = vel;
    m_collides = false;
}

void LightEntity::tick(float seconds) {
    m_life -= seconds;
    if (m_life <= 0.f) {
        parent()->deleteEntity(this);
    }
}

void LightEntity::draw(int pass) {
    graphics().deferred()->lightPoint(m_position, glm::vec3(m_life), glm::vec3(1.f, 2.f/1.f, 1.f/1.f));
}
