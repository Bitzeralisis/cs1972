#include "itementity.h"

using namespace WarmupGame;

ItemEntity::ItemEntity(bool isGood, glm::vec3 pos, glm::vec3 vel)
    : WarmupEntity(isGood ? Type::GOOD_ITEM : Type::BAD_ITEM)
{
    m_position = pos;
    m_velocity = vel;
}

void ItemEntity::tick(float seconds) {
    tickPhysicsDiscrete(seconds);

    // Once they reach the bottom floor, they stop and stick
    if (m_position.y < 0.f) {
        m_position.y = 0.f;
        m_velocity = glm::vec3(0.f);
        m_decay = DECAY_TIME;
    }

    if (m_decay > 0.f) {
        m_decay -= seconds;
        if (m_decay <= 0.f)
            parent()->deleteEntity(this);
    }
}

void ItemEntity::draw() {
    if (m_type == Type::BAD_ITEM)
        graphics().shaderColor(glm::vec3(1.f, 0.f, 0.f));
    else
        graphics().shaderColor(glm::vec3(0.f, 1.f, 0.f));

    graphics().shaderUseTexture(false);
    glm::mat4 m = glm::mat4(1.f);
    m = glm::translate(glm::mat4(1.f), m_position);
    m = glm::scale(m, SCALE_VECTOR);
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawCylinder();
}

glm::vec2 ItemEntity::getCylinder() const {
    return HITBOX;
}

void ItemEntity::collide(glm::vec3 mtv, const WarmupEntity *other) {
    switch (other->type()) {
    case Type::OBSTACLE:
        if (mtv.y == 0.f) {
            // Bounce off edge of obstacles. Math stolen from stackoverflow
            glm::vec2 xzVel(m_velocity.x, m_velocity.z);
            glm::vec2 xzMtv = glm::normalize(glm::vec2(mtv.x, mtv.z));
            glm::vec2 xzRef = xzVel - 2.f * (glm::dot(xzVel, xzMtv)) * xzMtv;
            m_velocity.x = xzRef.x;
            m_velocity.z = xzRef.y;
        }
        break;

    case Type::PLAYER:
        // Eaten by player
        parent()->deleteEntity(this);
        break;

    default:
        break;
    }
}
