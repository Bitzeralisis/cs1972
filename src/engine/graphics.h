#pragma once

#include "camera.h"
#include "../util/CommonIncludes.h"
#include <map>
#include <string>

namespace CS1972Engine {

/**
 * @brief The Graphics class
 * Helper class for OpenGL stuff.
 */
class Graphics {
public:
    Graphics(): camera(new Camera()) {}
    explicit Graphics(Camera *cam): camera(cam) {}

private:
    GLuint m_defaultShader;
    GLuint m_activeShader;

    GLuint m_quadVbo;
    GLuint m_quadVao;
    int m_quadNumVertices;

    std::map<std::string, GLuint> m_textures;

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
    void drawQuad();
};

}
