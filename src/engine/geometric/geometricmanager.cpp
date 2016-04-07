#include "geometricmanager.h"
#include "../primitive.h"
#include "../entity.h"
#include "../../csm/csm_collide.h"
#include "../../util/obj.h"
#include <set>

using namespace CS1972Engine;

GeometricManager::GeometricManager(World *parent, const char *terrain, const char *navmesh)
    : TerrainManager(parent)
{
    if (!graphics().hasObj(terrain))
        graphics().putObj(terrain, new OBJ(terrain));
    m_terrain = graphics().getObj(terrain);

    if (!graphics().hasPrimitive(terrain))
        graphics().putPrimitive(terrain, graphics().loadPrimitiveFromOBJ(m_terrain));
    m_primitive = graphics().getPrimitive(terrain);

    if (!graphics().hasObj(navmesh))
        graphics().putObj(navmesh, new OBJ(navmesh));
    m_navmesh = graphics().getObj(navmesh);

    if (!graphics().hasPrimitive(navmesh))
        graphics().putPrimitive(navmesh, graphics().loadPrimitiveFromOBJ(m_navmesh));
    m_navmeshPrimitive = graphics().getPrimitive(navmesh);

    // Build navmesh search graph
    for (QList<OBJ::Triangle *>::iterator it = m_navmesh->triangles.begin(); it != m_navmesh->triangles.end(); ++it) {
        // Add new triangle
        csm::triangle t((*it)->vertices[0], (*it)->vertices[1], (*it)->vertices[2]);
        NavNode *n = new NavNode(t);

        // Look for triangles which share an edge and link them to each other
        for (std::list<NavNode *>::iterator it2 = m_nav_nodes.begin(); it2 != m_nav_nodes.end(); ++it2) {
            NavNode *n2 = *it2;
            csm::triangle t2 = n2->node;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    csm::line l1(t.vertices[i], t.vertices[(i+1)%3]);
                    csm::line l2(t2.vertices[(j+1)%3], t2.vertices[j]);
                    if (l1 == l2) {
                        n->edges[i] = n2;
                        n2->edges[j] = n;
                    }
                }
            }
        }

        m_nav_nodes.push_back(n);
    }
}

GeometricManager::~GeometricManager() {
    for (std::list<NavNode *>::iterator it = m_nav_nodes.begin(); it != m_nav_nodes.end(); ++it)
        delete *it;
}

GeometricManager::NavNode *GeometricManager::nav_getNodeUnder(glm::vec3 p) {
    NavNode *node = 0;
    float best = std::numeric_limits<float>::infinity();
    for (std::list<NavNode *>::iterator it = m_nav_nodes.begin(); it != m_nav_nodes.end(); ++it) {
        glm::vec3 i, n;
        float t = csm::raycast_triangle(csm::ray(p, glm::vec3(0.f, -1.f, 0.f)), (*it)->node, i, n);
        if (t >= 0.f && t < best) {
            best = t;
            node = *it;
        }
    }

    return node;
}

GeometricManager::NavFunnel *GeometricManager::nav_getFunnelFrom(glm::vec3 p0, glm::vec3 p1) {
    NavNode *start = nav_getNodeUnder(p0);
    NavNode *end = nav_getNodeUnder(p1);
    if (start == 0 || end == 0)
        return 0;

    // Perform DFS
    start->prev = 0;
    end->prev = 0;

    std::deque<NavNode *> neighbors;
    std::set<NavNode *> visited;
    neighbors.push_back(start);
    visited.insert(start);
    while (neighbors.size() > 0) {
        NavNode *current = neighbors.front();
        neighbors.pop_front();
        if (current == end)
            break;
        for (int i = 0; i < 3; ++i) {
            NavNode *next = current->edges[i];
            if (next != 0 && visited.count(next) == 0) {
                next->prev = current;
                next->prevIdx = i;
                neighbors.push_back(next);
                visited.insert(next);
            }
        }
    }

    // Check if path found
    if (start != end && end->prev == 0)
        return 0;

    // Build funnel
    NavFunnel *funnel = new NavFunnel(csm::line(p1, p1), 0);
    while (end->prev != 0) {
        csm::line portal(end->prev->node.vertices[(end->prevIdx+1)%3], end->prev->node.vertices[end->prevIdx]);
        funnel = new NavFunnel(portal, funnel);
        end = end->prev;
    }
    funnel = new NavFunnel(csm::line(p0, p0), funnel);
    return funnel;
}

