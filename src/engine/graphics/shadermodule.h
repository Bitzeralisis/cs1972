#pragma once

#include "../../util/CommonIncludes.h"

namespace CS1972Engine {

class Graphics;

class ShaderModule {
public:
    explicit ShaderModule(Graphics *parent);
    ~ShaderModule();

private:
    Graphics *m_parent;
    GLuint m_shader;

public:
    void use();
    void pvTransformFromCamera();
    void mTransform(glm::mat4 m);
    void color(glm::vec4 color);
    void useTexture(bool use);
    void bindTexture(GLuint tex);
    void bindTexture(const char *name);
    void unbindTexture();
    void useFog(bool use);
    void useFog(bool use, float near, float far, glm::vec3 color);
    void useLight(bool use);
    void useLight(bool use, int type, glm::vec3 pos);
    glm::mat4 billboardMTransform(glm::vec3 position, glm::vec2 size = glm::vec2(1.f), float rotation = 0.f);
};

}
