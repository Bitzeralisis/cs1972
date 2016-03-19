#pragma once

#include "../terrainmanager.h"

class OBJ;

namespace CS1972Engine {

class GeometricManager : public CS1972Engine::TerrainManager {
public:
    GeometricManager(World *parent, const char *terrain);
    virtual ~GeometricManager();

private:
    const float MAX_SLIDING_PASSES = 4;
    const float LIFT_EPSILON = 0.001f;

    OBJ *m_terrain;
    CS1972Engine::Primitive *m_primitive;

    bool m_slideHack = true; // prevent sliding down ramps
    bool m_deflectionHack = true; // prevent ramp displacement

public:
    VALACC_MUT(bool,slideHack)
    VALACC_MUT(bool,deflectionHack)

    virtual void tick(float seconds);
    virtual void draw();
    virtual bool collideEntity(const CS1972Engine::Entity *ent, const glm::vec3 &pos0, const glm::vec3 &pos1, glm::vec3 &pos2, glm::vec3 &normal) const;
    virtual glm::vec3 collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1) const { return pos1; }
    virtual bool rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const;
};

}
