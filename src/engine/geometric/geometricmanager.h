#pragma once

#include "../terrainmanager.h"

class OBJ;

namespace CS1972Engine {

class GeometricManager : public CS1972Engine::TerrainManager {
private:
    struct NavNode;
    struct NavNode {
        NavNode() = delete;
        explicit NavNode(csm::triangle n) : node(n) { }
        csm::triangle node;
        NavNode *edges[3] = { 0, 0, 0 };

        NavNode *prev = 0;
        int prevIdx = -1;
    };

    struct NavFunnel;
    struct NavFunnel {
        NavFunnel() = delete;
        NavFunnel(csm::line p, NavFunnel *n) : portal(p), next(n) { }
        ~NavFunnel() { delete next; }
        csm::line portal;
        NavFunnel *next;
    };

public:
    struct NavPath;
    struct NavPath {
        NavPath() = delete;
        explicit NavPath(glm::vec3 point) : p(point) { }
        ~NavPath() { delete next; }
        glm::vec3 p;
        NavPath *next = 0;
    };

public:
    GeometricManager(World *parent, const char *terrain, const char *navmesh);
    virtual ~GeometricManager();

private:
    const float MAX_SLIDING_PASSES = 4;
    const float LIFT_EPSILON = 0.001f;

    OBJ *m_terrain;
    OBJ *m_navmesh;
    CS1972Engine::Primitive *m_primitive;
    CS1972Engine::Primitive *m_navmeshPrimitive;

    // Variables used for navigation
    bool m_nav_render = false;
    std::list<NavNode *> m_nav_nodes;

    bool m_slideHack = true; // prevent sliding down ramps
    bool m_deflectionHack = true; // prevent ramp displacement

public:
    VALACC_MUT(bool,slideHack)
    VALACC_MUT(bool,deflectionHack)

    // Navigation related methods
    VALACC_MUT(bool,nav_render)
    NavNode *nav_getNodeUnder(glm::vec3 p);
    NavFunnel *nav_getFunnelFrom(glm::vec3 p0, glm::vec3 p1);
    NavPath *nav_getPathFrom(glm::vec3 p0, glm::vec3 p1);

    virtual void tick(float seconds);
    virtual void draw();
    virtual bool collideEntity(const CS1972Engine::Entity *ent, const glm::vec3 &pos0, const glm::vec3 &pos1, glm::vec3 &pos2, glm::vec3 &normal) const;
    virtual glm::vec3 collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1) const { return pos1; }
    virtual float rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const;
};

}
