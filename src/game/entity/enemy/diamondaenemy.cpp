#include "diamondaenemy.h"
#include "game/gamescreen.h"
#include "game/entity/lightentity.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"

using namespace COG;

DiamondaEnemy::DiamondaEnemy(float beat)
    : EnemyEntity(beat, 8, 2000)
{ }

void DiamondaEnemy::draw(int pass, float beat) {
    if (pass != GameScreen::DRAW_GEOMETRY)
        return;

    graphics().shader()->useTexture(false);
    graphics().shader()->color(glm::vec4(0.f));
    graphics().deferred()->useGlowTexture(true);
    graphics().deferred()->bindGlowTexture("diamond1");
    graphics().deferred()->glowColor(glm::vec4(1.5f, 1.f, 0.3f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::rotate(m, totalBeats()*glm::pi<float>()*0.5f, glm::vec3(0.f, 1.f, 0.f));
    graphics().shader()->mTransform(m);
    graphics().getPrimitive("diamond")->drawArray();
}

csm::ellipsoid DiamondaEnemy::getEllipsoid() const {
    return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.8f));
}

void DiamondaEnemy::hitEffect(float beat) {
    audio().queueSoundOnBeat("supergate-snare.aif", beat+0.25f);
}

void DiamondaEnemy::deathEffect(float beat) {
    audio().queueSoundOnBeat("popslap.aif", beat+0.25f);
    parent()->addEntity(GameScreen::LAYER_AMBIENCE, new LightEntity(beat, m_position, m_velocity));
}
