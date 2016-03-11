#include "playershotentity.h"

using namespace Minecraft;

PlayerShotEntity::PlayerShotEntity(glm::vec3 pos, glm::vec3 vel)
    : MinecraftEntity(Type::PLAYER_ATTACK)
{
    m_position = pos;
    m_velocity = vel;
    m_static = true;
    m_collidesTerrain = false;
}

void PlayerShotEntity::tick(float seconds) {
    m_lifetime -= seconds;
    if (m_lifetime <= 0.f)
        parent()->deleteEntity(this);
    tickPhysicsDiscrete(seconds);
}

void PlayerShotEntity::draw() {
    graphics().shaderUseTexture(false);
    graphics().shaderColor(glm::vec4(1.f, 0.f, 1.f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::rotate(m, 4.f*m_lifetime, glm::vec3(1.f));
    m = glm::scale(m, glm::vec3(0.4f));
    graphics().shaderMTransform(m);
    graphics().drawBox();
}

glm::vec2 PlayerShotEntity::getCylinder() const {
    return glm::vec2(0.2f, 0.4f);
}

csm::aabb PlayerShotEntity::getAabb() const {
    return csm::aabb(glm::vec3(-0.2f), glm::vec3(0.4f));
}

void PlayerShotEntity::collideTerrain(glm::vec3 mtv) {
    if (glm::length2(mtv) > 0.0000001f)
        parent()->deleteEntity(this);
}

void PlayerShotEntity::collide(glm::vec3, const MinecraftEntity *other) {
    if (other->type() == Type::ENEMY) {
        m_collides = false;
        parent()->deleteEntity(this);
    }
}
