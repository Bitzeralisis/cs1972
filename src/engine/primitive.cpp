#include "primitive.h"

using namespace CS1972Engine;

Primitive::Primitive(int vertices, int size, GLfloat *data)
    : Primitive(vertices, size, GL_TRIANGLES, data, 0)
{ }

Primitive::Primitive(int vertices, int size, GLenum defaultMode, void *data, int type) {
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    m_numVertices = vertices;
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    m_defaultMode = defaultMode;

    switch (type) {
    case 0: // 3 vertices, 3 normals, 2 texture coordinates
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*3));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*6));
        break;

    case 1: // 2 texture coordinates (for particles)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2, 0);
        break;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Primitive::~Primitive() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Primitive::drawArray() const {
    glBindVertexArray(m_vao);
    glDrawArrays(m_defaultMode, 0, m_numVertices);
    glBindVertexArray(0);
}

void Primitive::drawArray(GLenum mode, GLint first, GLsizei count) const {
    glBindVertexArray(m_vao);
    glDrawArrays(mode, first, count);
    glBindVertexArray(0);
}
