#include "enemyentity.h"
#include "game/gamescreen.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"

using namespace COG;

EnemyEntity::EnemyEntity(float beat, glm::vec3 pos)
    : COGEntity(beat)
{
    m_position = pos;
}

void EnemyEntity::tickBeats(float beats) {
    tickPhysicsContinuous(beats);
}

void EnemyEntity::draw(int pass) {
    if (pass != GameScreen::DRAW_GEOMETRY)
        return;

    graphics().shader()->useTexture(false);
    graphics().shader()->color(glm::vec4(0.f));
    graphics().deferred()->useGlowTexture(true);
    graphics().deferred()->bindGlowTexture("cube");
    graphics().deferred()->glowColor(glm::vec4(0.75f, 0.5f, 0.15f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(0.5f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();
}

csm::ellipsoid EnemyEntity::getEllipsoid() const {
    return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.5f));
}
