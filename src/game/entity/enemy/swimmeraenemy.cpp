#include "swimmeraenemy.h"
#include "game/gamescreen.h"
#include "game/entity/lightentity.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"

using namespace COG;

SwimmeraEnemy::SwimmeraEnemy(float beat)
    : EnemyEntity(beat, 1, 200, 1)
{ }

void SwimmeraEnemy::draw(int pass, float beat) {
    if (pass != GameScreen::DRAW_GEOMETRY)
        return;

    graphics().shader()->useTexture(false);
    graphics().shader()->color(glm::vec4(0.f));
    graphics().deferred()->useGlowTexture(true);
    graphics().deferred()->bindGlowTexture("cube1");
    graphics().deferred()->glowColor(glm::vec4(1.5f, 1.f, 0.3f, 1.f));

    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(0.1f, 0.8f, 0.1f));
    m = glm::rotate(m, totalBeats()*glm::pi<float>()*0.5f, glm::vec3(0.f, 1.f, 0.f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();

    m = glm::mat4(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(0.6f, 0.1f, 0.6f));
    m = glm::rotate(m, totalBeats()*glm::pi<float>()*0.5f, glm::vec3(0.f, 1.f, 0.f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();
}

csm::ellipsoid SwimmeraEnemy::getEllipsoid() const {
    return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.5f));
}

void SwimmeraEnemy::deathEffect(float beat) {
    audio().queueSoundOnBeat("metal-plink.wav", beat+0.25f);
    parent()->addEntity(GameScreen::LAYER_AMBIENCE, new LightEntity(beat, m_position, m_velocity));
}
