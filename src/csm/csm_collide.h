#pragma once

#include "csm_primitive.h"

namespace csm {

// raycast_X : Returns parametric time at which ray intersects with X, or a negative time if there is no intersection.
//             Returns point of intersection and normal of intersected surface if there is an intersection.
float raycast_ellipsoid(const csm::ray &ray, const csm::ellipsoid &ellipsoid, glm::vec3 &intersect, glm::vec3 &normal);
float raycast_triangle(const csm::ray &ray, const csm::triangle &triangle, glm::vec3 &intersect, glm::vec3 &normal);
float raycast_cylinderSurface(const csm::ray &ray, const csm::cylinder &cylinder, glm::vec3 &intersect, glm::vec3 &normal);

// collide_X_Y : Given an X moving uniformly from X0 to X1 over a time unit of 1.0, returns the parametric time at which X collides with Y,
//               or a negative time if there is no collision between X0 and X1.
//               Returns point of collision and normal of Y against X if there is a collision.
float collide_ellipsoid_triangle(const csm::ellipsoid &ellipsoid0, const csm::ellipsoid &ellipsoid1, const csm::triangle &triangle, glm::vec3 &intersect, glm::vec3 &normal);

}
