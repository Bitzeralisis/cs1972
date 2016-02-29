#include "enemyentity.h"

using namespace Minecraft;

EnemyEntity::EnemyEntity(PlayerEntity *target, glm::vec3 pos)
    : MinecraftEntity(Type::ENEMY)
    , m_target(target)
{
    m_position = pos;
    m_accel = glm::vec3(0.f, GRAVITY_ACCEL, 0.f);
}

void EnemyEntity::attack() {
    m_attacking = true;
}

void EnemyEntity::runAway(){
    m_attacking = false;
}

void EnemyEntity::tick(float seconds) {
    // Walk and jump towards / away from the player
    glm::vec3 dpos = m_target->position() - m_position;

    if (!m_attacking)
        dpos *= -1.f;

    glm::vec2 walk(dpos.x, dpos.z);
    if (walk != glm::vec2(0.f))
        walk = glm::normalize(walk);

    if (m_standing) {
        walk *= WALK_VELOCITY;

        m_velocity.x = walk.x;
        m_velocity.z = walk.y;
        m_velocity.y = JUMP_VELOCITY;

        m_accel.x = 0.f;
        m_accel.z = 0.f;
    } else {
        walk *= WALK_VELOCITY;
        glm::vec2 curr(m_velocity.x, m_velocity.z);
        glm::vec2 goal = 2.f*(walk-curr);

        m_accel.x = goal.x;
        m_accel.z = goal.y;
    }

    tickPhysicsDiscrete(seconds);
}

void EnemyEntity::draw() {
    graphics().shaderUseTexture(false);
    graphics().shaderColor(glm::vec4(1.f, 0.f, 1.f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(1.f, 1.6f, 1.f));
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawCylinder();
}

glm::vec2 EnemyEntity::getCylinder() const {
    return glm::vec2(0.5f, 2.f);
}

csm::aabb EnemyEntity::getAabb() const {
    return csm::aabb(glm::vec3(-0.5f, 0.f, -0.5f), glm::vec3(1.f, 2.f, 1.f));
}

void EnemyEntity::collideTerrain(glm::vec3 tv) {
    if ((tv.x > 0.01f && m_velocity.x < 0.f) || (tv.x < -0.01f && m_velocity.x > 0.f)) m_velocity.x = 0.f;
    if (/*(tv.y > 0.01f && m_velocity.y < 0.f) ||*/ (tv.y < -0.01f && m_velocity.y > 0.f)) m_velocity.y = 0.f;
    if ((tv.z > 0.01f && m_velocity.z < 0.f) || (tv.z < -0.01f && m_velocity.z > 0.f)) m_velocity.z = 0.f;

    if (tv.y > 0.f && m_velocity.y < 0.f) {
        // Translated up due to falling onto terrain means we are "standing" on ground
        m_velocity.y = 0.f;
        m_standing = true;
    } else {
        m_standing = false;
    }
}

void EnemyEntity::collide(glm::vec3 mtv, const MinecraftEntity *other) {
    // Standing on an entity that may be standing on the floor, so stop our downwards velocity
    if (mtv.y > 0.f)
        m_velocity.y = 0.f;

    if (other->type() == Type::PLAYER_ATTACK) {
        m_dead = true;
        parent()->removeEntity(this);
    }
}
