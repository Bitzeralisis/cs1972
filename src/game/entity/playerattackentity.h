#pragma once

#include "minecraftentity.h"

namespace Minecraft {

class PlayerAttackEntity : public MinecraftEntity {
public:
    explicit PlayerAttackEntity(glm::vec3 pos, glm::vec3 dir);
    virtual ~PlayerAttackEntity() { }

private:
    float m_lifetime = 0.1f;

public:
    virtual void tick(float seconds);
    virtual void draw();
    virtual glm::vec2 getCylinder() const;
    virtual csm::aabb getAabb() const;
    virtual void collideTerrain(glm::vec3 tv);
    virtual void collide(glm::vec3 mtv, const MinecraftEntity *other);
};

}
