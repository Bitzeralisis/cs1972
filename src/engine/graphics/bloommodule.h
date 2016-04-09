#pragma once

#include "../../util/CommonIncludes.h"
#include "../../util/HeaderUtils.h"

namespace CS1972Engine {

class Graphics;

class BloomModule {
public:
    explicit BloomModule(Graphics *parent);
    virtual ~BloomModule();

private:
    Graphics *m_parent;
    GLuint m_brightShader;
    GLuint m_blurShader;

    GLuint m_hdr;
    GLuint m_hdrTex;
    GLuint m_blur;
    GLuint m_blurTex;
    GLuint m_pingpong;
    GLuint m_pingpongTex;

public:
    VALUE_ACCESSOR(GLuint,hdrTex)
    VALUE_ACCESSOR(GLuint,blurTex)

    void initBuffers(int width, int height);
    void cleanupBuffers();
    void bindHdrBuffer();
    void bindBlurBuffer();
    void unbindBuffer();

    void useBrightShader();
    void blurBlurBuffer();
};

}
