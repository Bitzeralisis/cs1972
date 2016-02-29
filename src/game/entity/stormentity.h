#pragma once

#include "minecraftentity.h"

namespace Minecraft {

class StormEntity : public MinecraftEntity {
public:
    explicit StormEntity(glm::vec3 pos, glm::vec3 dir);
    virtual ~StormEntity() { }

private:
    const float TIME_PER_SHOT = 0.005f;

    float m_lifetime = 3.f;
    float m_shots = 3.f;

public:
    virtual void tick(float seconds);
    virtual void draw();
    virtual glm::vec2 getCylinder() const;
    virtual csm::aabb getAabb() const;
    virtual void collideTerrain(glm::vec3 tv);
    virtual void collide(glm::vec3 mtv, const MinecraftEntity *other);
};

}
