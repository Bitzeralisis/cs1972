#include "graphics.h"
#include "primitive.h"
#include "../util/CommonIncludes.h"
#include "../util/NewCylinderData.h"
#include "../util/obj.h"
#include "../util/ResourceLoader.h"
#include "../util/SphereData.h"
#include <QGLWidget>
#include <QImage>

using namespace CS1972Engine;

Graphics::~Graphics() {
    glDeleteProgram(m_defaultShader);
    glDeleteProgram(m_uiShader);

    for (std::map<std::string, GLuint>::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
        glDeleteTextures(1, &(it->second));

    for (std::map<std::string, OBJ *>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
        delete it->second;

    delete m_pQuad;
    delete m_pBox;
    delete m_pCylinder;
    delete m_pSphere;
    delete m_uiQuad;

    delete camera;
}

void Graphics::initializeGL() {
    // Load default shader
    m_defaultShader = ResourceLoader::loadShaders(":/shaders/shader.vert", ":/shaders/shader.frag");
    m_uiShader = ResourceLoader::loadShaders(":/shaders/2d.vert", ":/shaders/2d.frag");
    m_gShader = ResourceLoader::loadShaders(":/shaders/gbuffer.vert", ":/shaders/gbuffer.frag");
    m_dShader = ResourceLoader::loadShaders(":/shaders/deferred.vert", ":/shaders/deferred.frag");

    // Make some primitives or something
    int quadNumVertices = 6;
    GLfloat quadData[48] = {
        -.5f,0.f,-.5f, 0.f,1.f,0.f, 0.f,1.f,
        -.5f,0.f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,0.f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
         .5f,0.f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
        -.5f,0.f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,0.f, .5f, 0.f,1.f,0.f, 1.f,0.f
    };
    int quadDataSize = 48 * sizeof(GLfloat);
    m_pQuad = new Primitive(quadNumVertices, quadDataSize, quadData);

    int boxNumVertices = 36;
    GLfloat boxData[48*6] = {
        -.5f,.5f,-.5f, 0.f,1.f,0.f, 0.f,1.f,
        -.5f,.5f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,.5f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
         .5f,.5f,-.5f, 0.f,1.f,0.f, 1.f,1.f,
        -.5f,.5f, .5f, 0.f,1.f,0.f, 0.f,0.f,
         .5f,.5f, .5f, 0.f,1.f,0.f, 1.f,0.f,

        -.5f,-.5f,-.5f, 0.f,-1.f,0.f, 0.f,1.f,
         .5f,-.5f,-.5f, 0.f,-1.f,0.f, 1.f,1.f,
        -.5f,-.5f, .5f, 0.f,-1.f,0.f, 0.f,0.f,
        -.5f,-.5f, .5f, 0.f,-1.f,0.f, 0.f,0.f,
         .5f,-.5f,-.5f, 0.f,-1.f,0.f, 1.f,1.f,
         .5f,-.5f, .5f, 0.f,-1.f,0.f, 1.f,0.f,

        -.5f,-.5f,.5f, 0.f,0.f,1.f, 0.f,1.f,
         .5f,-.5f,.5f, 0.f,0.f,1.f, 1.f,1.f,
        -.5f, .5f,.5f, 0.f,0.f,1.f, 0.f,0.f,
        -.5f, .5f,.5f, 0.f,0.f,1.f, 0.f,0.f,
         .5f,-.5f,.5f, 0.f,0.f,1.f, 1.f,1.f,
         .5f, .5f,.5f, 0.f,0.f,1.f, 1.f,0.f,

        -.5f,-.5f,-.5f, 0.f,0.f,-1.f, 0.f,1.f,
        -.5f, .5f,-.5f, 0.f,0.f,-1.f, 0.f,0.f,
         .5f,-.5f,-.5f, 0.f,0.f,-1.f, 1.f,1.f,
         .5f,-.5f,-.5f, 0.f,0.f,-1.f, 1.f,1.f,
        -.5f, .5f,-.5f, 0.f,0.f,-1.f, 0.f,0.f,
         .5f, .5f,-.5f, 0.f,0.f,-1.f, 1.f,0.f,

        .5f,-.5f,-.5f, 1.f,0.f,0.f, 0.f,1.f,
        .5f, .5f,-.5f, 1.f,0.f,0.f, 1.f,1.f,
        .5f,-.5f, .5f, 1.f,0.f,0.f, 0.f,0.f,
        .5f,-.5f, .5f, 1.f,0.f,0.f, 0.f,0.f,
        .5f, .5f,-.5f, 1.f,0.f,0.f, 1.f,1.f,
        .5f, .5f, .5f, 1.f,0.f,0.f, 1.f,0.f,

        -.5f,-.5f,-.5f, -1.f,0.f,0.f, 0.f,1.f,
        -.5f,-.5f, .5f, -1.f,0.f,0.f, 0.f,0.f,
        -.5f, .5f,-.5f, -1.f,0.f,0.f, 1.f,1.f,
        -.5f, .5f,-.5f, -1.f,0.f,0.f, 1.f,1.f,
        -.5f,-.5f, .5f, -1.f,0.f,0.f, 0.f,0.f,
        -.5f, .5f, .5f, -1.f,0.f,0.f, 1.f,0.f
    };
    int boxDataSize = 48*6 * sizeof(GLfloat);
    m_pBox = new Primitive(boxNumVertices, boxDataSize, boxData);

    m_pCylinder = new Primitive(cylinderVertexCount, cylinderDataSize, cylinderVertexBufferData);
    m_pSphere = new Primitive(sphereVertexCount, sphereArraySize, sphereVertexBufferData);

    GLfloat uiQuadData[48] = {
        0.f,0.f,0.f, 0.f,0.f,-1.f, 0.f,1.f,
        0.f,1.f,0.f, 0.f,0.f,-1.f, 0.f,0.f,
        1.f,0.f,0.f, 0.f,0.f,-1.f, 1.f,1.f,
        1.f,0.f,0.f, 0.f,0.f,-1.f, 1.f,1.f,
        0.f,1.f,0.f, 0.f,0.f,-1.f, 0.f,0.f,
        1.f,1.f,0.f, 0.f,0.f,-1.f, 1.f,0.f
    };
    m_uiQuad = new Primitive(quadNumVertices, quadDataSize, uiQuadData);
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

Primitive *Graphics::loadPrimitiveFromOBJ(OBJ *obj) {
    return new Primitive(obj->vertexCount, obj->vertexCount*8*sizeof(GLfloat), obj->vboData.data());
}

void Graphics::useShader(GLuint shader) {
    m_activeShader = shader;
    glUseProgram(m_activeShader);
}

void Graphics::useDefaultShader() {
    useShader(m_defaultShader);
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

void Graphics::shaderColor(glm::vec4 color) {
    glUniform4fv(glGetUniformLocation(m_activeShader, "color"), 1, glm::value_ptr(color));
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

void Graphics::shaderUseFog(bool use) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useFog"), use);
}

void Graphics::shaderUseFog(bool use, float fogNear, float fogFar, glm::vec3 color) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useFog"), use);
    glUniform1f(glGetUniformLocation(m_activeShader, "fogNear"), fogNear);
    glUniform1f(glGetUniformLocation(m_activeShader, "fogFar"), fogFar);
    glUniform3fv(glGetUniformLocation(m_activeShader, "fogColor"), 1, glm::value_ptr(color));
}

void Graphics::shaderUseLight(bool use) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useLight"), use);
}

void Graphics::shaderUseLight(bool use, int type, glm::vec3 pos) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useLight"), use);
    glUniform1i(glGetUniformLocation(m_activeShader, "lightType"), type);
    glUniform3fv(glGetUniformLocation(m_activeShader, "lightPosition"), 1, glm::value_ptr(pos));
}

void Graphics::drawQuad() {
    m_pQuad->drawArray();
}

void Graphics::drawBox() {
    m_pBox->drawArray();
}

void Graphics::drawCylinder() {
    m_pCylinder->drawArray();
}

void Graphics::drawSphere() {
    m_pSphere->drawArray();
}

void Graphics::useUiShader() {
    useShader(m_uiShader);
}

void Graphics::uisOrthoTransform(float left, float right, float bottom, float top) {
    glm::mat4 m = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    glUniformMatrix4fv(glGetUniformLocation(m_activeShader, "p"), 1, GL_FALSE, glm::value_ptr(m));
}

void Graphics::uisColor(glm::vec4 color) {
    glUniform4fv(glGetUniformLocation(m_activeShader, "color"), 1, glm::value_ptr(color));
}

void Graphics::uisQuad(float left, float right, float bottom, float top) {
    glm::mat4 m(1.f);
    m = glm::translate(m, glm::vec3(left, bottom, 0.f));
    m = glm::scale(m, glm::vec3(right-left, top-bottom, 1.f));

    shaderMTransform(m);
    m_uiQuad->drawArray();
}
