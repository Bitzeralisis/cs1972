#include "testentity.h"

using namespace Minecraft;

TestEntity::TestEntity(glm::vec3 pos) {
    m_position = pos;
    m_accel = glm::vec3(0.f, GRAVITY_ACCEL, 0.f);
}

void TestEntity::walk(glm::vec3 walk, bool dashing, bool jumping) {
    glm::vec3 move(0.f);

    if (walk != glm::vec3(0.f)) {
        walk = glm::normalize(walk) * (dashing ? DASH_VELOCITY : WALK_VELOCITY);

        // Hijack camera walk function to walk in eye direction
        glm::vec3 pos = graphics().camera->position();
        graphics().camera->position(m_position);
        graphics().camera->walk(walk);
        move = (graphics().camera->position() - m_position);
        graphics().camera->position(pos);
    }

    // Set position changes to velocity so collision detection works
    m_velocity.x = move.x;
    m_velocity.z = move.z;
    if (m_standing && jumping)
        m_velocity.y = JUMP_VELOCITY;
}

void TestEntity::tick(float seconds) {
    tickPhysicsDiscrete(seconds);
}

void TestEntity::draw() { }

glm::vec2 TestEntity::getCylinder() const {
    return glm::vec2(0.5f, 2.f);
}

csm::aabb TestEntity::getAabb() const {
    return csm::aabb(glm::vec3(-0.5f, 0.f, -0.5f), glm::vec3(1.f, 2.f, 1.f));
}

void TestEntity::collideTerrain(glm::vec3 tv) {
    if (tv.y > 0.f) {
        // Translated up due to terrain means we are "standing" on ground
        m_velocity.y = 0.f;
        m_standing = true;
    } else {
        m_standing = false;
    }
}

void TestEntity::collide(glm::vec3 mtv, const Entity *other) { }
