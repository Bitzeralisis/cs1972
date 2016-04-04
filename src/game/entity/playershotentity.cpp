#include "playershotentity.h"

using namespace Platformer;

PlayerShotEntity::PlayerShotEntity(glm::vec3 pos, glm::vec3 vel)
    : PlatformerEntity(Type::PLAYER_ATTACK)
{
    m_position = pos;
    m_velocity = vel;
    m_static = true;
}

void PlayerShotEntity::tick(float seconds) {
    m_life -= seconds;
    if (m_life <= 0.f)
        parent()->deleteEntity(this);

    tickPhysicsDiscrete(seconds);
}

void PlayerShotEntity::draw(int pass) {
    switch (pass) {
    case 1:
        graphics().dr_lightPoint(m_position, glm::vec3(2.f, 0.f, 2.f), glm::vec3(1.f, 1.f, 2.4f));
        graphics().dr_drawLight();
        break;

    case 2:
        graphics().shaderUseLight(false);
        graphics().shaderColor(glm::vec4(1.f, 0.f, 1.f, 1.f));
        glm::mat4 m(1.f);
        m = glm::translate(m, m_position);
        m = glm::scale(m, glm::vec3(0.25f));
        graphics().shaderMTransform(m);
        graphics().drawSphere();
        graphics().shaderUseLight(true);
        break;
    }
}

void PlayerShotEntity::collideTerrain(const glm::vec3&, const glm::vec3&) {
    parent()->deleteEntity(this);
}

void PlayerShotEntity::collide(glm::vec3 mtv, const PlatformerEntity *other) { }
