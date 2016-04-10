#include "particlemodule.h"
#include "shadermodule.h"
#include "../graphics.h"
#include "../primitive.h"
#include "../../util/ResourceLoader.h"

using namespace CS1972Engine;

ParticleModule::ParticleModule(Graphics *parent)
    : m_parent(parent)
{
    m_physicsShader = ResourceLoader::loadShaders(":/shaders/passthrough.vert", ":/shaders/particlephysics.frag");
    m_drawShader = ResourceLoader::loadShaders(":/shaders/particle.vert", ":/shaders/particle.frag");
}

ParticleModule::~ParticleModule() {
    glDeleteShader(m_physicsShader);
    glDeleteShader(m_drawShader);
}

void ParticleModule::init(int width, int height) {
    glGenTextures(1, &m_posLifeTex);
    glBindTexture(GL_TEXTURE_2D, m_posLifeTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_velTex);
    glBindTexture(GL_TEXTURE_2D, m_velTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_result);
    glBindFramebuffer(GL_FRAMEBUFFER, m_result);

    glGenTextures(1, &m_resultTex);
    glBindTexture(GL_TEXTURE_2D, m_resultTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resultTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_parent->useShader(m_physicsShader);
    glUniform1i(glGetUniformLocation(m_physicsShader, "poslife_tex"), 0);
    glUniform1i(glGetUniformLocation(m_physicsShader, "vel_tex"), 1);
    m_parent->useShader(0);

    m_parent->useShader(m_drawShader);
    glUniform1i(glGetUniformLocation(m_drawShader, "position_tex"), 0);
    m_parent->useShader(0);

    GLfloat *indices = new GLfloat[2*width*height];
    for (int i = 0; i < width*height; ++i) {
        indices[2*i+0] = (float)(i%width)/width;
        indices[2*i+1] = (float)(i/width)/height;
    }
    m_indices = new Primitive(width*height, 2*width*height*sizeof(GLfloat), indices, 1);

    m_width = width;
    m_height = height;
}

void ParticleModule::cleanup() {
    delete m_indices;
    glDeleteTextures(1, &m_posLifeTex);
    glDeleteTextures(1, &m_velTex);
    glDeleteFramebuffers(1, &m_result);
    glDeleteTextures(1, &m_resultTex);
}

void ParticleModule::updateParticles(float seconds) {
    // Save viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    m_parent->useShader(m_physicsShader);
    glViewport(0, 0, m_width, m_height);

    glBindFramebuffer(GL_FRAMEBUFFER, m_result);
    glClear(GL_COLOR_BUFFER_BIT);
    glUniform1f(glGetUniformLocation(m_parent->activeShader(), "timestep"), seconds);
    glBindTexture(GL_TEXTURE_2D, m_posLifeTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_velTex);
    glActiveTexture(GL_TEXTURE0);
    m_parent->fsQuad()->drawArray();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_parent->useShader(0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    GLuint swap = m_posLifeTex;
    m_posLifeTex = m_resultTex;
    m_resultTex = swap;
    glBindFramebuffer(GL_FRAMEBUFFER, m_result);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resultTex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ParticleModule::drawParticles() {
    m_parent->useShader(m_drawShader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    m_parent->shader()->pvTransformFromCamera();
    glBindTexture(GL_TEXTURE_2D, m_resultTex);
    //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    m_indices->drawArray(GL_POINTS, 0, m_width*m_height);

    m_parent->useShader(0);
    glDisable(GL_BLEND);
}
