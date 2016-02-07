#include "obstacleentity.h"
#include "../../engine/graphics.h"

using namespace WarmupGame;

ObstacleEntity::ObstacleEntity() {
    float x = 50.f * (float)rand() / RAND_MAX;
    float y = 50.f * (float)rand() / RAND_MAX;
    m_position = glm::vec3(x, 0.f, y);
    //m_velocity = glm::vec3(0.f, -0.05f, 0.f);
    m_hitbox = glm::vec2(10.f, 10.f);
}

void ObstacleEntity::tick() {
    tickPhysicsDiscrete();

    // Destroy self after hitting ground
    if (m_position.y < 0.f)
        m_position.y = 0.f;
        //parent()->deleteEntity(this);
}

void ObstacleEntity::draw() {
    graphics().shaderColor(glm::vec3(1.f, 0.f, 1.f));
    graphics().shaderUseTexture(false);
    glm::mat4 m = glm::mat4(1.f);
    m = glm::translate(glm::mat4(1.f), m_position);
    m = glm::scale(m, glm::vec3(2.f*m_hitbox.x, m_hitbox.y, 2.f*m_hitbox.x));
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawCylinder();
}

glm::vec2 ObstacleEntity::getCylinder() const {
    return m_hitbox;
}

void ObstacleEntity::collide(glm::vec3 mtv, const Entity *other) {
    m_position += mtv;
}
