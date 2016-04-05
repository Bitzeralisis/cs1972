#include "enemyshotentity.h"

using namespace Platformer;

EnemyShotEntity::EnemyShotEntity(glm::vec3 pos, glm::vec3 vel)
    : PlatformerEntity(Type::ENEMY_ATTACK)
{
    m_position = pos;
    m_velocity = vel;
    m_static = true;
}

void EnemyShotEntity::explode() {
    m_exploding = true;
    m_velocity = glm::vec3(0.f);
    m_collidesTerrain = false;
}

void EnemyShotEntity::tick(float seconds) {
    if (m_exploding) {
        m_life -= seconds;
        if (m_life <= 0.f)
            parent()->deleteEntity(this);
    }

    tickPhysicsDiscrete(seconds);
}

void EnemyShotEntity::draw(int pass) {
    switch (pass) {
    case 1:
        graphics().dr_lightPoint(m_position, m_life*glm::vec3(3.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 2.4f));
        graphics().dr_drawLight();
        break;

    case 2:
        if (m_exploding) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            graphics().shaderColor(glm::vec4(0.5f, 0.f, 0.f, m_life));
            glm::mat4 m(1.f);
            m = glm::translate(m, m_position);
            m = glm::scale(m, (4.f-3.f*m_life)*glm::vec3(1.f));
            graphics().shaderMTransform(m);
            graphics().drawSphere();
            glDisable(GL_BLEND);
        } else {
            graphics().shaderUseLight(false);
            graphics().shaderColor(glm::vec4(2.f, 0.f, 0.f, 1.f));
            glm::mat4 m(1.f);
            m = glm::translate(m, m_position);
            m = glm::scale(m, glm::vec3(0.4f));
            graphics().shaderMTransform(m);
            graphics().drawSphere();
        }
        break;
    }
}

glm::vec2 EnemyShotEntity::getCylinder() const {
    return (4.f-3.f*m_life) * glm::vec2(0.25f, 0.5f);
}

void EnemyShotEntity::collideTerrain(const glm::vec3&, const glm::vec3&) {
    explode();
}

void EnemyShotEntity::collide(glm::vec3, const PlatformerEntity *other) {
    if (other->type() == Type::PLAYER)
        explode();
}
