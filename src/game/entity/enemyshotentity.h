#pragma once

#include "platformerentity.h"

namespace Platformer {

class EnemyShotEntity : public PlatformerEntity {
public:
    EnemyShotEntity(glm::vec3 pos, glm::vec3 vel);
    virtual ~EnemyShotEntity() { }

private:
    bool m_exploding = false;
    float m_life = 1.f;

private:
    void explode();

public:
    virtual void tick(float seconds) override;
    virtual void draw(int pass) override;
    virtual glm::vec2 getCylinder() const override;
    virtual csm::aabb getAabb() const override { return csm::aabb(); }
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.25f)); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) override;
    virtual void collide(glm::vec3 mtv, const PlatformerEntity *other) override;
};

}
