#pragma once

#include "../../util/CommonIncludes.h"
#include "../../util/HeaderUtils.h"

namespace CS1972Engine {

class Graphics;
class Primitive;

class ParticleModule {
public:
    explicit ParticleModule(Graphics *parent);
    ~ParticleModule();

private:
    Graphics *m_parent;
    GLuint m_physicsShader;
    GLuint m_drawShader;

    GLuint m_width;
    GLuint m_height;
    int m_currRow = 0;

    Primitive *m_indices;
    GLuint m_posLifeTex;
    GLuint m_velTex;
    GLuint m_result;
    GLuint m_resultTex;

public:
    VALUE_ACCESSOR(GLuint,width)
    VALUE_ACCESSOR(GLuint,height)
    VALUE_ACCESSOR(GLuint,posLifeTex)
    VALUE_ACCESSOR(GLuint,velTex)
    VALUE_ACCESSOR(GLuint,resultTex)

    void init(int width, int height);
    void cleanup();
    void usePhysicsShader();
    void globalVelocity(glm::vec3 vel);
    void putParticles(int numParticles, GLfloat *posLife, GLfloat *vel);
    void updateParticles(float seconds);
    void useParticleShader();
    void particleStyle(const char *tex, float size);
    void drawParticles();
};

}
