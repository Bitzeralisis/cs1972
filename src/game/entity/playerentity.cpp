#include "playerentity.h"
#include "stormentity.h"
#include "playerattackentity.h"
#include "playershotentity.h"

using namespace Minecraft;

PlayerEntity::PlayerEntity(glm::vec3 pos)
    : MinecraftEntity(Type::PLAYER)
{
    m_position = pos;
    m_accel = glm::vec3(0.f, GRAVITY_ACCEL, 0.f);
}

PlayerEntity::PlayerEntity(Type type)
    : MinecraftEntity(type)
{
    m_accel = glm::vec3(0.f, GRAVITY_ACCEL, 0.f);
}

void PlayerEntity::walk(float seconds, glm::vec2 walk, bool dashing, bool jumping) {
    if (walk != glm::vec2(0.f))
        walk = glm::normalize(walk);
    else
        dashing = false;

    float cost = seconds*DASH_COST;
    bool dashed = false;
    if (dashing && tryToPayEnergy(cost))
        dashed = true;

    if (m_standing) {
        walk *= (dashed ? DASH_VELOCITY : WALK_VELOCITY);

        m_velocity.x = walk.x;
        m_velocity.z = walk.y;

        if (jumping) {
            if (dashed && tryToPayEnergy(DASH_JUMP_COST))
                m_velocity.y = DASH_JUMP_VELOCITY;
            else
                m_velocity.y = JUMP_VELOCITY;
        }

        m_accel.x = 0.f;
        m_accel.z = 0.f;
    } else {
        walk *= (dashed ? DASH_VELOCITY : WALK_VELOCITY);
        glm::vec2 curr(m_velocity.x, m_velocity.z);
        glm::vec2 goal = 2.f*(walk-curr);

        m_accel.x = goal.x;
        m_accel.z = goal.y;
    }

    if (dashed && m_dashEffect < 5)
        ++m_dashEffect;
    else if (!dashed && m_dashEffect > 0)
        --m_dashEffect;
}

bool PlayerEntity::attack() {
    if (m_attackEffect <= 0.f && tryToPayEnergy(ATTACK_COST)) {
        parent()->addEntity(new PlayerAttackEntity(m_position, graphics().camera->lookVector()));
        m_attackEffect = ATTACK_COOLDOWN;
        return true;
    }
    return false;
}

bool PlayerEntity::shoot() {
    if (m_chaosLevel >= 2 && tryToPayChaos(SHOOT_COST)) {
        parent()->addEntity(new PlayerShotEntity(m_position+glm::vec3(0.f, 1.5f, 0.f), m_velocity + 20.f*glm::normalize(graphics().camera->lookVector())));
        return true;
    }
    return false;
}

bool PlayerEntity::storm() {
    if (m_chaosLevel >= 3 && tryToPayChaos(STORM_COST)) {
        parent()->addEntity(new StormEntity(m_position+glm::vec3(0.f, 1.5f, 0.f), m_velocity + 5.f*glm::normalize(graphics().camera->lookVector())));
        return true;
    }
    return false;
}

bool PlayerEntity::dig() {
    return tryToPayEnergy(DIG_COST);
}

bool PlayerEntity::place() {
    if (m_dirt >= PLACE_DIRT_COST && tryToPayEnergy(PLACE_COST)) {
        m_dirt -= PLACE_DIRT_COST;
        return true;
    }
    return false;
}

bool PlayerEntity::takeDamage(float damage) {
    if (m_iframe <= 0.f) {
        m_health -= damage;
        if (m_health <= 0.f) {
            m_health = 0.f;
            m_dead = true;
        }
        m_iframe = IFRAMES;
        return true;
    }
    return false;
}

bool PlayerEntity::takeChaosDamage(float seconds) {
    m_health -= seconds*CHAOS_DAMAGE;
    if (m_health <= 0.f) {
        m_health = 0.f;
        m_dead = true;
    }
    return true;
}

bool PlayerEntity::tryToPayEnergy(float energy) {
    if (m_energyLocked)
        return false;
    if (m_energy >= energy) {
        m_energy -= energy;
        if (m_energy < ENERGY_LOCK_LOW)
            m_energyLocked = true;
        return true;
    }
    return false;
}

