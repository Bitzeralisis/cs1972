#include "playershotentity.h"
#include "enemyentity.h"
#include "playerentity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"
#include "csm/csm.h"

using namespace COG;

PlayerShotEntity::PlayerShotEntity(PlayerEntity *parent, float beat, EnemyEntity *target, glm::vec3 back, glm::vec3 up)
    : COGEntity(beat)
    , m_parent(parent)
    , m_shotBeat(beat)
    , m_target(target)
    , m_back(back)
    , m_up(up)
{ }

PlayerShotEntity::~PlayerShotEntity() { }

void PlayerShotEntity::tickBeats(float beats) {
    if (beat() > m_shotBeat + 1.f)
        parent()->deleteEntity(this);
}

void PlayerShotEntity::draw(int pass) {
    if (pass != GameScreen::DRAW_GEOMETRY)
        return;

    float travel = GAME->beat() - m_shotBeat;

    glm::vec3 dir = m_target->position() - m_parent->position();
    glm::vec3 rej = m_up - glm::dot(m_up, dir) / glm::length2(dir) * dir;
    rej = glm::normalize(rej);

    int numPoints = 11;
    GLfloat *line = new GLfloat[8*numPoints];
    for (int i = 0; i < numPoints; ++i) {
        float along = (float) i / (numPoints-1);
        float t = 0.2f + 0.8f*(0.2f*along + 0.8f*travel);
        glm::vec3 pos = csm::bezier_curve(m_parent->position()+m_back, rej, m_target->position(), t);
        line[8*i+0] = pos.x;
        line[8*i+1] = pos.y;
        line[8*i+2] = pos.z;
    }

    glLineWidth(3.f);
    graphics().shader()->useTexture(false);
    graphics().shader()->color(glm::vec4(0.f));
    graphics().deferred()->useGlowTexture(false);
    graphics().deferred()->glowColor(glm::vec4(1.f));
    graphics().shader()->mTransform(glm::mat4(1.f));
    CS1972Engine::Primitive(numPoints, 8*numPoints*sizeof(GLfloat), line).drawArray(GL_LINE_STRIP, 0, numPoints);

    delete line;
}
