#include "enemyshotentity.h"
#include "playerentity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"
#include "engine/graphics/uishadermodule.h"

using namespace COG;

EnemyShotEntity::EnemyShotEntity(float beat, glm::vec3 pos, PlayerEntity *target, int type, int lane)
    : COGEntity(beat)
    , m_target(target)
    , m_hitBeat(beat+8.f)
    , m_shotType(type)
    , m_approachLane(lane)
{
    m_position = pos;
}

void EnemyShotEntity::tickBeats(float beats) {
    float travel = beats / (m_hitBeat-(beat()-beats));
    m_position = travel*(m_target->position()+glm::vec3(0.3f, 0.f, 0.f)) + (1.f-travel)*m_position;
}

void EnemyShotEntity::draw(int pass) {
    float beat = GAME->beat();

    switch (pass) {
    case GameScreen::DRAW_GEOMETRY: {
        graphics().shader()->useTexture(false);
        graphics().shader()->color(glm::vec4(0.f));
        graphics().deferred()->useGlowTexture(false);
        graphics().deferred()->glowColor(glm::vec4(3.5f, 0.2f, 0.2f, 1.f));
        glm::mat4 m(1.f);
        m = glm::translate(m, m_position);
        m = glm::scale(m, glm::vec3(0.2f));
        graphics().shader()->mTransform(m);
        graphics().pSphere()->drawArray();
    }

    case GameScreen::DRAW_ORTHOGRAPHIC: {
        glm::vec3 pos = graphics().uishader()->cameraSpaceToUisSpace(m_position);
        float travel = (m_hitBeat-beat)/8.f;
        float hSize = parent()->parent()->height()/HEXAGON_SIZE_FACTOR * 0.5f;
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("enemyshot");
        graphics().uishader()->color(glm::vec4(0.f, 1.f, 1.f, (1.f-travel)*(1.f-0.5f*glm::mod(beat, 1.f))));
        graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize, 0.f, 0.25f, 0.f, 0.5f);
        if (travel < 0.5f) {
            graphics().uishader()->color(glm::vec4(travel, 1.f-travel, 1.f-travel, 0.5f-travel));
            graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize,  360.f*travel, 0.f, 0.25f, 0.5f, 1.f);
            graphics().uishader()->drawQuad(pos.x-hSize, pos.x+hSize, pos.y-hSize, pos.y+hSize, -360.f*travel, 0.f, 0.25f, 0.5f, 1.f);
        }
    }
    }
}
