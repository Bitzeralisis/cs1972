#pragma once

#include <glm/glm.hpp>

namespace csm {

class ray {
public:
    ray(glm::vec3 point, glm::vec3 dir)
        : p(point)
        , d(dir)
    { }

public:
    const glm::vec3 p;
    const glm::vec3 d;

public:
    inline ray operator+(const glm::vec3 &rhs) const { return ray(p+rhs, d); }
    inline ray operator-(const glm::vec3 &rhs) const { return ray(p-rhs, d); }
    inline ray operator*(const glm::vec3 &rhs) const { return ray(p*rhs, d*rhs); }
    inline ray operator/(const glm::vec3 &rhs) const { return ray(p/rhs, d/rhs); }

    inline glm::vec3 at(const float t) const { return p + d*t; }
};

class line {
public:
    line(glm::vec3 point0, glm::vec3 point1)
        : p0(point0)
        , p1(point1)
    { }

public:
    const glm::vec3 p0;
    const glm::vec3 p1;

public:
    inline bool operator==(const csm::line &rhs) const { return p0 == rhs.p0 && p1 == rhs.p1; }
};

class triangle {
public:
    triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
        : vertices({v1, v2, v3})
    { }

public:
    const glm::vec3 vertices[3];

public:
    inline glm::vec3 normal() const { return glm::normalize(glm::cross(vertices[1]-vertices[0], vertices[2]-vertices[0])); }
    inline triangle operator*(const glm::vec3 &rhs) const { return triangle(vertices[0]*rhs, vertices[1]*rhs, vertices[2]*rhs); }
    inline triangle operator/(const glm::vec3 &rhs) const { return triangle(vertices[0]/rhs, vertices[1]/rhs, vertices[2]/rhs); }
};

class aabb {
public:
    aabb()
        : pos(glm::vec3())
        , size(glm::vec3())
    { }
    aabb(glm::vec3 v, glm::vec3 s)
        : pos(v)
        , size(s)
    { }

public:
    const glm::vec3 pos;
    const glm::vec3 size;

public:
    bool intersects(const aabb &rhs) const {
        glm::vec3 max = pos+size;
        glm::vec3 rhsmax = rhs.pos+rhs.size;
        return
            max[0] > rhs.pos[0] &&
            max[1] > rhs.pos[1] &&
            max[2] > rhs.pos[2] &&
            pos[0] < rhsmax[0] &&
            pos[1] < rhsmax[1] &&
            pos[2] < rhsmax[2] ;
    }
};

class cylinder {
public:
    cylinder(glm::vec3 p0, glm::vec3 p1, float r)
        : v0(p0)
        , v1(p1)
        , radius(r)
    { }

public:
    const glm::vec3 v0;
    const glm::vec3 v1;
    const float radius;
};

class ellipsoid {
public:
    ellipsoid()
        : center(glm::vec3())
        , radii(glm::vec3())
    { }
    ellipsoid(glm::vec3 c, glm::vec3 r)
        : center(c)
        , radii(r)
    { }

public:
    const glm::vec3 center;
    const glm::vec3 radii;

public:
    inline ellipsoid operator+(const glm::vec3 &rhs) const { return ellipsoid(center+rhs, radii); }
    inline ellipsoid operator-(const glm::vec3 &rhs) const { return ellipsoid(center-rhs, radii); }
    inline ellipsoid operator*(const glm::vec3 &rhs) const { return ellipsoid(center*rhs, radii*rhs); }
    inline ellipsoid operator/(const glm::vec3 &rhs) const { return ellipsoid(center/rhs, radii/rhs); }
};

}
