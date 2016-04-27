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
            //glm::vec3 rej = m_up - glm::dot(m_up, dir) / glm::length2(dir) * dir;
            //rej = glm::normalize(rej);
            glm::vec3 rej = 2.f*glm::normalize(m_up);

            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(false);
            graphics().shader()->mTransform(glm::mat4(1.f));

            int totalPoints = 51;
            int beamLength = 10;
            float beamStart = glm::min(glm::max(0.f, beat-m_shotBeat), 1.f) * (totalPoints-beamLength);
            //int beamEnd = beamStart + beamLength;
            //int lineLength = totalPoints - beamEnd;

            GLfloat *beam = new GLfloat[16*(beamLength+1)];
            for (int i = 0; i <= beamLength; ++i) {
                float t = glm::min((i+beamStart) / (totalPoints-1), 1.f);
                glm::vec3 pos = csm::bezier_curve(m_parent->position()+m_back, rej, lockedPosition, t);
                glm::vec3 nextPos = csm::bezier_curve(m_parent->position()+m_back, rej, lockedPosition, t + 1.f/(totalPoints-1));
                glm::vec3 toNext = nextPos-pos;
                glm::vec3 toEye = m_parent->position()-pos;
                glm::vec3 right = 0.01f*glm::normalize(glm::cross(toNext, toEye));
                glm::vec3 p1 = pos-right;
                glm::vec3 p2 = pos+right;
                beam[16*i+ 0] = p1.x;
                beam[16*i+ 1] = p1.y;
                beam[16*i+ 2] = p1.z;
                beam[16*i+ 8] = p2.x;
                beam[16*i+ 9] = p2.y;
                beam[16*i+10] = p2.z;
            }
            graphics().deferred()->glowColor(glm::vec4(1.5f, 1.5f, 1.5f, 1.f));
            CS1972Engine::Primitive(2*(beamLength+1), 8*2*(beamLength+1)*sizeof(GLfloat), GL_TRIANGLE_STRIP, beam, 0).drawArray();
            delete beam;

            /*
            GLfloat *line = new GLfloat[8*(lineLength+1)];
            for (int i = 0; i <= lineLength; ++i) {
                float t = (float) (i+beamEnd) / (totalPoints-1);
                glm::vec3 pos = csm::bezier_curve(m_parent->position()+m_back, rej, lockedPosition, t);
                line[8*i+0] = pos.x;
                line[8*i+1] = pos.y;
                line[8*i+2] = pos.z;
            }
            glLineWidth(1.f);
            graphics().deferred()->glowColor(glm::vec4(0.5f));
            CS1972Engine::Primitive(lineLength+1, 8*(lineLength+1)*sizeof(GLfloat), GL_LINE_STRIP, line, 0).drawArray();
            delete line;
            */
        }

    case GameScreen::DRAW_ORTHOGRAPHIC: {
        glm::vec3 pos = graphics().uishader()->cameraSpaceToUisSpace(lockedPosition);
        if (pos.z > 0.f) {
            if (beat < m_shotBeat + 1.f) {
                pos = glm::round(pos);
                float rotation = 22.5f - glm::min(45.f*(beat-m_shotBeat), 22.5f);
                float hSize = parent()->parent()->height()*TARGET_SIZE_FACTOR * 0.5f * (1.f+rotation/22.5f);
                graphics().shader()->useTexture(true);
                graphics().shader()->bindTexture("target");
                graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, 1.f-rotation/22.5f));
                graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize, rotation, 0.f, 0.5f, 0.f, 0.5f);
            } else if (beat < m_shotBeat + 2.f) {
                float fade = m_shotBeat+2.f - beat;
                float dist = glm::distance(m_parent->position(), lockedPosition);
                graphics().shader()->useTexture(false);
                graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, fade));
                glm::mat4 m(1.f);
                m = glm::translate(m, pos);
                m = glm::scale(m, glm::vec3(parent()->parent()->height()*BURST_SIZE_FACTOR*(1.f-fade*fade) / dist));
                graphics().shader()->mTransform(m);
                graphics().getPrimitive("uis_circleLoop")->drawArray();
                m = glm::scale(m, glm::vec3(1.f/(1.f-fade*fade)));
                fade -= 0.05f;
                m = glm::scale(m, glm::vec3(1.f-fade*fade));
                graphics().shader()->mTransform(m);
                graphics().getPrimitive("uis_circleLoop")->drawArray();
            }
        }
    }
    }
}
