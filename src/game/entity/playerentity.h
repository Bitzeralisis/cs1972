#pragma once

#include "platformerentity.h"

namespace Platformer {

class PlayerShotEntity;

class PlayerEntity : public PlatformerEntity {
public:
    PlayerEntity(glm::vec3 pos);
    virtual ~PlayerEntity() { }

public:
    const float BASE_DOWNWARD_VELOCITY = -3.f;
    const float WALK_VELOCITY = 6.f;
    const float DASH_VELOCITY = 12.f;
    const float JUMP_VELOCITY = 6.f;
    const float DASH_JUMP_VELOCITY = 12.f;

    const float SHOT_SPEED = 20.f;
    const float SHOT_CD = 0.25f;

private:
    float m_yaw = 0.f;
    bool m_standing = false;
    int m_dashEffect = 0;

    float m_shotCd = 0.f;

    bool m_doPathfind = false;
    glm::vec3 m_pfPosition = glm::vec3(0.1f, 0.f, 0.f);

    bool m_rampBounceHack = true; // Prevent bouncing off (gradual) ramps

public:
    void walk(float seconds, glm::vec3 walk, bool dashing, bool jumping);
    bool shoot(glm::vec3 dir);

public:
    VALACC_MUT(glm::vec3,position)
    VALACC_MUT(glm::vec3,velocity)

    VALACC_MUT(float,yaw)
    VALUE_ACCESSOR(int,dashEffect)

    VALACC_MUT(bool,doPathfind)
    VALACC_MUT(glm::vec3,pfPosition)

    VALACC_MUT(bool,rampBounceHack)

    virtual void tick(float seconds) override;
    virtual void draw(int pass) override;
    virtual glm::vec2 getCylinder() const override { return glm::vec2(0.5f, 2.f); }
    virtual csm::aabb getAabb() const override { return csm::aabb(); }
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.5f, 1.f, 0.5f)); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) override;
    virtual void collide(glm::vec3 mtv, const PlatformerEntity *other) override;
};

}
