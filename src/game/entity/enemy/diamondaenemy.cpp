#include "diamondaenemy.h"
#include "game/gamescreen.h"
#include "game/entity/lightentity.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"
#include "csm/csm.h"

using namespace COG;

DiamondaEnemy::DiamondaEnemy(float beat, glm::vec3 pos)
    : EnemyEntity(beat, 8, 2000)
{
    m_position = pos;
    m_position.y = 20.f;
    m_velocity = glm::vec3(1.f, -10.f, 0.f);
    m_accel = glm::vec3(0.f, 2.5f, 0.f);
}

void DiamondaEnemy::tickBeats(float beats) {
    if (totalBeats() >= 12.f) {
        m_accel.y = 2.5f;
    } else if (totalBeats() >= 4.f) {
        m_velocity.y = 0.f;
        m_accel.y = 0.f;
    }

    if (!m_shot && totalBeats() >= 5.f) {
        m_shot = true;
        shoot(firstBeat() + 5.f, 8.f, m_position + glm::vec3(0.f, 0.75f, 0.f), glm::vec3(0.f, 4.f, 0.f), 0, csm::rand(0, 3));
        shoot(firstBeat() + 5.f, 8.5f, m_position + glm::vec3(0.f, 0.75f, 0.f), glm::vec3(0.f, 4.f, 0.f), 0, csm::rand(0, 3));
        shoot(firstBeat() + 5.f, 9.f, m_position + glm::vec3(0.f, 0.75f, 0.f), glm::vec3(0.f, 4.f, 0.f), 0, 0);
        shoot(firstBeat() + 5.f, 9.f, m_position + glm::vec3(0.f, 0.75f, 0.f), glm::vec3(0.f, 4.f, 0.f), 0, 1);
        shoot(firstBeat() + 5.f, 9.f, m_position + glm::vec3(0.f, 0.75f, 0.f), glm::vec3(0.f, 4.f, 0.f), 0, 2);
    }

    if (m_position.y > 40.f)
        parent()->deleteEntity(this);

    EnemyEntity::tickBeats(beats);
}

void DiamondaEnemy::draw(int pass) {
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
    return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.5f));
}

void DiamondaEnemy::hitEffect(float beat) {
    audio().queueSoundOnBeat("supergate-snare.aif", beat+0.25f);
}

void DiamondaEnemy::deathEffect(float beat) {
    audio().queueSoundOnBeat("popslap.aif", beat+0.25f);
    parent()->addEntity(GameScreen::LAYER_AMBIENCE, new LightEntity(beat, m_position, glm::vec3(0.f, 0.f, 0.f)));
}
