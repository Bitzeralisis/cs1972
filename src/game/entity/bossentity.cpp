#include "bossentity.h"
#include "itementity.h"

using namespace WarmupGame;

BossEntity::BossEntity(int period, int radius, int height)
    : WarmupEntity(Type::BOSS)
    , m_period(period)
    , m_radius(radius)
    , m_height(height)
{
    // ROBOCYLINDER SMASH THROUGH WHATEVER HE RUN INTO. RHHRHRHR
    m_static = true;
}

void BossEntity::tick() {
    // Move around circularly
    m_angle += 2.f * glm::pi<float>() / m_period;
    m_position = glm::vec3(m_radius*glm::cos(m_angle), m_height, m_radius*glm::sin(m_angle));

    // Shoot some items
    if (m_time % 3 == 0) {
        float ang = 2.f * glm::pi<float>() * (float)rand() / RAND_MAX;
        float x = 0.2f * glm::cos(ang);
        float z = 0.2f * glm::sin(ang);
        ItemEntity *item = new ItemEntity(false, m_position, glm::vec3(x, -0.2f, z));
        parent()->addEntity(item);
    }

    ++m_time;
}

void BossEntity::draw() {
    graphics().shaderColor(glm::vec3(0.5f, 0.5f, 0.5f));
    graphics().shaderUseTexture(false);
    glm::mat4 m = glm::mat4(1.f);
    m = glm::translate(glm::mat4(1.f), m_position);
    m = glm::scale(m, SCALE_VECTOR);
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawCylinder();
}

glm::vec2 BossEntity::getCylinder() const {
    return HITBOX;
}

void BossEntity::collide(glm::vec3, const WarmupEntity *other) {
    if (other->type() == Type::PLAYER)
        m_dead = true;
}
