#pragma once

#include "platformerentity.h"

namespace Platformer {

class EnemyEntity : public PlatformerEntity {
public:
    EnemyEntity(glm::vec3 pos);
    virtual ~EnemyEntity() { }

public:
    const float BASE_DOWNWARD_VELOCITY = -3.f;
    const float WALK_VELOCITY = 6.f;
    const float JUMP_VELOCITY = 12.f;

private:
    bool m_standing = false;

public:
    void walk(float seconds, glm::vec3 walk, bool jumping);

public:
    virtual void tick(float seconds) override;
    virtual void draw(int pass) override;
    virtual glm::vec2 getCylinder() const override { return glm::vec2(0.5f, 2.f); }
    virtual csm::aabb getAabb() const override { return csm::aabb(); }
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.5f, 1.f, 0.5f)); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) override;
    virtual void collide(glm::vec3 mtv, const PlatformerEntity *other) override;
};

}
