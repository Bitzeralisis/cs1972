#pragma once

#include "engine/entity.h"

namespace COG {

class LightEntity : public CS1972Engine::Entity {
public:
    LightEntity(glm::vec3 pos, glm::vec3 vel);
    virtual ~LightEntity() { }

private:
    float m_life = 2.f;

public:
    virtual void tick(float seconds) override;
    virtual void draw(int pass) override;
    virtual glm::vec2 getCylinder() const override { return glm::vec2(0.f); }
    virtual csm::aabb getAabb() const override { return csm::aabb(); }
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(glm::vec3(0.f), glm::vec3(0.f)); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) override { }
    virtual void collide(glm::vec3 mtv, const Entity *other) override { }
};

}
