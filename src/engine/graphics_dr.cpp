#include "graphics.h"
#include "primitive.h"
#include "../util/CommonIncludes.h"

using namespace CS1972Engine;

void Graphics::dr_init(int width, int height) {
    glGenFramebuffers(1, &dr_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, dr_fb);

    glGenTextures(1, &dr_gPos);
    glBindTexture(GL_TEXTURE_2D, dr_gPos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dr_gPos, 0);

    glGenTextures(1, &dr_gNorm);
    glBindTexture(GL_TEXTURE_2D, dr_gNorm);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, dr_gNorm, 0);

    glGenTextures(1, &dr_gColor);
    glBindTexture(GL_TEXTURE_2D, dr_gColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, dr_gColor, 0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &dr_gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, dr_gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dr_gDepth);

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

    useShader(m_dShader);
    glUniform1i(glGetUniformLocation(m_dShader, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(m_dShader, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_dShader, "gColor"), 2);
    useShader(0);
}

void Graphics::dr_cleanup() {
    glDeleteTextures(1, &dr_gPos);
    glDeleteTextures(1, &dr_gNorm);
    glDeleteTextures(1, &dr_gColor);
    glDeleteRenderbuffers(1, &dr_gDepth);
    glDeleteFramebuffers(1, &dr_fb);
}

void Graphics::dr_bindGbuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, dr_fb);
}

void Graphics::dr_unbindGbuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Graphics::dr_useGbufferShader() {
    useShader(m_gShader);
}

void Graphics::dr_blitGbufferDepthToBb(int width, int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, dr_fb);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Graphics::dr_useDeferredShader() {
    useShader(m_dShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dr_gPos);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dr_gNorm);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dr_gColor);
}

void Graphics::dr_lightAmbient(glm::vec3 color) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useLight"), false);
    glUniform3fv(glGetUniformLocation(m_activeShader, "lightColor"), 1, glm::value_ptr(color));
}

void Graphics::dr_lightDirectional(glm::vec3 dir, glm::vec3 color) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useLight"), true);
    glUniform1i(glGetUniformLocation(m_activeShader, "lightType"), 1);
    glUniform3fv(glGetUniformLocation(m_activeShader, "lightPosition"), 1, glm::value_ptr(dir));
    glUniform3fv(glGetUniformLocation(m_activeShader, "lightColor"), 1, glm::value_ptr(color));
}

void Graphics::dr_lightPoint(glm::vec3 pos, glm::vec3 color, glm::vec3 atten) {
    glUniform1i(glGetUniformLocation(m_activeShader, "useLight"), true);
    glUniform1i(glGetUniformLocation(m_activeShader, "lightType"), 2);
    glUniform3fv(glGetUniformLocation(m_activeShader, "lightPosition"), 1, glm::value_ptr(pos));
    glUniform3fv(glGetUniformLocation(m_activeShader, "lightColor"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(m_activeShader, "lightAtten"), 1, glm::value_ptr(atten));
}

void Graphics::dr_drawLight() {
    m_uiQuad->drawArray();
}
