#pragma once

#include "camera.h"
#include "../util/CommonIncludes.h"
#include <map>
#include <string>

#define MAP_PUT(TYPE,VAR1,VAR2) inline void put##VAR1(const char *name, TYPE VAR2) { m_##VAR2##s[name] = VAR2; }
#define MAP_HAS(TYPE,VAR1,VAR2) inline bool has##VAR1(const char *name) const { return m_##VAR2##s.count(name); }
#define MAP_GET(TYPE,VAR1,VAR2) inline TYPE get##VAR1(const char *name) { return m_##VAR2##s[name]; }
#define MAP_DELETE_DCLR(TYPE,VAR1,VAR2) inline void delete##VAR1(const char *name, TYPE VAR2);
#define MAP_OPS(TYPE,VAR1,VAR2) MAP_PUT(TYPE,VAR1,VAR2) MAP_HAS(TYPE,VAR1,VAR2) MAP_GET(TYPE,VAR1,VAR2)

class OBJ;
class QFont;

namespace CS1972Engine {

class Primitive;

/**
 * @brief The Graphics class
 * Helper class for OpenGL stuff.
 */
class Graphics {
public:
    Graphics(): camera(new Camera()) {}
    explicit Graphics(Camera *cam): camera(cam) {}
    virtual ~Graphics();

private:
    GLuint m_defaultShader;
    GLuint m_uiShader;
    GLuint m_activeShader;

    std::map<std::string, GLuint> m_textures;
    std::map<std::string, OBJ *> m_objs;
    std::map<std::string, Primitive *> m_primitives;

    Primitive *m_pQuad;
    Primitive *m_pBox;
    Primitive *m_pCylinder;
    Primitive *m_pSphere;
    Primitive *m_uiQuad;

public:
    Camera *camera;

public:
    void initializeGL();

    // Texture helpers
    GLuint loadTextureFromQRC(const char *path);
    MAP_OPS(GLuint,Texture,texture)

    void useDefaultShader();
    void useUiShader();
    void useShader(GLuint shader);

    // OBJ & Primitive helpers
    MAP_OPS(OBJ *,Obj,obj)
    Primitive *loadPrimitiveFromOBJ(OBJ *obj);
    MAP_OPS(Primitive *,Primitive,primitive)

    // Default shader helpers
    void shaderPvTransformFromCamera();
    void shaderMTransform(glm::mat4 m);
    void shaderColor(glm::vec4 color);
    void shaderUseTexture(bool use);
    void shaderBindTexture(GLuint tex);
    void shaderBindTexture(const char *name);
    void shaderUnbindTexture();
    void shaderUseFog(bool use);
    void shaderUseFog(bool use, float near, float far, glm::vec3 color);
    void shaderUseLight(bool use);
    void shaderUseLight(bool use, int type, glm::vec3 pos);

    // Draw some primitives from the ones included with this object
    void drawQuad();
    void drawBox();
    void drawCylinder();
    void drawSphere();

    // Ui shader helpers
    void uisOrthoTransform(float left, float right, float bottom, float top);
    void uisColor(glm::vec4 color);
    void uisQuad(float left, float right, float bottom, float top);
};

}
