#pragma once

#include "camera.h"
#include "../util/CommonIncludes.h"
#include <map>
#include <string>

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

    Primitive *m_pQuad;
    Primitive *m_pBox;
    Primitive *m_pCylinder;
    Primitive *m_uiQuad;

public:
    Camera *camera;

public:
    void initializeGL();

    // Texture helpers
    GLuint loadTextureFromQRC(const char *path);
    void putTexture(const char *name, GLuint tex);
    GLuint getTexture(const char *name);

    void useDefaultShader();
    void useUiShader();
    void useShader(GLuint shader);

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

    // Draw some primitives from the ones included with this object
    void drawQuad();
    void drawBox();
    void drawCylinder();

    // Ui shader helpers
    void uisOrthoTransform(float left, float right, float bottom, float top);
    void uisColor(glm::vec4 color);
    void uisQuad(float left, float right, float bottom, float top);
};

}
