#include "enemyentity.h"

using namespace Platformer;

EnemyEntity::EnemyEntity(glm::vec3 pos)
    : PlatformerEntity(Type::ENEMY)
{
    m_position = pos;
    m_accel = GRAVITY_ACCEL;
}

void EnemyEntity::walk(float, glm::vec3 walk, bool jumping) {
    walk.y = 0.f;
    if (walk != glm::vec3(0.f))
        walk = glm::normalize(walk);

    if (m_standing) {
        walk *= WALK_VELOCITY;

        m_velocity.x = walk.x;
        m_velocity.z = walk.z;

        if (jumping)
            m_velocity.y = JUMP_VELOCITY;

        m_accel.x = 0.f;
        m_accel.z = 0.f;
    } else {
        walk *= WALK_VELOCITY;
        glm::vec3 goal = 2.f*(walk-m_velocity);

        m_accel.x = goal.x;
        m_accel.z = goal.z;
    }
}

void EnemyEntity::tick(float seconds) {
    tickPhysicsDiscrete(seconds);
    m_standing = false;
}

void EnemyEntity::draw(int pass) {
    if (pass != 0)
        return;

    graphics().shaderUseTexture(false); graphics().shaderColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(1.f, 2.f, 1.f));
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawSphere();
}

void EnemyEntity::collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) {
    if (normal.y > 0.f && m_velocity.y < 0.f) {
        m_velocity.y = BASE_DOWNWARD_VELOCITY;
        m_standing = true;
    } else {
        m_standing = false;
    }
}

void EnemyEntity::collide(glm::vec3 mtv, const PlatformerEntity *other) {

}
