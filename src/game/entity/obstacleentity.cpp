#include "obstacleentity.h"
#include "../../engine/graphics.h"

using namespace WarmupGame;

ObstacleEntity::ObstacleEntity(glm::vec3 pos, glm::vec2 size)
    : WarmupEntity(Type::OBSTACLE)
{
    m_position = pos;
    m_static = true;
    m_hitbox = size;
}

void ObstacleEntity::tick(float seconds) {
    tickPhysicsDiscrete(seconds);
}

void ObstacleEntity::draw() {
    graphics().shaderColor(glm::vec3(1.f, 0.f, 1.f));
    graphics().shaderUseTexture(true);
    graphics().shaderBindTexture(graphics().getTexture("grass"));
    glm::mat4 m = glm::mat4(1.f);
    m = glm::translate(glm::mat4(1.f), m_position);
    m = glm::scale(m, glm::vec3(2.f*m_hitbox.x, m_hitbox.y, 2.f*m_hitbox.x));
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawCylinder();
}

glm::vec2 ObstacleEntity::getCylinder() const {
    return m_hitbox;
}

void ObstacleEntity::collide(glm::vec3, const WarmupEntity *) { }
