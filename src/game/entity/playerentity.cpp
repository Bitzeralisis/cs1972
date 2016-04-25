#include "playerentity.h"
#include "engine/graphics/particlemodule.h"

using namespace COG;

PlayerEntity::PlayerEntity()
    : COGEntity(0.f)
{
    m_velocity = glm::vec3(0.8f, 0.f, 0.f);
}

void PlayerEntity::tickBeats(float beats) {
    tickPhysicsContinuous(beats);
}

void PlayerEntity::gainScoreValue(int score) {
    m_score += m_combo*score;
}

void PlayerEntity::makeParticles(glm::vec3 position, glm::vec3 velocity) {
    int numParts = 256;
    GLfloat *pos = new GLfloat[4*numParts];
    GLfloat *vel = new GLfloat[3*numParts];
    for (int i = 0; i < numParts; ++i) {
        pos[4*i+0] = 0.5f * (float) rand() / RAND_MAX - 0.25f + position.x;
        pos[4*i+1] = 0.5f * (float) rand() / RAND_MAX - 0.25f + position.y;
        pos[4*i+2] = 0.5f * (float) rand() / RAND_MAX - 0.25f + position.z;
        pos[4*i+3] = 1.f * (float) rand() / RAND_MAX + 1.f;
    }
    for (int i = 0; i < numParts; ++i) {
        vel[3*i+0] = 0.8f * (float) rand() / RAND_MAX - 0.4f + velocity.x;
        vel[3*i+1] = 0.8f * (float) rand() / RAND_MAX - 0.4f + velocity.y;
        vel[3*i+2] = 0.8f * (float) rand() / RAND_MAX - 0.4f + velocity.z;
    }
    graphics().particle()->putParticles(numParts, pos, vel);
    delete pos;
    delete vel;
}
