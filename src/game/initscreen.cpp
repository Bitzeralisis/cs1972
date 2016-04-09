#include "initscreen.h"
#include "../engine/primitive.h"
#include "../engine/graphics/bloommodule.h"
#include "../engine/graphics/deferredmodule.h"
#include "../engine/graphics/shadermodule.h"
#include "../engine/graphics/uishadermodule.h"

using namespace COG;

InitScreen::InitScreen(CS1972Engine::Game *parent)
    : Screen(parent)
{
    graphics().deferred()->initGbuffer(parent->width(), parent->height());
    graphics().bloom()->initBuffers(parent->width(), parent->height());
}

InitScreen::~InitScreen() {
    graphics().deferred()->cleanupGbuffer();
    graphics().bloom()->cleanupBuffers();
}

void InitScreen::tick(float seconds) {
    m_time += seconds;
}

void InitScreen::draw() {
    float dx = -2.f * glm::mod(m_time, 9.f);

    // Set up camera
    graphics().camera()->position(glm::vec3(0.f));
    graphics().camera()->fovy(0.75f*glm::half_pi<float>());
    graphics().camera()->near(0.1f);
    graphics().camera()->far(50.f);

    // Draw to g-buffer
    graphics().deferred()->bindGbuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render geometry to g-buffer
    graphics().deferred()->useGbufferShader();
    graphics().shader()->pvTransformFromCamera();
    graphics().shader()->useFog(true, 40.f, 50.f, glm::vec3(0.f));
    graphics().shader()->useTexture(false);
    for (int x = -3; x < 3; ++x)
        for (int y = -3; y < 3; ++y)
            for (int i = 0; i < 20; ++i) {
                if (x == -2 && y == -2 && (i % 3 == 0)) {
                    graphics().deferred()->glowColor(glm::vec4(3.f, 0.1f, 0.1f, 1.f));
                    graphics().shader()->color(glm::vec4(0.f));
                } else if (x == 1 && y == 1 && (i % 3 == 0)) {
                    graphics().deferred()->glowColor(glm::vec4(0.1f, 0.1f, 3.f, 1.f));
                    graphics().shader()->color(glm::vec4(0.f));
                } else {
                    graphics().deferred()->glowColor(glm::vec4(0.f));
                    graphics().shader()->color(glm::vec4(1.f));
                }
                graphics().shader()->mTransform(glm::translate(glm::mat4(1.f), glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f)));
                graphics().pBox()->drawArray();
            }
    graphics().useShader(0);

    // Stop drawing to g-buffer
    graphics().deferred()->unbindGbuffer();

    // Draw to hdr buffer
    graphics().bloom()->bindHdrBuffer();
    glClear(GL_COLOR_BUFFER_BIT);

    // Prepare to do full-screen filters
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    // Render lights in deferred pass
    graphics().deferred()->useDeferredShader();
    graphics().shader()->pvTransformFromCamera();
    int x = 1;
    int y = 1;
    for (int i = -20; i < 20; i += 3) {
        graphics().deferred()->lightPoint(glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(0.f, 0.f, 2.3f), glm::vec3(1.f, 2.f/08.f, 1.f/064.f));
    }
    x = -2;
    y = -2;
    for (int i = -20; i < 20; i += 3) {
        graphics().deferred()->lightPoint(glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(2.3f, 0.f, 0.f), glm::vec3(1.f, 2.f/08.f, 1.f/064.f));
    }
    graphics().useShader(0);

    // Blend in glow texture
    graphics().uishader()->use();
    graphics().uishader()->orthoTransform(0.f, 1.f, 1.f, 0.f);
    graphics().uishader()->color(glm::vec4(1.f));
    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture(graphics().deferred()->glowTex());
    graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
    graphics().useShader(0);

    // Stop drawing to hdr buffer
    graphics().bloom()->unbindBuffer();

    // Draw to blur buffer
    graphics().bloom()->bindBlurBuffer();
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw bright parts of hdr buffer to blur buffer
    graphics().bloom()->useBrightShader();
    graphics().uishader()->orthoTransform(0.f, 1.f, 1.f, 0.f);
    graphics().shader()->bindTexture(graphics().bloom()->hdrTex());
    graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
    graphics().useShader(0);

    // Stop drawing to blur buffer
    graphics().bloom()->unbindBuffer();

    // Blur the blur buffer
    graphics().bloom()->blurBlurBuffer();

    // Draw hdr buffer to screen
    graphics().uishader()->use();
    graphics().uishader()->orthoTransform(0.f, 1.f, 1.f, 0.f);
    graphics().uishader()->color(glm::vec4(1.f));
    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture(graphics().bloom()->hdrTex());
    graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);

    // Draw blur buffer to screen
    graphics().shader()->bindTexture(graphics().bloom()->blurTex());
    graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
    graphics().useShader(0);

    // Stop doing full-screen filters
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void InitScreen::mousePressEvent(QMouseEvent *event) { }

void InitScreen::mouseMoveEvent(QMouseEvent *event) { }

void InitScreen::mouseReleaseEvent(QMouseEvent *event) { }

void InitScreen::wheelEvent(QWheelEvent *event) { }

void InitScreen::keyPressEvent(QKeyEvent *event) { }

void InitScreen::keyReleaseEvent(QKeyEvent *event) { }

void InitScreen::resizeEvent(int w, int h) {
    graphics().deferred()->cleanupGbuffer();
    graphics().bloom()->cleanupBuffers();
    graphics().deferred()->initGbuffer(w, h);
    graphics().bloom()->initBuffers(w, h);
}
