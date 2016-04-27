#include "playershotentity.h"
#include "enemyentity.h"
#include "playerentity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"
#include "engine/graphics/uishadermodule.h"
#include "csm/csm.h"

using namespace COG;

PlayerShotEntity::PlayerShotEntity(PlayerEntity *parent, float beat, EnemyEntity *target, glm::vec3 back, glm::vec3 up)
    : COGEntity(beat)
    , m_parent(parent)
    , m_shotBeat(beat)
    , m_hit(false)
    , m_target(target)
    , m_back(back)
    , m_up(up)
{
    target->attachShot(this);
}

PlayerShotEntity::~PlayerShotEntity() { }

void PlayerShotEntity::detatchShot(bool hit, glm::vec3 position) {
    m_position = position;
    m_target = 0;
    m_hit = hit;
    if (!m_hit)
        parent()->deleteEntity(this);
}

void PlayerShotEntity::tickBeats(float) {
    if (!m_hit)
        return;

    if (beat() >= m_shotBeat + 2.f)
        parent()->deleteEntity(this);
}

void PlayerShotEntity::draw(int pass) {
    if (!m_hit && (!m_target || !m_target->parent()))
        return;

    glm::vec3 lockedPosition;
    if (m_hit)
        lockedPosition = m_position;
    else
        lockedPosition = m_target->position();

    float beat = GAME->beat();

    switch (pass) {
    case GameScreen::DRAW_GEOMETRY:
        if (beat < m_shotBeat + 1.f) {
            glm::vec3 dir = lockedPosition - m_parent->position();
            glm::vec3 rej = m_up - glm::dot(m_up, dir) / glm::length2(dir) * dir;
            rej = glm::normalize(rej);

            int numPoints = 51;
            int length = 11;
            GLfloat *line = new GLfloat[8*numPoints];
            for (int i = 0; i < numPoints; ++i) {
                float t = (float) i / (numPoints-1);
                glm::vec3 pos = csm::bezier_curve(m_parent->position()+m_back, rej, lockedPosition, t);
                line[8*i+0] = pos.x;
                line[8*i+1] = pos.y;
                line[8*i+2] = pos.z;
            }
            CS1972Engine::Primitive *primitive = new CS1972Engine::Primitive(numPoints, 8*numPoints*sizeof(GLfloat), line);

            int travel = glm::min(glm::max(0.f, beat-m_shotBeat), 1.f) * (numPoints-length);
            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(false);
            graphics().shader()->mTransform(glm::mat4(1.f));
            glLineWidth(5.f);
            graphics().deferred()->glowColor(glm::vec4(1.f));
            primitive->drawArray(GL_LINE_STRIP, travel, length);
            glLineWidth(1.f);
            graphics().deferred()->glowColor(glm::vec4(0.5f));
            primitive->drawArray(GL_LINE_STRIP, travel+length, numPoints-(travel+length));

            delete line;
            delete primitive;
        }

    case GameScreen::DRAW_ORTHOGRAPHIC: {
        glm::vec3 pos = graphics().uishader()->cameraSpaceToUisSpace(lockedPosition);
        if (pos.z > 0.f) {
            if (beat < m_shotBeat + 1.f) {
                pos = glm::round(pos);
                float rotation = 22.5f - glm::min(45.f*(beat-m_shotBeat), 22.5f);
                float hSize = parent()->parent()->height()/TARGET_SIZE_FACTOR * 0.5f * (1.f+rotation/22.5f);
                graphics().shader()->useTexture(true);
                graphics().shader()->bindTexture("target");
                graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, 1.f-rotation/22.5f));
                graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize, rotation, 0.f, 0.5f, 0.f, 0.5f);
            } else if (beat < m_shotBeat + 2.f) {
                float fade = m_shotBeat+2.f - beat;
                graphics().shader()->useTexture(false);
                graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, fade));
                glm::mat4 m(1.f);
                m = glm::translate(m, pos);
                m = glm::scale(m, glm::vec3(parent()->parent()->height()/TARGET_SIZE_FACTOR*(1.f-fade*fade)));
                graphics().shader()->mTransform(m);
                graphics().getPrimitive("uis_circleLoop")->drawArray();
            }
        }
    }
    }
}
