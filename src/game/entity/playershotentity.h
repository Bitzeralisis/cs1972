#pragma once

#include "minecraftentity.h"

namespace Minecraft {

class PlayerShotEntity : public MinecraftEntity {
public:
    explicit PlayerShotEntity(glm::vec3 pos, glm::vec3 vel);
    virtual ~PlayerShotEntity() { }

private:
    float m_lifetime = 3.f;

public:
    virtual void tick(float seconds);
    virtual void draw();
    virtual glm::vec2 getCylinder() const;
    virtual csm::aabb getAabb() const;
    virtual void collideTerrain(glm::vec3 tv);
    virtual void collide(glm::vec3 mtv, const MinecraftEntity *other);
};

}
