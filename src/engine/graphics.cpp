#include "graphics.h"
#include "primitive.h"
#include "graphics/deferredmodule.h"
#include "graphics/shadermodule.h"
#include "graphics/uishadermodule.h"
#include "../util/CommonIncludes.h"
#include "../util/NewCylinderData.h"
#include "../util/obj.h"
#include "../util/ResourceLoader.h"
#include "../util/SphereData.h"
#include <QGLWidget>
#include <QImage>

using namespace CS1972Engine;

Graphics::~Graphics() {
    for (std::map<std::string, GLuint>::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
        glDeleteTextures(1, &(it->second));

    for (std::map<std::string, OBJ *>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
        delete it->second;

    delete m_pQuad;
    delete m_pBox;
    delete m_pCylinder;
    delete m_pSphere;
    delete m_uiQuad;
    delete m_fsQuad;

    delete m_camera;
}

void Graphics::initializeGL() {
    m_shader = new ShaderModule(this);
    m_deferred = new DeferredModule(this);
    m_uishader = new UiShaderModule(this);

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

    GLfloat fsQuadData[48] = {
        -1.f,-1.f,0.f, 0.f,0.f,-1.f, 0.f,1.f,
         1.f,-1.f,0.f, 0.f,0.f,-1.f, 1.f,1.f,
        -1.f, 1.f,0.f, 0.f,0.f,-1.f, 0.f,0.f,
        -1.f, 1.f,0.f, 0.f,0.f,-1.f, 0.f,0.f,
         1.f,-1.f,0.f, 0.f,0.f,-1.f, 1.f,1.f,
         1.f, 1.f,0.f, 0.f,0.f,-1.f, 1.f,0.f
    };
    m_fsQuad = new Primitive(quadNumVertices, quadDataSize, fsQuadData);
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
