#pragma once

#include "engine/entity.h"

namespace COG {

class COGGame;

class COGEntity : public CS1972Engine::Entity {
public:
    COGEntity(float beat)
        : m_firstBeat(beat)
        , m_prevBeat(beat)
    { }
    virtual ~COGEntity() { }

private:
    float m_firstBeat;
    float m_prevBeat;
    float m_beat;

protected:
    inline float totalBeats() const { return m_beat-m_firstBeat; }
    VALUE_ACCESSOR(float,firstBeat)
    VALUE_ACCESSOR(float,beat)

public:
    void offsetPosition(glm::vec3 controllerPos) { m_position -= controllerPos; }

    virtual void tickBeats(float beats) = 0;
    virtual void draw(int pass) override = 0;
    virtual csm::ellipsoid getEllipsoid() const override { return csm::ellipsoid(); }

    virtual void tick(float seconds) final override {
        m_beat = seconds;
        if (m_beat > m_prevBeat) {
            tickBeats(m_beat - m_prevBeat);
            m_prevBeat = m_beat;
        }
    }
    virtual glm::vec2 getCylinder() const final override { return glm::vec2(); }
    virtual csm::aabb getAabb() const final override { return csm::aabb(); }
    virtual void collideTerrain(const glm::vec3 &tv, const glm::vec3 &normal) final override { }
    virtual void collide(glm::vec3 mtv, const Entity *other) final override { }
};

}
