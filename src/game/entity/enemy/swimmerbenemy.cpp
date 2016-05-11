#include "swimmerbenemy.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "game/entity/playerentity.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"

using namespace COG;

SwimmerbEnemy::SwimmerbEnemy(float beat)
    : EnemyEntity(beat, 16, 5000)
{ }

void SwimmerbEnemy::draw(int pass, float beat) {
    if (pass != GameScreen::DRAW_GEOMETRY)
        return;

    graphics().shader()->useTexture(true);
    graphics().deferred()->bindGlowTexture("cube2");
    graphics().shader()->color(glm::vec4(1.f));
    graphics().deferred()->useGlowTexture(true);
    graphics().deferred()->bindGlowTexture("cube1");
    graphics().deferred()->glowColor(glm::vec4(0.75f, 0.5f, 0.15f, 1.f));

    float wiggle = 0.5f*glm::sin(glm::mod(totalBeats(), 2.f)*glm::pi<float>());
    glm::mat4 m0(1.f);
    m0 = glm::translate(m0, m_position);
    m0 = glm::translate(m0, glm::vec3(-1.2f, 0.f, 0.f));
    m0 = glm::rotate(m0, -0.2f*wiggle, glm::vec3(0.f, 1.f, 0.f));
    m0 = glm::translate(m0, glm::vec3(1.2f, 0.f, 0.f));

    // Body
    glm::mat4 m = m0;
    m = glm::translate(m, glm::vec3(-0.9f, -0.3f, 0.f));
    m = glm::scale(m, glm::vec3(3.2f, 2.f, 0.4f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();

    // Fins
    m = m0;
    m = glm::translate(m, glm::vec3(-1.4f, -0.3f, 0.f));
    m = glm::scale(m, glm::vec3(1.2f, 2.8f, 0.3f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();

    // Tail
    m = m0;
    m = glm::translate(m, glm::vec3(-2.8f, -0.3f, 0.f));
    m = glm::translate(m, glm::vec3(0.3f, 0.f, 0.f));
    m = glm::rotate(m, wiggle, glm::vec3(0.f, 1.f, 0.f));
    m = glm::translate(m, glm::vec3(-0.3f, 0.f, 0.f));
    m = glm::scale(m, glm::vec3(0.8f, 2.6f, 0.2f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();

    // Eye
    graphics().shader()->useTexture(false);
    graphics().shader()->color(glm::vec4(0.f));
    graphics().deferred()->glowColor(glm::vec4(1.5f, 1.f, 0.3f, 1.f));
    m = m0;
    m = glm::scale(m, glm::vec3(0.2f, 0.2f, 0.6f));
    graphics().shader()->mTransform(m);
    graphics().pBox()->drawArray();
}

csm::ellipsoid SwimmerbEnemy::getEllipsoid() const {
    return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.2f));
}

void SwimmerbEnemy::hitEffect(float beat) {
    audio().queueSoundOnBeat("metal-plink.wav", beat+0.25f);
}

void SwimmerbEnemy::deathEffect(float beat) {
    audio().queueSoundOnBeat("popslap.aif", beat+0.25f);
    GAME->controller()->makeParticles(2048, m_position+glm::vec3(-1.2f, -0.3f, 0.f), glm::vec3(3.8f, 2.6f, 0.4f), m_velocity, glm::vec3(1.f), glm::vec2(1.f, 2.f));
}
