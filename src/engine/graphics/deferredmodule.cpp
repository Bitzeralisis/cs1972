#include "deferredmodule.h"
#include "../graphics.h"
#include "../primitive.h"
#include "../../util/ResourceLoader.h"
#include "../../csm/csm.h"

using namespace CS1972Engine;

DeferredModule::DeferredModule(Graphics *parent)
    : m_parent(parent)
{
    m_gShader = ResourceLoader::loadShaders(":/shaders/gbuffer.vert", ":/shaders/gbuffer.frag");
    m_dShader = ResourceLoader::loadShaders(":/shaders/deferred.vert", ":/shaders/deferred.frag");
}

DeferredModule::~DeferredModule() {
    glDeleteProgram(m_gShader);
    glDeleteProgram(m_dShader);
}

void DeferredModule::initGbuffer(int width, int height) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_normalTex);
    glBindTexture(GL_TEXTURE_2D, m_normalTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_normalTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_colorTex);
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_colorTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_glowTex);
    glBindTexture(GL_TEXTURE_2D, m_glowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_glowTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenTextures(1, &m_depthTex);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    default:
        break;
    }
    std::cout.flush();*/

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_parent->useShader(m_gShader);
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "glowTex"), 1);
    m_parent->useShader(0);

    m_parent->useShader(m_dShader);
    glUniform1i(glGetUniformLocation(m_dShader, "gNormal"), 0);
    glUniform1i(glGetUniformLocation(m_dShader, "gColor"), 1);
    glUniform1i(glGetUniformLocation(m_dShader, "gDepth"), 2);
    glUniform2f(glGetUniformLocation(m_dShader, "screenSize"), width, height);
    m_parent->useShader(0);
}

void DeferredModule::cleanupGbuffer() {
    glDeleteTextures(1, &m_normalTex);
    glDeleteTextures(1, &m_colorTex);
    glDeleteTextures(1, &m_glowTex);
    glDeleteTextures(1, &m_depthTex);
    glDeleteFramebuffers(1, &m_fbo);
}

void DeferredModule::bindGbuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void DeferredModule::unbindGbuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredModule::useGbufferShader() {
    m_parent->useShader(m_gShader);
}

void DeferredModule::glowColor(glm::vec4 color) {
    glUniform4fv(glGetUniformLocation(m_parent->activeShader(), "glow"), 1, glm::value_ptr(color));
}

void DeferredModule::useGlowTexture(bool use) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useGlowTexture"), use);
}

void DeferredModule::bindGlowTexture(GLuint tex) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE0);
}

void DeferredModule::bindGlowTexture(const char *name) {
    bindGlowTexture(m_parent->getTexture(name));
}

void DeferredModule::unbindGlowTexture() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

void DeferredModule::blitGbufferDepthTo(int width, int height, GLuint fbo) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DeferredModule::useDeferredShader() {
    m_parent->useShader(m_dShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_normalTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);
    glActiveTexture(GL_TEXTURE0);
}

void DeferredModule::lightCutoff(float cutoff) {
    m_cutoff = cutoff;
    glUniform1f(glGetUniformLocation(m_parent->activeShader(), "lightCutoff"), m_cutoff);
}

void DeferredModule::lightAmbient(glm::vec3 color) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useLight"), false);
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "lightColor"), 1, glm::value_ptr(color));
    glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "pvm"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
    m_parent->fsQuad()->drawArray();
    ++m_numFsLights;
}

void DeferredModule::lightDirectional(glm::vec3 dir, glm::vec3 color) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useLight"), true);
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "lightType"), 1);
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "lightPosition"), 1, glm::value_ptr(dir));
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "lightColor"), 1, glm::value_ptr(color));
    glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "pvm"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
    m_parent->fsQuad()->drawArray();
    ++m_numFsLights;
}

void DeferredModule::lightPoint(glm::vec3 pos, glm::vec3 color, glm::vec3 atten) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useLight"), true);
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "lightType"), 2);
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "lightPosition"), 1, glm::value_ptr(pos));
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "lightColor"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "lightAtten"), 1, glm::value_ptr(atten));
    // Figure out what the maximum distance this light can affect is so we can draw it as a volume
    float dist, neg;
    float cutoff = glm::min((m_cutoff == 0.f ? 512.f : 1.f/m_cutoff), 512.f);
    int inside = csm::solve_quadratic(atten.z, atten.y, atten.x - cutoff*glm::max(glm::max(color.r, color.g), color.b), dist, neg);
    if (inside && dist*dist < glm::distance2(pos, m_parent->camera()->position())+1.f) {
        // Outside the volume - draw a sphere
        glm::mat4 pv = m_parent->camera()->perspectiveMatrix() * m_parent->camera()->viewMatrix();
        glm::mat4 m(1.f);
        m = glm::translate(m, pos);
        m = glm::scale(m, glm::vec3(dist));
        glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "pvm"), 1, GL_FALSE, glm::value_ptr(pv*m));
        m_parent->pSphere()->drawArray();
        ++m_numLightVolumes;
    } else {
        // Inside the volume - just draw full screen
        glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "pvm"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
        m_parent->fsQuad()->drawArray();
        ++m_numFsLights;
    }
}

void DeferredModule::lightPoint(glm::vec3 pos, glm::vec3 color, float radius) {
    lightPoint(pos, color, glm::vec3(1.f, 2.f/radius, 1.f/radius/radius));
}

void DeferredModule::resetLightCounts() {
    m_numFsLights = 0;
    m_numLightVolumes = 0;
}

void DeferredModule::printLightCounts() {
    std::cout << "deferred module (full screen lights: " << m_numFsLights << ", light volumes: " << m_numLightVolumes << ")\n";
}
