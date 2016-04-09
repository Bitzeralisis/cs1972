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

class BloomModule;
class DeferredModule;
class Primitive;
class ShaderModule;
class UiShaderModule;

/**
 * @brief The Graphics class
 * Helper class for OpenGL stuff.
 */
class Graphics {
public:
    Graphics();
    explicit Graphics(Camera *camera);
    virtual ~Graphics();

private:
    GLuint m_activeShader;

    std::map<std::string, GLuint> m_textures;
    std::map<std::string, OBJ *> m_objs;
    std::map<std::string, Primitive *> m_primitives;

    Primitive *m_pQuad;
    Primitive *m_pBox;
    Primitive *m_pCylinder;
    Primitive *m_pSphere;
    Primitive *m_uiQuad;
    Primitive *m_fsQuad;

private:
    Camera *m_camera;

    ShaderModule *m_shader;
    DeferredModule *m_deferred;
    BloomModule *m_bloom;
    UiShaderModule *m_uishader;

public:
    void initializeGL();

    VALUE_ACCESSOR(Camera *,camera)

    // Texture, OBJ, Primitive maps
    GLuint loadTextureFromQRC(const char *path);
    Primitive *loadPrimitiveFromOBJ(OBJ *obj);
    MAP_OPS(GLuint,Texture,texture)
    MAP_OPS(OBJ *,Obj,obj)
    MAP_OPS(Primitive *,Primitive,primitive)

    VALUE_ACCESSOR(const Primitive *,pQuad)
    VALUE_ACCESSOR(const Primitive *,pBox)
    VALUE_ACCESSOR(const Primitive *,pCylinder)
    VALUE_ACCESSOR(const Primitive *,pSphere)
    VALUE_ACCESSOR(const Primitive *,uiQuad)
    VALUE_ACCESSOR(const Primitive *,fsQuad)

    // Shader modules
    void useShader(GLuint shader);
    VALUE_ACCESSOR(GLuint,activeShader)
    VALUE_ACCESSOR(ShaderModule *,shader)
    VALUE_ACCESSOR(DeferredModule *,deferred)
    VALUE_ACCESSOR(BloomModule *,bloom)
    VALUE_ACCESSOR(UiShaderModule *,uishader)
};

}
