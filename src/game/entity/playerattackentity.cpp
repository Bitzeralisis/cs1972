#include "playerattackentity.h"

using namespace Minecraft;

PlayerAttackEntity::PlayerAttackEntity(glm::vec3 pos, glm::vec3 dir)
    : MinecraftEntity(Type::PLAYER_ATTACK)
{
    m_position = pos;
    m_velocity = 20.f*glm::normalize(dir);
    m_static = true;
    m_collidesTerrain = false;
}

void PlayerAttackEntity::tick(float seconds) {
    m_lifetime -= seconds;
    if (m_lifetime <= 0.f)
        parent()->deleteEntity(this);
    tickPhysicsDiscrete(seconds);
}

void PlayerAttackEntity::draw() { }

glm::vec2 PlayerAttackEntity::getCylinder() const {
    return glm::vec2(0.5f, 2.f);
}

csm::aabb PlayerAttackEntity::getAabb() const {
    return csm::aabb(glm::vec3(-0.5f, 0.f, -0.5f), glm::vec3(1.f, 2.f, 1.f));
}

void PlayerAttackEntity::collideTerrain(glm::vec3) { }

void PlayerAttackEntity::collide(glm::vec3, const MinecraftEntity *other) {
    if (other->type() == Type::ENEMY) {
        m_collides = false;
    }
}
