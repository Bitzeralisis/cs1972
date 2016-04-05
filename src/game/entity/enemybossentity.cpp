#include "enemybossentity.h"
#include "enemyentity.h"
#include "enemyshotentity.h"
#include "playerentity.h"
#include "../../engine/primitive.h"

using namespace Platformer;

EnemyBossEntity::EnemyBossEntity(PlayerEntity *target)
    : PlatformerEntity(Type::ENEMY)
    , m_target(target)
{
    m_static = true;
    m_collidesTerrain = false;
}

void EnemyBossEntity::tick(float seconds) {
    if (m_health <= 0.f) {
        parent()->removeEntity(this);
        return;
    }

    float period = 15.f;
    float x = glm::cos(2.f*glm::pi<float>()*m_time/period);
    float z = glm::sin(2.f*glm::pi<float>()*m_time/period);
    float period2 = 20.f;
    float y = glm::sin(2.f*glm::pi<float>()*m_time/period2);
    m_position = glm::vec3(10.f*x, 14.f + 3.f*y, 10.f*z);

    // Choose an attack if the attack switch timer is over, and perform attack's immediate effects
    m_nextAttack -= seconds * (1.5f - 0.5f*m_health/MAX_HEALTH);
    if (m_nextAttack <= 0.f) {
        m_currentAttack = rand() % 2 + 1;

        switch (m_currentAttack) {
        case 1:
            // Shooting attack
            m_shotCd = 0.f;
            m_shotCount = SHOT_COUNT;
            m_nextAttack = SHOT_FREQ;
            break;

        case 2:
            // Summon attack
            // Immediately pick another attack if already have max minions
            if (m_minionCount == MAX_MINIONS) {
                m_nextAttack = 0.f;
                break;
            }

            // Summon until you have max living minions
            for (; m_minionCount < MAX_MINIONS; ++m_minionCount) {
                float dir = 2.f*glm::pi<float>() * (float)rand()/RAND_MAX;
                EnemyEntity *e = new EnemyEntity(m_position, this, m_target);
                e->velocity(15.f*glm::vec3(glm::cos(dir), 0.f, glm::sin(dir)));
                parent()->addEntity(e);
            }
            m_nextAttack = SUMMON_FREQ;
            break;
        }
    }

    // Perform current attack's ongoing effects
    switch (m_currentAttack) {
    case 1: {
        // Shooting attack
        m_shotCd -= seconds;
        if (m_shotCount > 0 && m_shotCd <= 0.f) {
            parent()->addEntity(new EnemyShotEntity(m_position + glm::vec3(0.f, 1.f, 0.f), SHOT_SPEED * glm::normalize(m_target->position() - m_position)));
            --m_shotCount;
            m_shotCd += SHOT_CD;
        }
        break;
    }
    }

    m_time += seconds;
}

void EnemyBossEntity::draw(int pass) {
    switch (pass) {
    case 0: {
        graphics().shaderUseTexture(false);
        graphics().shaderColor(glm::vec4(0.75f, 0.75f, 0.75f, 1.f));
        glm::mat4 m(1.f);
        m = glm::translate(m, m_position);
        m = glm::scale(m, glm::vec3(1.f, 2.f, 1.f));
        graphics().shaderMTransform(m);
        graphics().drawCylinder();
        break;
    }

    /*case 1:
        graphics().dr_lightPoint(m_position + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.5f), glm::vec3(1.f, 0.01f, 0.0003f));
        graphics().dr_drawLight();
        break;*/

    case 4: {
        glm::vec3 pos = graphics().uis_cameraSpaceToUisSpace(m_position);
        graphics().shaderUseTexture(false);
        graphics().uis_color(glm::vec4(1.2f, 0.f, 0.f, 1.f));
        glm::mat4 m(1.f);
        m = glm::translate(m, pos);
        m = glm::scale(m, glm::vec3(60.f));
        graphics().shaderMTransform(m);
        graphics().getPrimitive("uis_circleLoop")->drawArray(GL_LINE_STRIP, 0, 1 + (int)(360.f*m_health/MAX_HEALTH));
    }
    }
}

void EnemyBossEntity::collideTerrain(const glm::vec3&, const glm::vec3&) { }

void EnemyBossEntity::collide(glm::vec3, const PlatformerEntity *other) {
    if (other->type() == Type::PLAYER_ATTACK)
        m_health -= m_target->SHOT_DAMAGE;
}