GeometricManager::NavPath *GeometricManager::nav_getPathFrom(glm::vec3 p0, glm::vec3 p1) {
    NavFunnel *funnel = nav_getFunnelFrom(p0, p1);
    if (funnel == 0)
        return 0;

    NavPath *retval = new NavPath(glm::vec3(0.f));
    NavPath *path = retval;

    int apexSide = 0;
    NavFunnel *apexF = funnel;
    NavFunnel *leftF = apexF->next;
    NavFunnel *rightF = apexF->next;
    NavFunnel *tryF = apexF->next;
    while (1) {
        glm::vec3 apexP = apexSide == 0 ? apexF->portal.p0 : apexF->portal.p1;
        glm::vec3 leftP = 0.99f*leftF->portal.p0 + 0.01f*leftF->portal.p1;
        glm::vec3 rightP = 0.99f*rightF->portal.p1 + 0.01f*rightF->portal.p0;
        glm::vec2 dLeft(leftP.x - apexP.x, leftP.z - apexP.z);
        glm::vec2 dRight(rightP.x - apexP.x, rightP.z - apexP.z);
        float width = glm::acos(glm::dot(dLeft, dRight) / (glm::length(dLeft) * glm::length(dRight)));

        // Check if we are at end
        if (tryF->next == 0) {
            path->next = new NavPath(apexP);
            path->next->next = new NavPath(tryF->portal.p0);
            break;
        }

        // Step to next portal
        tryF = tryF->next;

        // Check left
        glm::vec3 leftP2 = 0.99f*tryF->portal.p0 + 0.01f*tryF->portal.p1;
        glm::vec2 dLeft2(leftP2.x - apexP.x, leftP2.z - apexP.z);
        float width2 = glm::acos(glm::dot(dLeft2, dRight) / (glm::length(dLeft2) * glm::length(dRight)));
        if (glm::cross(glm::vec3(dRight.x, 0.f, dRight.y), glm::vec3(dLeft2.x, 0.f, dLeft2.y)).y < 0.f) {
            path->next = new NavPath(apexP);
            path = path->next;
            apexSide = 1;
            apexF = rightF;
            leftF = apexF->next;
            rightF = apexF->next;
            tryF = apexF->next;
            continue;
        }
        if (width2 < width) {
            leftF = tryF;
            leftP = leftP2;
            dLeft = dLeft2;
            width = width2;
        }

        // Check right
        glm::vec3 rightP2 = 0.99f*tryF->portal.p1 + 0.01f*tryF->portal.p0;
        glm::vec2 dRight2(rightP2.x - apexP.x, rightP2.z - apexP.z);
        width2 = glm::acos(glm::dot(dLeft, dRight2) / (glm::length(dLeft) * glm::length(dRight2)));
        if (glm::cross(glm::vec3(dRight2.x, 0.f, dRight2.y), glm::vec3(dLeft.x, 0.f, dLeft.y)).y < 0.f) {
            path->next = new NavPath(apexP);
            path = path->next;
            apexSide = 0;
            apexF = leftF;
            leftF = apexF->next;
            rightF = apexF->next;
            tryF = apexF->next;
            continue;
        }
        if (width2 < width) {
            rightF = tryF;
        }
    }

    // The first item in the path is a dummy head pointer, so get rid of it
    NavPath *head = retval;
    retval = retval->next;
    head->next = 0;
    delete head;

    return retval;
}

void GeometricManager::tick(float seconds) { }

void GeometricManager::draw(int pass) {
    switch (pass) {
    case 0:
        graphics().shaderColor(glm::vec4(1.f));
        graphics().shaderUseTexture(false);
        graphics().shaderMTransform(glm::mat4(1.f));
        m_primitive->drawArray();
        break;

    case 3:
        if (m_nav_render) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            graphics().shaderUseTexture(false);
            graphics().shaderUseLight(false);
            graphics().shaderColor(glm::vec4(1.f, 0.f, 1.f, 0.2f));
            graphics().shaderMTransform(glm::mat4(1.f));
            m_navmeshPrimitive->drawArray();
            graphics().shaderUseLight(true);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
        }
        break;
    }
}

