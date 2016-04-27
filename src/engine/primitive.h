#pragma once

#include "util/CommonIncludes.h"
#include "util/HeaderUtils.h"

namespace CS1972Engine {

class Primitive {
public:
    Primitive(int vertices, int size, GLfloat *data);
    Primitive(int vertices, int size, GLenum defaultMode, void *data, int type);
    virtual ~Primitive();

private:
    int m_numVertices;
    GLuint m_vbo;
    GLuint m_vao;
    GLenum m_defaultMode;

public:
    VALUE_ACCESSOR(int,numVertices)

    void drawArray() const;
    void drawArray(GLenum mode, GLint first, GLsizei count) const;
};

}
