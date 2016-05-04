#include "enemyshotentity.h"
#include "playerentity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"
#include "engine/graphics/uishadermodule.h"
#include "csm/csm.h"
#include <glm/gtx/rotate_vector.hpp>

using namespace COG;

EnemyShotEntity::EnemyShotEntity(float beat, float travelTime, glm::vec3 pos, glm::vec3 vel, PlayerEntity *target, int type, int lane)
    : COGEntity(beat)
    , m_target(target)
    , m_visualPosition(pos)
    , m_shotVel(vel)
    , m_travelTime(travelTime)
    , m_hitBeat(beat+travelTime)
    , m_shotType(type)
    , m_approachLane(lane)
{
    m_position = pos;
}

void EnemyShotEntity::detatchShot() {
    m_approachLane = -1;
    m_position = m_visualPosition;
    m_velocity = 0.5f*m_shotVel;
}

void EnemyShotEntity::tickBeats(float beats) {
    if (m_approachLane >= 0) {
        float travel = (m_hitBeat-beat()) / m_travelTime;
        float rotation[3] = { 0.f, 0.25f*glm::pi<float>(), -0.25f*glm::pi<float>() };
        glm::vec3 direction = glm::rotate(m_target->velocity(), rotation[m_approachLane], graphics().camera()->upVector());
        direction = 1.f*glm::normalize(direction);
        m_visualPosition = csm::bezier_curve(m_target->position() + direction, m_target->position() + 5.f*direction, m_position+m_shotVel, m_position, travel);
    } else {
        if (totalBeats() >= m_travelTime)
            parent()->deleteEntity(this);
        tickPhysicsContinuous(beats);
        m_visualPosition = m_position;
    }
}

void EnemyShotEntity::draw(int pass, float beat) {
    switch (pass) {
    case GameScreen::DRAW_GEOMETRY: {
        graphics().shader()->useTexture(false);
        graphics().shader()->color(glm::vec4(0.f));
        graphics().deferred()->useGlowTexture(false);
        graphics().deferred()->glowColor(glm::vec4(1.5f, 0.2f, 0.2f, 1.f));
        glm::mat4 m(1.f);
        m = glm::translate(m, m_visualPosition);
        m = glm::scale(m, glm::vec3(0.2f));
        m = glm::rotate(m, totalBeats(), glm::vec3(1.f, 1.f, 1.f));
        graphics().shader()->mTransform(m);
        graphics().getPrimitive("cubeLines")->drawArray();
        break;
    }

    case GameScreen::DRAW_ADDITIVE: {
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("blur");
        graphics().shader()->color(glm::vec4(1.0f, 0.0f, 0.0f, 1.f));
        graphics().shader()->mTransform(graphics().shader()->billboardMTransform(m_visualPosition, glm::vec2(0.3f)));
        graphics().pQuad()->drawArray();
        break;
    }

    /*
    case GameScreen::DRAW_ORTHOGRAPHIC: {
        glm::vec3 pos = graphics().uishader()->cameraSpaceToUisSpace(m_visualPosition);
        float travel = (m_hitBeat-beat)/8.f;
        float hSize = parent()->parent()->height()*HEXAGON_SIZE_FACTOR * 0.5f;
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("enemyshot");
        graphics().uishader()->color(glm::vec4(0.f, 1.f, 1.f, (1.f-travel)*(1.f-0.5f*glm::mod(beat, 1.f))));
        graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize, 0.f, 0.25f, 0.f, 0.5f);
        if (travel < 0.5f) {
            graphics().uishader()->color(glm::vec4(travel, 1.f-travel, 1.f-travel, 0.5f-travel));
            graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize,  360.f*travel, 0.f, 0.25f, 0.5f, 1.f);
            graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize, -360.f*travel, 0.f, 0.25f, 0.5f, 1.f);
        }
        break;
    }
    */
    }
}
