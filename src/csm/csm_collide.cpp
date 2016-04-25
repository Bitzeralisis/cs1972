#include "csm_collide.h"
#include "csm.h"
#define GLM_FORCE_RADIANS
#include <glm/gtx/norm.hpp>
#include <iostream>

using namespace csm;

float csm::raycast_ellipsoid(const csm::ray &ray, const csm::ellipsoid &ellipsoid, glm::vec3 &intersect, glm::vec3 &normal) {
    // Transform ray to unit-sphere space
    csm::ray usRay = (ray - ellipsoid.center) / ellipsoid.radii;

    // Solve ray-unit-sphere quadratic
    float a = glm::length2(usRay.d);
    float b = 2.f*glm::dot(usRay.d, usRay.p);
    float c = glm::length2(usRay.p) - 1.f;
    float pos, neg;
    int hits = csm::solve_quadratic(a, b, c, pos, neg);

    if (hits) {
        intersect = ray.at(neg);
        normal = glm::normalize(usRay.at(neg) / ellipsoid.radii);
        return neg;
    }
    return -1.f;
}

float csm::raycast_triangle(const csm::ray &ray, const csm::triangle &triangle, glm::vec3 &intersect, glm::vec3 &normal) {
    // Check that ray is going into the non-culled face of triangle
    glm::vec3 n = triangle.normal();
    if (glm::dot(ray.d, n) >= 0.f) return -1.f;

    // Find time at which ray intersects with triangle's plane
    glm::vec3 s = triangle.vertices[0];
    float t = -1.f * glm::dot(n, ray.p-s) / glm::dot(n, ray.d);
    if (t < 0.f) return -1.f;

    // Check if intersection point is inside triangle
    glm::vec3 p = ray.at(t);
    glm::vec3 n1 = csm::triangle(p, triangle.vertices[0], triangle.vertices[1]).normal();
    glm::vec3 n2 = csm::triangle(p, triangle.vertices[1], triangle.vertices[2]).normal();
    glm::vec3 n3 = csm::triangle(p, triangle.vertices[2], triangle.vertices[0]).normal();
    bool inside = glm::dot(n1, n2) > 0.f && glm::dot(n1, n3) > 0.f;

    intersect = p;
    normal = n;
    return (inside) ? t : -1.f;
}

float csm::raycast_cylinderSurface(const ray &ray, const cylinder &cylinder, glm::vec3 &intersect, glm::vec3 &normal) {
    // Find time at which ray instersects with the infinite non-flat surface of the cylinder
    glm::vec3 dv = cylinder.v1-cylinder.v0;
    glm::vec3 c1 = glm::cross(ray.d, dv);
    glm::vec3 c2 = glm::cross(ray.p-cylinder.v0, dv);
    float a = glm::length2(c1);
    float b = 2.f * glm::dot(c1, c2);
    float c = glm::length2(c2) - glm::length2(dv);
    float pos, neg;
    int hits = csm::solve_quadratic(a, b, c, pos, neg);
    if (hits < 1) return -1.f;

    // Check if intersection point is inside cylinder
    intersect = ray.at(neg);
    normal = glm::normalize(glm::cross(glm::cross(dv, intersect-cylinder.v0), dv));
    float p2 = glm::dot(intersect-cylinder.v0, dv);
    return (p2 > 0.f && p2 < glm::length2(dv)) ? neg : -1.f;
}

// Hacky, probably wrong way of checking if a point is in a cone
bool csm::intersect_point_cone(const glm::vec3 &point, const cone &cone) {
    glm::vec3 dp = point - cone.point;
    glm::vec3 proj = glm::dot(dp, cone.dir) / glm::length2(cone.dir) * cone.dir;
    float radius2 = cone.radius*cone.radius * glm::length2(proj);
    float dist2 = glm::distance2(proj, dp);
    return dist2 <= radius2;
}

// Extremely hacky and incorrect cone ellipsoid intersection where ellipsoid's radius is just added to radius of cone
bool csm::intersect_cone_ellipsoid(const cone &cone, const ellipsoid &ellipsoid) {
    glm::vec3 dp = ellipsoid.center - cone.point;
    glm::vec3 proj = glm::dot(dp, cone.dir) / glm::length2(cone.dir) * cone.dir;
    float radius = cone.radius*glm::length(proj) + ellipsoid.radii[0];
    float dist = glm::distance(proj, dp);
    return dist <= radius;
}

