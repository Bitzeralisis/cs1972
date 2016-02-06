#include "graphics.h"
#include "../util/ResourceLoader.h"
#include "../util/CommonIncludes.h"
#include <QGLWidget>
#include <QImage>

using namespace CS1972Engine;

void Graphics::initializeGL() {
    // Load default shader
    m_defaultShader = ResourceLoader::loadShaders(":/shaders/shader.vert", ":/shaders/shader.frag");

    // Build VAO for a quad
    glGenBuffers(1, &m_quadVbo);
    glGenVertexArrays(1, &m_quadVao);
    glBindVertexArray(m_quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);

    m_quadNumVertices = 6;
    GLfloat quadData[48] = {
        -.5f,0.f,-.5f, 0.f,1.f,0.f, 0.f,1.f,
        -.5f,0.f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,0.f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
         .5f,0.f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
        -.5f,0.f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,0.f, .5f, 0.f,1.f,0.f, 1.f,0.f
    };
    int quadDataSize = 48 * sizeof(GLfloat);

    glBufferData(GL_ARRAY_BUFFER, quadDataSize, quadData, GL_STATIC_DRAW);

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

GLuint Graphics::loadTextureFromQRC(const char *path) {
    QImage img(path);
    img = QGLWidget::convertToGLFormat(img);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

void Graphics::putTexture(const char *name, GLuint tex) {
    m_textures[name] = tex;
}

GLuint Graphics::getTexture(const char *name) {
    return m_textures[name];
}

void Graphics::useDefaultShader() {
    useShader(m_defaultShader);
}

void Graphics::useShader(GLuint shader) {
    m_activeShader = shader;
    glUseProgram(m_activeShader);
}

void Graphics::shaderPvTransformFromCamera() {
    glm::mat4 p = camera->perspectiveMatrix();
    glm::mat4 v = camera->viewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(m_activeShader, "p"), 1, GL_FALSE, glm::value_ptr(p));
    glUniformMatrix4fv(glGetUniformLocation(m_activeShader, "v"), 1, GL_FALSE, glm::value_ptr(v));
}

void Graphics::shaderMTransform(glm::mat4 m) {
    glUniformMatrix4fv(glGetUniformLocation(m_activeShader, "m"), 1, GL_FALSE, glm::value_ptr(m));
}

void Graphics::shaderColor(glm::vec3 color) {
    glUniform3fv(glGetUniformLocation(m_activeShader, "color"), 1, glm::value_ptr(color));
}

void Graphics::shaderUseTexture(bool use) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useTexture"), use);
}

void Graphics::shaderBindTexture(GLuint tex) {
    glUniform1i(glGetUniformLocation(m_activeShader, "tex"), GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void Graphics::shaderBindTexture(const char *name) {
    shaderBindTexture(getTexture(name));
}

void Graphics::shaderUnbindTexture() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Graphics::drawQuad() {
    glBindVertexArray(m_quadVao);
    glDrawArrays(GL_TRIANGLES, 0, m_quadNumVertices);
    glBindVertexArray(0);
}
