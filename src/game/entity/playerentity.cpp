#include "playerentity.h"
#include "playershotentity.h"

using namespace Platformer;

PlayerEntity::PlayerEntity(glm::vec3 pos)
    : PlatformerEntity(Type::PLAYER)
{
    m_position = pos;
    m_accel = GRAVITY_ACCEL;
}

void PlayerEntity::walk(float, glm::vec3 walk, bool dashing, bool jumping) {
    walk.y = 0.f;
    if (walk != glm::vec3(0.f))
        walk = glm::normalize(walk);
    else
        dashing = false;

    bool dashed = false;
    if (dashing)
        dashed = true;

    if (m_standing) {
        walk *= (dashed ? DASH_VELOCITY : WALK_VELOCITY);

        m_velocity.x = walk.x;
        m_velocity.z = walk.z;

        if (jumping) {
            if (dashed)
                m_velocity.y = DASH_JUMP_VELOCITY;
            else
                m_velocity.y = JUMP_VELOCITY;
        }

        m_accel.x = 0.f;
        m_accel.z = 0.f;
    } else {
        walk *= (dashed ? DASH_VELOCITY : WALK_VELOCITY);
        glm::vec3 goal = 2.f*(walk-m_velocity);

        m_accel.x = goal.x;
        m_accel.z = goal.z;
    }

    if (dashed && m_dashEffect < 5)
        ++m_dashEffect;
    else if (!dashed && m_dashEffect > 0)
        --m_dashEffect;
}

bool PlayerEntity::shoot(glm::vec3 dir) {
    if (m_shotCd <= 0.f) {
        m_shotCd = SHOT_CD;
        PlayerShotEntity *shot = new PlayerShotEntity(m_position + glm::vec3(0.f, 1.5f, 0.f), glm::normalize(dir)*SHOT_SPEED);
        parent()->addEntity(shot);
        return true;
    }
    return false;
}

void PlayerEntity::tick(float seconds) {
    if (m_shotCd > 0.f)
        m_shotCd -= seconds;
    tickPhysicsDiscrete(seconds);
    m_standing = false;
}

void PlayerEntity::draw(int pass) {
    switch (pass) {
    case 0: {
        graphics().shaderUseTexture(false);
        graphics().shaderColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
        glm::mat4 m(1.f);
        m = glm::translate(m, m_position);
        m = glm::scale(m, glm::vec3(1.f, 2.f, 1.f));
        m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
        graphics().shaderMTransform(m);
        graphics().drawSphere();

        if (m_doPathfind) {
            graphics().shaderColor(glm::vec4(1.f, 1.f, 0.f, 1.f));
            m = glm::mat4(1.f);
            m = glm::translate(m, m_pfPosition);
            m = glm::scale(m, glm::vec3(1.f, 2.f, 1.f));
            m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
            graphics().shaderMTransform(m);
            graphics().drawSphere();
        }
        break;
    }

    case 1:
        graphics().dr_lightPoint(m_position + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.5f), glm::vec3(1.f, 0.01f, 0.0003f));
        graphics().dr_drawLight();
        break;
    }
}

void PlayerEntity::collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) {
    if (normal.y > 0.f && m_velocity.y < 0.f) {
        m_velocity.y = m_rampBounceHack ? BASE_DOWNWARD_VELOCITY : 0.f;
        m_standing = true;
    } else {
        m_standing = false;
    }
}

void PlayerEntity::collide(glm::vec3 mtv, const PlatformerEntity *other) {
    m_position -= mtv;
}
