#include "entity.h"

using namespace CS1972Engine;

void Entity::tickPhysicsDiscrete(float seconds) {
    m_velocity += seconds * m_accel;
    m_position += seconds * m_velocity;
}

void Entity::checkCollide(Entity *other) {
    glm::vec3& thisPos = m_position;
    glm::vec3& thatPos = other->m_position;
    glm::vec2 thisCyl = getCylinder();
    glm::vec2 thatCyl = other->getCylinder();

    glm::vec2 thisY(thisPos.y, thisPos.y+thisCyl.y);
    glm::vec2 thatY(thatPos.y, thatPos.y+thatCyl.y);

    // Collides in y-dimension if their y-dimension projections (two lines) collide
    bool yCollision = thisY.x < thatY.y && thatY.x < thisY.y;
    if (!yCollision) return;

    // Collides in xz-plane if the distance between the projections (two circles) is less than the sum of radii
    glm::vec2 xzDelta = glm::vec2(thisPos.x, thisPos.z) - glm::vec2(thatPos.x, thatPos.z);
    float radii = thisCyl.x + thatCyl.x;
    bool xzCollision = xzDelta.x*xzDelta.x + xzDelta.y*xzDelta.y < radii*radii;
    if (!xzCollision) return;

    // The cylinders collide; figure out MTV
    float yMtv1 = thatY.y - thisY.x;
    float yMtv2 = thisY.y - thatY.x;
    float yMtv = yMtv1 < yMtv2 ? yMtv1 : -1.f*yMtv2;

    float xzMtv = radii - glm::sqrt(xzDelta.x*xzDelta.x + xzDelta.y*xzDelta.y);

    glm::vec3 totalMtv;
    if (glm::abs(yMtv) <= glm::abs(xzMtv) || xzDelta == glm::vec2(0.f))
        totalMtv = glm::vec3(0.f, yMtv, 0.f);
    else
        totalMtv = xzMtv * glm::normalize(glm::vec3(xzDelta.x, 0.f, xzDelta.y));

    // Resolve collisions
    glm::vec3 thisMtv(0.f);
    glm::vec3 thatMtv(0.f);
    if (m_static && other->m_static) {
        // noop
    } else if (m_static)
        thatMtv = -1.f*totalMtv;
    else if (other->m_static)
        thisMtv = totalMtv;
    else {
        thisMtv = 0.5f*totalMtv;
        thatMtv = -0.5f*totalMtv;
    }

    m_position += thisMtv;
    other->m_position += thatMtv;

    // Send response callbacks
    collide(thisMtv, other);
    other->collide(thatMtv, this);
}
