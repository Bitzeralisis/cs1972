#pragma once

#include "../../engine/entity.h"

namespace Platformer {

class PlayerEntity : public CS1972Engine::Entity {
public:
    PlayerEntity(glm::vec3 pos);
    virtual ~PlayerEntity() { }

public:
    const glm::vec3 GRAVITY_ACCEL = glm::vec3(0.f, -15.f, 0.f);

    const float BASE_DOWNWARD_VELOCITY = -3.f;
    const float WALK_VELOCITY = 6.f;
    const float DASH_VELOCITY = 12.f;
    const float JUMP_VELOCITY = 6.f;
    const float DASH_JUMP_VELOCITY = 12.f;

private:
    float m_yaw = 0.f;
    bool m_standing = false;
    int m_dashEffect = 0;

    bool m_rampBounceHack = true; // Prevent bouncing off (gradual) ramps

public:
    void walk(float seconds, glm::vec3 walk, bool dashing, bool jumping);

public:
    VALACC_MUT(glm::vec3,position)
    VALACC_MUT(glm::vec3,velocity)

    VALACC_MUT(float,yaw)
    VALUE_ACCESSOR(int,dashEffect)
    VALACC_MUT(bool,rampBounceHack)

    virtual void tick(float seconds) override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override { return glm::vec2(); }
    virtual csm::aabb getAabb() const override { return csm::aabb(); }
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.5f, 1.f, 0.5f)); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) override;
    virtual void collide(glm::vec3 mtv, const Entity *other) override;
};

}
