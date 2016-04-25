#pragma once

#include "engine/entity.h"

namespace COG {

class COGGame;

class COGEntity : public CS1972Engine::Entity {
public:
    COGEntity(float beat)
        : m_lastBeat(beat)
    { }
    virtual ~COGEntity() { }

private:
    float m_lastBeat;
    float m_beat;

protected:
    VALUE_ACCESSOR(float,beat)

public:
    void offsetPosition(glm::vec3 controllerPos) { m_position -= controllerPos; }

    virtual void tickBeats(float beats) = 0;
    virtual void draw(int pass) override = 0;
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(); }

    virtual void tick(float seconds) final override {
        m_beat = seconds;
        if (m_beat > m_lastBeat) {
            tickBeats(m_beat - m_lastBeat);
            m_lastBeat = m_beat;
        }
    }
    virtual glm::vec2 getCylinder() const final override { return glm::vec2(); }
    virtual csm::aabb getAabb() const final override { return csm::aabb(); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) final override { }
    virtual void collide(glm::vec3 mtv, const Entity *other) final override { }
};

}
