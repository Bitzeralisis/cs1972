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
    m_drawShader = Graphics::createProgram(
        Graphics::loadShaderFromQRC(":/shaders/particle.vert", GL_VERTEX_SHADER),
        Graphics::loadShaderFromQRC(":/shaders/particle.geom", GL_GEOMETRY_SHADER),
        Graphics::loadShaderFromQRC(":/shaders/particle.frag", GL_FRAGMENT_SHADER)
    );
}

ParticleModule::~ParticleModule() {
    glDeleteShader(m_physicsShader);
    glDeleteShader(m_drawShader);
}

void ParticleModule::init(int width, int height) {
    glGenTextures(1, &m_posLifeTex);
    glBindTexture(GL_TEXTURE_2D, m_posLifeTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_velTex);
    glBindTexture(GL_TEXTURE_2D, m_velTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_result);
    glBindFramebuffer(GL_FRAMEBUFFER, m_result);

    glGenTextures(1, &m_resultTex);
    glBindTexture(GL_TEXTURE_2D, m_resultTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
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
    glUniform1i(glGetUniformLocation(m_drawShader, "particle_tex"), 1);
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

void ParticleModule::putParticles(int numParticles, GLfloat *posLife, GLfloat *vel) {
    int numRows = numParticles / m_width;
    while (numRows > 0) {
        int first = m_currRow;
        int last = glm::min((int)m_height, first+numRows);

        glBindTexture(GL_TEXTURE_2D, m_posLifeTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, first, m_width, last-first, GL_RGBA, GL_FLOAT, posLife);

        glBindTexture(GL_TEXTURE_2D, m_velTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, first, m_width, last-first, GL_RGB, GL_FLOAT, vel);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_currRow = last % m_height;
        numRows -= (last-first);
        posLife += 4*m_width*(last-first);
        vel += 3*m_width*(last-first);
    }

    int lastRow = numParticles % m_width;
    if (lastRow > 0) {
        glBindTexture(GL_TEXTURE_2D, m_posLifeTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, m_currRow, lastRow, 1, GL_RGBA, GL_FLOAT, posLife);

        glBindTexture(GL_TEXTURE_2D, m_velTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, m_currRow, lastRow, 1, GL_RGB, GL_FLOAT, vel);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_currRow = (m_currRow+1) % m_height;
    }
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

void ParticleModule::useParticleShader() {
    m_parent->useShader(m_drawShader);
}

void ParticleModule::particleStyle(const char *tex, float size) {
    glUniform1f(glGetUniformLocation(m_parent->activeShader(), "particleSize"), size);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_parent->getTexture(tex));
    glActiveTexture(GL_TEXTURE0);
}

void ParticleModule::drawParticles() {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    m_parent->shader()->pvTransformFromCamera();
    glBindTexture(GL_TEXTURE_2D, m_resultTex);
    m_indices->drawArray(GL_POINTS, 0, m_width*m_height);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}
