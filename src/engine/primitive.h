#pragma once

#include "../util/CommonIncludes.h"

namespace CS1972Engine {

class Primitive {
public:
    Primitive(int vertices, int size, GLfloat *data);
    Primitive(int vertices, int size, void *data, int type);
    virtual ~Primitive();

private:
    int m_vertices;
    GLuint m_vbo;
    GLuint m_vao;

public:
    void drawArray() const;
    void drawArray(GLenum mode, GLint first, GLsizei count) const;
};

}