bool PlayerEntity::tryToPayChaos(float chaos) {
    if (m_chaos >= chaos) {
        m_chaos -= chaos;
        m_chaosCd = CHAOS_CD;
        return true;
    }
    return false;
}

void PlayerEntity::tick(float seconds) {
    m_attackEffect -= seconds;
    if (m_attackEffect < 0.f)
        m_attackEffect = 0.f;

    if (m_dead) {
        m_health = 0.f;
        m_energy = 0.f;
    } else {
        m_health += seconds*HEALTH_REGEN;
        if (m_health > MAX_HEALTH)
            m_health = MAX_HEALTH;
        m_iframe -= seconds;
        if (m_iframe < 0.f)
            m_iframe = 0.f;

        m_energy += seconds*ENERGY_REGEN;
        if (m_energy > MAX_ENERGY)
            m_energy = MAX_ENERGY;
        if (m_energy >= ENERGY_LOCK_HIGH)
            m_energyLocked = false;

        m_chaosCd -= seconds;
        if (m_chaosCd <= 0.f) {
            m_chaosCd = 0.f;
            m_chaos += seconds*CHAOS_REGEN;
            if (m_chaos > m_chaosLevel*MAX_CHAOS_PER_LEVEL)
                m_chaos = m_chaosLevel*MAX_CHAOS_PER_LEVEL;
        }
    }

    tickPhysicsDiscrete(seconds);
}

void PlayerEntity::draw() {
    graphics().shaderUseTexture(false);
    graphics().shaderColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
    glm::mat4 m(1.f);
    m = glm::translate(m, m_position);
    m = glm::scale(m, glm::vec3(1.f, 1.6f, 1.f));
    m = glm::translate(m, glm::vec3(0.f, 0.5f, 0.f));
    graphics().shaderMTransform(m);
    graphics().drawCylinder();
}

glm::vec2 PlayerEntity::getCylinder() const {
    return glm::vec2(0.5f, 2.f);
}

csm::aabb PlayerEntity::getAabb() const {
    return csm::aabb(glm::vec3(-0.5f, 0.f, -0.5f), glm::vec3(1.f, 2.f, 1.f));
}

void PlayerEntity::collideTerrain(glm::vec3 tv) {
    if ((tv.x > 0.01f && m_velocity.x < 0.f) || (tv.x < -0.01f && m_velocity.x > 0.f)) m_velocity.x = 0.f;
    if (/*(tv.y > 0.01f && m_velocity.y < 0.f) ||*/ (tv.y < -0.01f && m_velocity.y > 0.f)) m_velocity.y = 0.f;
    if ((tv.z > 0.01f && m_velocity.z < 0.f) || (tv.z < -0.01f && m_velocity.z > 0.f)) m_velocity.z = 0.f;

    if (tv.y > 0.f && m_velocity.y < 0.f) {
        // Translated up due to falling onto terrain means we are "standing" on ground
        m_velocity.y = 0.f;
        m_standing = true;
    } else {
        m_standing = false;
    }
}

void PlayerEntity::collide(glm::vec3 mtv, const MinecraftEntity *other) {
    // Don't get resolved by colliding with own projectiles
    if (other->type() == Type::PLAYER_ATTACK) {
        m_position -= mtv;
        return;
    }

    // Standing on an entity that may be standing on the floor, so stop our downwards velocity
    if (mtv.y > 0.f)
        m_velocity.y = 0.f;

    if (other->type() == Type::ENEMY) {
        if (takeDamage(10.f)) {
            // Launched when hit by enemy
            m_velocity = glm::vec3(mtv.x, 0.f, mtv.z);
            if (m_velocity == glm::vec3(0.f)) {
                float dir = 2.f*glm::pi<float>() * (float)rand2() / RAND2_MAX;
                m_velocity = glm::vec3(glm::cos(dir), 0.f, glm::sin(dir));
            }
            m_velocity = 20.f*glm::normalize(m_velocity);
            m_velocity.y = 5.f;
        }
    }
}
