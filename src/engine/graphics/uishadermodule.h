#pragma once

#include "../../util/CommonIncludes.h"

namespace CS1972Engine {

class Graphics;

class UiShaderModule {
public:
    explicit UiShaderModule(Graphics *parent);
    ~UiShaderModule();

private:
    Graphics *m_parent;
    GLuint m_shader;

    float m_left;
    float m_right;
    float m_bottom;
    float m_top;

public:
    void use();
    void orthoTransform(float left, float right, float bottom, float top);
    void color(glm::vec4 color);
    void drawQuad(float left, float right, float bottom, float top);
    glm::vec3 cameraSpaceToUisSpace(glm::vec3 pos);
};

}
