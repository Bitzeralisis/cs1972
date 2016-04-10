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

    Primitive *m_indices;
    GLuint m_posLifeTex;
    GLuint m_velTex;
    GLuint m_result;
    GLuint m_resultTex;

public:
    VALUE_ACCESSOR(GLuint,posLifeTex)
    VALUE_ACCESSOR(GLuint,velTex)
    VALUE_ACCESSOR(GLuint,resultTex)

    void init(int width, int height);
    void cleanup();
    void updateParticles(float seconds);
    void drawParticles();
};

}
