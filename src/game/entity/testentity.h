#pragma once

#include "../../engine/entity.h"

namespace Minecraft {

class TestEntity : public CS1972Engine::Entity {
public:
    explicit TestEntity(glm::vec3 pos);
    virtual ~TestEntity() { }

private:
    const float GRAVITY_ACCEL = -20.f;
    const float WALK_VELOCITY = 4.f;
    const float DASH_VELOCITY = 7.f;
    const float JUMP_VELOCITY = 6.9f;

    bool m_standing = false;
    int m_dashEffect = 0;

public:
    VALUE_ACCESSOR(int,dashEffect)

    void walk(glm::vec3 walk, bool dashing, bool jumping);

    virtual void tick(float seconds) override;
    virtual void draw() override;
    virtual glm::vec2 getCylinder() const override;
    virtual csm::aabb getAabb() const override;
    virtual void collideTerrain(glm::vec3 tv) override;
    virtual void collide(glm::vec3 mtv, const Entity *other) override;
};

}
