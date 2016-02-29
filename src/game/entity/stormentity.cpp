#include "stormentity.h"
#include "playershotentity.h"

using namespace Minecraft;

StormEntity::StormEntity(glm::vec3 pos, glm::vec3 dir)
    : MinecraftEntity(Type::PLAYER_ATTACK)
{
    m_position = pos;
    m_velocity = 3.f*glm::normalize(dir);
    m_collides = false;
}

void StormEntity::tick(float seconds) {
    m_lifetime -= seconds;
    if (m_lifetime <= 0.f)
        parent()->deleteEntity(this);

    while (m_shots > m_lifetime) {
        m_shots -= TIME_PER_SHOT;
        float yaw = 2.f*glm::pi<float>() * (float)rand2() / RAND2_MAX;
        // Make rand2 "more random"
        rand2(); rand2(); rand2();
        float pitch = glm::pi<float>() * ((float)rand2() / RAND2_MAX - 0.5f);
        parent()->addEntity(new PlayerShotEntity(m_position, 10.f*glm::vec3(glm::cos(yaw)*glm::cos(pitch), glm::sin(pitch), glm::sin(yaw)*glm::cos(pitch))));
    }

    tickPhysicsDiscrete(seconds);
}

void StormEntity::draw() { }

glm::vec2 StormEntity::getCylinder() const { return glm::vec2(0.f); }

csm::aabb StormEntity::getAabb() const { return csm::aabb(glm::vec3(-0.2f), glm::vec3(0.4f)); }

void StormEntity::collideTerrain(glm::vec3) { }

void StormEntity::collide(glm::vec3, const MinecraftEntity *) { }
