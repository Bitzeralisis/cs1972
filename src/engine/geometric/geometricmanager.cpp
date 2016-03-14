#include "geometricmanager.h"
#include "../primitive.h"
#include "../entity.h"
#include "../../csm/csm_collide.h"
#include "../../util/obj.h"

using namespace CS1972Engine;

GeometricManager::GeometricManager(OBJ *terrain)
    : m_terrain(terrain)
{
    m_primitive = new CS1972Engine::Primitive(m_terrain->vertexCount, m_terrain->vertexCount*8*sizeof(GLfloat), m_terrain->vboData.data());
}

GeometricManager::~GeometricManager() {
    delete m_terrain;
    delete m_primitive;
}

void GeometricManager::tick(float seconds) {

}

void GeometricManager::draw() {
    graphics().shaderColor(glm::vec4(1.f));
    graphics().shaderUseTexture(false);
    graphics().shaderMTransform(glm::mat4(1.f));
    m_primitive->drawArray();
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

bool GeometricManager::rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const {
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
    return (best < range);
}
