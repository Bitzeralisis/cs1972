#include "primitive.h"

using namespace CS1972Engine;

Primitive::Primitive(int vertices, int size, GLfloat *data) {
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    /*m_quadNumVertices = 6;
    GLfloat quadData[48] = {
        -.5f,0.f,-.5f, 0.f,1.f,0.f, 0.f,1.f,
        -.5f,0.f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,0.f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
         .5f,0.f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
        -.5f,0.f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,0.f, .5f, 0.f,1.f,0.f, 1.f,0.f
    };
    int quadDataSize = 48 * sizeof(GLfloat);*/

    m_vertices = vertices;
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 0);
    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*3));
    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*6));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Primitive::~Primitive() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Primitive::drawArray() const {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices);
    glBindVertexArray(0);
}