bool GeometricManager::collideEntity(const CS1972Engine::Entity *ent, const glm::vec3 &pos0, const glm::vec3 &pos1, glm::vec3 &pos2, glm::vec3 &normal) const {
    bool collided = false;

    glm::vec3 p0 = pos0;
    glm::vec3 p1 = pos1;
    float horiz = p1.y-p0.y;
    // Slide-prevention hack: regular passes of collision tests ignore y-movement
    if (m_slideHack)
        p1.y = p0.y;

    for (int i = 0; i < MAX_SLIDING_PASSES; ++i) {
        if (p0 == p1)
            break;

        float best = std::numeric_limits<float>::infinity();
        for (QList<OBJ::Triangle *>::iterator it = m_terrain->triangles.begin(); it != m_terrain->triangles.end(); ++it) {
            glm::vec3 i, n;
            csm::triangle triangle((*it)->vertices[0], (*it)->vertices[1], (*it)->vertices[2]);
            float t = csm::collide_ellipsoid_triangle(ent->getEllipsoid() + p0, ent->getEllipsoid() + p1, triangle, i, n);
            if (t >= 0.f && t < best) {
                best = t;
                pos2 = p0 + t*(p1-p0);
                normal = n;
            }
        }
        if (best == std::numeric_limits<float>::infinity())
            break;

        collided = true;
        p0 = pos2 + LIFT_EPSILON*normal;

        // Deflection hack
        glm::vec3 up(0.f, 1.f, 0.f);
        glm::vec3 disp = p1-pos2;
        float length = glm::length(disp - glm::dot(disp, normal)*normal);
        glm::vec3 dir = disp - glm::dot(disp, normal)/glm::dot(normal, up)*up;

        if (!m_deflectionHack || glm::dot(normal, up) == 0.f || dir == glm::vec3(0.f)) {
            pos2 = p1 - glm::dot(disp, normal)*normal + LIFT_EPSILON*normal;
        } else {
            dir = glm::normalize(dir);
            pos2 += length*dir + LIFT_EPSILON*normal;
        }
        p1 = pos2;
    }

    // Slide-prevention hack: must do one more pass of collision tests, but only for y-movement
    if (m_slideHack) {
        if (collided) {
            p0 = pos2;
            p1 = p0;
        } else {
            p0 = p1;
        }
        p1.y += horiz;
        pos2.y += horiz;

        if (p0 != p1) {
            float best = std::numeric_limits<float>::infinity();
            for (QList<OBJ::Triangle *>::iterator it = m_terrain->triangles.begin(); it != m_terrain->triangles.end(); ++it) {
                glm::vec3 i, n;
                csm::triangle triangle((*it)->vertices[0], (*it)->vertices[1], (*it)->vertices[2]);
                float t = csm::collide_ellipsoid_triangle(ent->getEllipsoid() + p0, ent->getEllipsoid() + p1, triangle, i, n);
                if (t >= 0.f && t < best) {
                    best = t;
                    pos2 = p0 + t*(p1-p0);
                    normal = n;
                }
            }
            if (best != std::numeric_limits<float>::infinity()) {
                collided = true;
                glm::vec3 n2(0.f, normal.y, 0.f);
                if (n2 != glm::vec3(0.f))
                    n2 = glm::normalize(n2);
                pos2 += LIFT_EPSILON*n2;
            }
        }
    }

    return collided;
}

float GeometricManager::rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const {
    float best = range;
    for (QList<OBJ::Triangle *>::iterator it = m_terrain->triangles.begin(); it != m_terrain->triangles.end(); ++it) {
        glm::vec3 i, n;
        csm::triangle triangle((*it)->vertices[0], (*it)->vertices[1], (*it)->vertices[2]);
        float t = csm::raycast_triangle(csm::ray(p, v), triangle, i, n);
        if (t >= 0.f && t < best) {
            best = t;
            intersect = i;
            normal = n;
        }
    }
    return (best < range) ? best : -1.f;
}
