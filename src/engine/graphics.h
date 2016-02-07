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
    GLuint m_activeShader;

    std::map<std::string, GLuint> m_textures;

    Primitive *m_pQuad;
    Primitive *m_pCylinder;

public:
    Camera *camera;

public:
    void initializeGL();

    // Texture helpers
    GLuint loadTextureFromQRC(const char *path);
    void putTexture(const char *name, GLuint tex);
    GLuint getTexture(const char *name);

    void useDefaultShader();
    void useShader(GLuint shader);

    // Default shader helpers
    void shaderPvTransformFromCamera();
    void shaderMTransform(glm::mat4 m);
    void shaderColor(glm::vec3 color);
    void shaderUseTexture(bool use);
    void shaderBindTexture(GLuint tex);
    void shaderBindTexture(const char *name);
    void shaderUnbindTexture();

    // Draw some primitives from the ones included with this object
    void drawQuad();
    void drawCylinder();
};

}
