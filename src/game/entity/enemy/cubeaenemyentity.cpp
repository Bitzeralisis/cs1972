#include "cubeaenemyentity.h"
#include "game/gamescreen.h"
#include "game/entity/lightentity.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"

using namespace COG;

CubeaEnemyEntity::CubeaEnemyEntity(float beat, glm::vec3 pos)
    : EnemyEntity(beat, 1, 100)
{
    m_position = pos;
    m_velocity = glm::vec3(1.8f, 0.f, 0.f);
}

void CubeaEnemyEntity::tickBeats(float beats) {
    EnemyEntity::tickBeats(beats);
}

void CubeaEnemyEntity::draw(int pass) {
    if (pass != GameScreen::DRAW_GEOMETRY)
        return;

    graphics().shader()->useTexture(false);
    graphics().shader()->color(glm::vec4(0.f));
    graphics().deferred()->useGlowTexture(true);
    graphics().deferred()->bindGlowTexture("cube1");
    graphics().deferred()->glowColor(glm::vec4(0.75f, 0.5f, 0.15f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(0.5f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();
}

csm::ellipsoid CubeaEnemyEntity::getEllipsoid() const {
    return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.5f));
}

void CubeaEnemyEntity::hitEffect(float) { }

void CubeaEnemyEntity::deathEffect(float beat) {
    audio().queueSoundOnBeat("noisy1.aif", beat+0.25f);
    parent()->addEntity(GameScreen::LAYER_AMBIENCE, new LightEntity(beat, m_position, glm::vec3(0.f, 0.f, 0.f)));
}
