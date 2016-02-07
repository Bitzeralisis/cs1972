#include "playerentity.h"
#include "../../engine/graphics.h"

using namespace WarmupGame;

PlayerEntity::PlayerEntity() {
    // Constant gravity
    m_accel.y = FALL_ACCEL;
}

void PlayerEntity::move(glm::vec3 walk, bool jumping, bool dashing) {
    // Figure how far we need to walk in camera space
    if (walk != glm::vec3(0.f)) {
        walk = glm::normalize(walk);
        if (dashing) {
            walk *= DASH_SPEED;
            if (m_dash < 5) ++m_dash;
        } else {
            walk *= RUN_SPEED;
            if (m_dash > 0) --m_dash;
        }
    } else {
        if (m_dash > 0) --m_dash;
    }

    // Hijack the camera's walk function
    glm::vec3 pos = graphics().camera->position();
    graphics().camera->position(m_position);
    graphics().camera->walk(walk);
    m_position = graphics().camera->position();
    graphics().camera->position(pos);

    // Jump on ground if space held
    if (m_onGround && jumping) {
        m_velocity.y = dashing ? DASH_JUMP_SPEED : JUMP_SPEED;
    }
}

void PlayerEntity::tick() {
    tickPhysicsDiscrete();

    m_onGround = false;

    // Can't fall through floor
    if (m_position.y < 0.f) {
        m_position.y = 0.f;
        m_velocity.y = 0.f;
        m_onGround = true;
    }
}

void PlayerEntity::draw() {
    graphics().shaderColor(glm::vec3(0.f, 1.f, 0.f));
    graphics().shaderUseTexture(false);
    glm::mat4 m = glm::mat4(1.f);
    m = glm::translate(glm::mat4(1.f), m_position);
    m = glm::scale(m, SCALE_VECTOR);
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawCylinder();
}

glm::vec2 PlayerEntity::getCylinder() const {
    return PLAYER_HITBOX;
}

void PlayerEntity::collide(glm::vec3 mtv, const Entity *other) {
    m_position += mtv;

    // Upwards mtv means we are "standing on something"
    if (mtv.y > 0.f) {
        m_velocity.y = 0.f;
        m_onGround = true;
    }
}
