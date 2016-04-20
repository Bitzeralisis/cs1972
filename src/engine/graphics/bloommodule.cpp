#include "bloommodule.h"
#include "../graphics.h"
#include "../primitive.h"
#include "../../util/ResourceLoader.h"

using namespace CS1972Engine;

BloomModule::BloomModule(Graphics *parent)
    : m_parent(parent)
{
    m_brightShader = ResourceLoader::loadShaders(":/shaders/2d.vert", ":/shaders/bloombright.frag");
    m_blurShader = ResourceLoader::loadShaders(":/shaders/passthrough.vert", ":/shaders/bloomblur.frag");
}

BloomModule::~BloomModule() {
    glDeleteShader(m_brightShader);
    glDeleteShader(m_blurShader);
}

void BloomModule::initBuffers(int width, int height) {
    glGenFramebuffers(1, &m_hdr);
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdr);

    glGenTextures(1, &m_hdrTex);
    glBindTexture(GL_TEXTURE_2D, m_hdrTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hdrTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &m_blur);
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur);

    glGenTextures(1, &m_blurTex);
    glBindTexture(GL_TEXTURE_2D, m_blurTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDrawBuffers(1, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &m_pingpong);
    glBindFramebuffer(GL_FRAMEBUFFER, m_pingpong);

    glGenTextures(1, &m_pingpongTex);
    glBindTexture(GL_TEXTURE_2D, m_pingpongTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDrawBuffers(1, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_parent->useShader(m_blurShader);
    glUniform2f(glGetUniformLocation(m_blurShader, "screenSize"), width, height);
    m_parent->useShader(0);
}

void BloomModule::cleanupBuffers() {
    glDeleteTextures(1, &m_hdrTex);
    glDeleteFramebuffers(1, &m_hdr);
    glDeleteTextures(1, &m_blurTex);
    glDeleteFramebuffers(1, &m_blur);
    glDeleteTextures(1, &m_pingpongTex);
    glDeleteFramebuffers(1, &m_pingpong);
}

void BloomModule::bindHdrBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdr);
}

void BloomModule::bindBlurBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_blur);
}

void BloomModule::unbindBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomModule::useBrightShader() {
    m_parent->useShader(m_brightShader);
}

void BloomModule::blurBlurBuffer() {
    m_parent->useShader(m_blurShader);

    for (int i = 0; i < 1; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingpong);
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform2f(glGetUniformLocation(m_blurShader, "direction"), 0.f, 1.f);
        glBindTexture(GL_TEXTURE_2D, m_blurTex);
        m_parent->fsQuad()->drawArray();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, m_blur);
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform2f(glGetUniformLocation(m_blurShader, "direction"), 1.f, 0.f);
        glBindTexture(GL_TEXTURE_2D, m_pingpongTex);
        m_parent->fsQuad()->drawArray();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    m_parent->useShader(0);
}
