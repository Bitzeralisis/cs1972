#pragma once

#include "minecraftentity.h"

namespace Minecraft {

class PlayerEntity : public MinecraftEntity {
public:
    explicit PlayerEntity(glm::vec3 pos);
    virtual ~PlayerEntity() { }

protected:
    explicit PlayerEntity(Type type);

public:
    const float WALK_VELOCITY = 6.f;
    const float DASH_VELOCITY = 120.f;
    const float CHAOS_DASH_VELOCITY = 70.f;
    const float JUMP_VELOCITY = 6.9f;
    const float DASH_JUMP_VELOCITY = 11.5f;
    const float CHAOS_JUMP_VELOCITY = 25.f;

    const float MAX_HEALTH = 100.f;
    const float HEALTH_REGEN = 1.f/1.2f;
    const float CHAOS_DAMAGE = 2.f/1.2f;
    const float IFRAMES = 0.5f;
    const float MAX_ENERGY = 110.f;
    const float ENERGY_LOCK_LOW = 10.f;
    const float ENERGY_LOCK_HIGH = 110.f;
    const float ENERGY_REGEN = 20.f;
    const float MAX_DIRT = 100.f;
    const float MAX_CHAOS_PER_LEVEL = 100.f;
    const float CHAOS_REGEN = 50.f;
    const float CHAOS_CD = 2.f;

    const float DASH_COST = 30.f;
    const float DASH_JUMP_COST = 10.f;
    const float ATTACK_COOLDOWN = 0.5f;
    const float ATTACK_COST = 20.f;
    const float SHOOT_COST = 30.f;
    const float STORM_COST = 400.f;
    const float DIG_COST = 60.f;
    const float DIG_DIRT_GAIN = 100.f;
    const float PLACE_COST = 40.f;
    const float PLACE_DIRT_COST = 90.f;

private:
    bool m_dead = false;
    float m_health = MAX_HEALTH;
    float m_iframe = 0.f;
    float m_energy = MAX_ENERGY;
    bool m_energyLocked = false;
    float m_dirt = 0.f;
    float m_chaos = 400.f;
    float m_chaosCd = 0.f;
    int m_chaosLevel = 4;

    bool m_standing = false;
    int m_dashEffect = 0;
    float m_attackEffect = 0.f;

public:
    VALUE_ACCESSOR(bool,dead)
    VALUE_ACCESSOR(float,health)
    VALUE_ACCESSOR(float,iframe)
    VALUE_ACCESSOR(float,energy)
    VALUE_ACCESSOR(bool,energyLocked)
    VALACC_MUT(float,dirt)
    VALUE_ACCESSOR(float,chaos)
    VALUE_ACCESSOR(int,chaosLevel)
    inline void chaosLevelUp() { ++m_chaosLevel; m_chaos = m_chaosLevel*MAX_CHAOS_PER_LEVEL; }

    VALUE_ACCESSOR(int,dashEffect)
    VALUE_ACCESSOR(float,attackEffect)

    void walk(float seconds, glm::vec2 walk, bool dashing, bool jumping);
    bool attack();
    bool shoot(); // Level 2 chaos power
    bool storm(); // Level 4 chaos power
    bool dig();
    bool place();

    bool takeDamage(float damage);
    bool takeChaosDamage(float seconds);
    bool tryToPayEnergy(float energy);
    bool tryToPayChaos(float chaos);

    virtual void tick(float seconds) override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual csm::aabb getAabb() const override;
    virtual void collideTerrain(glm::vec3 tv) override;
    virtual void collide(glm::vec3 mtv, const MinecraftEntity *other) override;
};

}