inline float collide_unitSphere_triangleInterior(const glm::vec3 &v0, const glm::vec3 &v1, const csm::triangle &t, glm::vec3 &intersect, glm::vec3 &normal) {
    glm::vec3 n = t.normal();
    glm::vec3 p0 = v0 - n;
    glm::vec3 p1 = v1 - n;
    csm::ray r(p0, p1-p0);
    float time = csm::raycast_triangle(r, t, intersect, normal);
    return (0.f <= time && time < 1.f) ? time : -1.f;
}

inline float collide_unitSphere_triangleEdges(const glm::vec3 &v0, const glm::vec3 &v1, const csm::triangle &t, glm::vec3 &intersect, glm::vec3 &normal) {
    csm::ray r(v0, v1-v0);

    float bestTime = std::numeric_limits<float>::infinity();
    float time;
    glm::vec3 i, n;

    time = csm::raycast_cylinderSurface(r, csm::cylinder(t.vertices[0], t.vertices[1], 1.f), i, n);
    if (0.f <= time && time < 1.f && time < bestTime) {
        bestTime = time;
        intersect = i;
        normal = n;
    }

    time = csm::raycast_cylinderSurface(r, csm::cylinder(t.vertices[1], t.vertices[2], 1.f), i, n);
    if (0.f <= time && time < 1.f && time < bestTime) {
        bestTime = time;
        intersect = i;
        normal = n;
    }

    time = csm::raycast_cylinderSurface(r, csm::cylinder(t.vertices[2], t.vertices[0], 1.f), i, n);
    if (0.f <= time && time < 1.f && time < bestTime) {
        bestTime = time;
        intersect = i;
        normal = n;
    }

    return (bestTime == std::numeric_limits<float>::infinity()) ? -1.f : bestTime;
}

inline float collide_unitSphere_triangleVertices(const glm::vec3 &v0, const glm::vec3 &v1, const csm::triangle &t, glm::vec3 &intersect, glm::vec3 &normal) {
    csm::ellipsoid e(v0, glm::vec3(1.f));

    float bestTime = std::numeric_limits<float>::infinity();
    float time;
    glm::vec3 i, n;

    time = csm::raycast_ellipsoid(ray(t.vertices[0], v0-v1), e, i, n);
    if (0.f <= time && time < 1.f && time < bestTime) {
        bestTime = time;
        intersect = t.vertices[0];
        normal = -1.f*n;
    }

    time = csm::raycast_ellipsoid(ray(t.vertices[1], v0-v1), e, i, n);
    if (0.f <= time && time < 1.f && time < bestTime) {
        bestTime = time;
        intersect = t.vertices[1];
        normal = -1.f*n;
    }

    time = csm::raycast_ellipsoid(ray(t.vertices[2], v0-v1), e, i, n);
    if (0.f <= time && time < 1.f && time < bestTime) {
        bestTime = time;
        intersect = t.vertices[2];
        normal = -1.f*n;
    }

    return (bestTime == std::numeric_limits<float>::infinity()) ? -1.f : bestTime;
}

float csm::collide_ellipsoid_triangle(const csm::ellipsoid &ellipsoid0, const csm::ellipsoid &ellipsoid1, const csm::triangle &triangle, glm::vec3 &intersect, glm::vec3 &normal) {
    // Transform to unit sphere
    csm::ellipsoid e0 = ellipsoid0 / ellipsoid0.radii;
    csm::ellipsoid e1 = ellipsoid1 / ellipsoid0.radii;
    csm::triangle t = triangle / ellipsoid0.radii;

    float bestTime = std::numeric_limits<float>::infinity();
    float time;
    glm::vec3 i, n;

    time = collide_unitSphere_triangleInterior(e0.center, e1.center, t, i, n);
    if (time >= 0.f && time < bestTime) {
        bestTime = time;
        intersect = i;
        normal = n;
    }

    time = collide_unitSphere_triangleEdges(e0.center, e1.center, t, i, n);
    if (time >= 0.f && time < bestTime) {
        bestTime = time;
        intersect = i;
        normal = n;
    }

    time = collide_unitSphere_triangleVertices(e0.center, e1.center, t, i, n);
    if (time >= 0.f && time < bestTime) {
        bestTime = time;
        intersect = i;
        normal = n;
    }

    // Transform back to original space
    intersect *= ellipsoid0.radii;
    normal = glm::normalize(normal / ellipsoid0.radii);
    return (bestTime == std::numeric_limits<float>::infinity()) ? -1.f : bestTime;
}
