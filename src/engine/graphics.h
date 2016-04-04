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
    GLuint m_gShader;
    GLuint m_dShader;
    GLuint m_activeShader;

    GLuint dr_fb;
    GLuint dr_gPos, dr_gNorm, dr_gColor;
    GLuint dr_gDepth;

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

    // OBJ & Primitive helpers
    MAP_OPS(OBJ *,Obj,obj)
    Primitive *loadPrimitiveFromOBJ(OBJ *obj);
    MAP_OPS(Primitive *,Primitive,primitive)

    void useShader(GLuint shader);

    // Default shader helpers
    void useDefaultShader();
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

    // Deferred shader helpers
    void dr_init(int width, int height);
    void dr_cleanup();
    void dr_bindGbuffer();
    void dr_unbindGbuffer();
    void dr_useGbufferShader();
    void dr_blitGbufferDepthToBb(int width, int height);
    VALUE_ACCESSOR(GLuint,gShader)
    VALUE_ACCESSOR(GLuint,dShader)
    void dr_useDeferredShader();
    void dr_lightAmbient(glm::vec3 color);
    void dr_lightDirectional(glm::vec3 dir, glm::vec3 color);
    void dr_lightPoint(glm::vec3 dir, glm::vec3 color, glm::vec3 atten);
    void dr_drawLight();

    // Draw some primitives from the ones included with this object
    void drawQuad();
    void drawBox();
    void drawCylinder();
    void drawSphere();

    // Ui shader helpers
    void useUiShader();
    void uisOrthoTransform(float left, float right, float bottom, float top);
    void uisColor(glm::vec4 color);
    void uisQuad(float left, float right, float bottom, float top);
};

}
