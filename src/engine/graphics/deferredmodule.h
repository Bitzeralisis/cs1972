#pragma once

#include "../../util/CommonIncludes.h"
#include "../../util/HeaderUtils.h"

namespace CS1972Engine {

class Graphics;

class DeferredModule {
public:
    explicit DeferredModule(Graphics *parent);
    ~DeferredModule();

private:
    Graphics *m_parent;
    GLuint m_gShader;
    GLuint m_dShader;

    GLuint m_fbo;
    GLuint m_normalTex;
    GLuint m_colorTex;
    GLuint m_glowTex;
    GLuint m_depthTex;

    float m_cutoff = 0.f;

    int m_numFsLights;
    int m_numLightVolumes;

public:
    VALUE_ACCESSOR(GLuint,normalTex)
    VALUE_ACCESSOR(GLuint,colorTex)
    VALUE_ACCESSOR(GLuint,glowTex)
    VALUE_ACCESSOR(GLuint,depthTex)

    // G-buffer
    void initGbuffer(int width, int height);
    void cleanupGbuffer();
    void bindGbuffer();
    void unbindGbuffer();

    // Geometry pass shader
    void useGbufferShader();
    void glowColor(glm::vec4 color);
    void useGlowTexture(bool use);
    void bindGlowTexture(GLuint tex);
    void bindGlowTexture(const char *name);
    void unbindGlowTexture();
    void blitGbufferDepthToBb(int width, int height);

    // Deferred pass shader
    void useDeferredShader();
    void setLightCutoff(float cutoff);
    void lightAmbient(glm::vec3 color);
    void lightDirectional(glm::vec3 dir, glm::vec3 color);
    void lightPoint(glm::vec3 pos, glm::vec3 color, glm::vec3 atten);
    void lightPoint(glm::vec3 pos, glm::vec3 color, float radius);
    void resetLightCounts();
    void printLightCounts();
};

}
