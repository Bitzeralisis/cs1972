#include "initscreen.h"
#include "../engine/primitive.h"
#include "../engine/graphics/deferredmodule.h"
#include "../engine/graphics/shadermodule.h"
#include "../engine/graphics/uishadermodule.h"

using namespace COG;

InitScreen::InitScreen(CS1972Engine::Game *parent)
    : Screen(parent)
{

}

InitScreen::~InitScreen() {

}

void InitScreen::tick(float seconds) {
    m_time += seconds;
}

void InitScreen::draw() {
    float dx = -2.f * glm::mod(m_time, 9.f);

    graphics().camera()->position(glm::vec3(0.f));
    graphics().camera()->fovy(0.75f*glm::half_pi<float>());
    graphics().camera()->near(0.1f);
    graphics().camera()->far(50.f);

    graphics().deferred()->initGbuffer(parent->width(), parent->height());

    graphics().deferred()->bindGbuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    graphics().deferred()->useGbufferShader();
    graphics().shader()->pvTransformFromCamera();
    graphics().shader()->useFog(true, 40.f, 50.f, glm::vec3(0.f));
    graphics().shader()->useTexture(false);
    graphics().shader()->color(glm::vec4(1.f));
    for (int x = -3; x < 3; ++x)
        for (int y = -3; y < 3; ++y)
            for (int i = 0; i < 20; ++i) {
                if (x == -2 && y == -2 && (i % 3 == 0)) {
                    graphics().deferred()->glowColor(glm::vec4(2.f, 0.f, 0.f, 1.f));
                } else if (x == 1 && y == 1 && (i % 3 == 0)) {
                    graphics().deferred()->glowColor(glm::vec4(0.f, 0.f, 2.f, 1.f));
                } else {
                    graphics().deferred()->glowColor(glm::vec4(0.f));
                }
                graphics().shader()->mTransform(glm::translate(glm::mat4(1.f), glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f)));
                graphics().pBox()->drawArray();
            }
    graphics().deferred()->unbindGbuffer();
    graphics().useShader(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    graphics().deferred()->useDeferredShader();
    graphics().shader()->pvTransformFromCamera();
    int x = 1;
    int y = 1;
    for (int i = -20; i < 20; i += 3) {
        graphics().deferred()->lightPoint(glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 2.f/10.f, 1.f/100.f));
    }
    x = -2;
    y = -2;
    for (int i = -20; i < 20; i += 3) {
        graphics().deferred()->lightPoint(glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 2.f/10.f, 1.f/100.f));
    }
    graphics().useShader(0);

    graphics().uishader()->use();
    graphics().uishader()->orthoTransform(0.f, 1.f, 1.f, 0.f);
    graphics().uishader()->color(glm::vec4(1.f));
    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture(graphics().deferred()->glowTex());
    graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
    graphics().useShader(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    graphics().deferred()->cleanupGbuffer();
}

void InitScreen::mousePressEvent(QMouseEvent *event) { }

void InitScreen::mouseMoveEvent(QMouseEvent *event) { }

void InitScreen::mouseReleaseEvent(QMouseEvent *event) { }

void InitScreen::wheelEvent(QWheelEvent *event) { }

void InitScreen::keyPressEvent(QKeyEvent *event) { }

void InitScreen::keyReleaseEvent(QKeyEvent *event) { }

void InitScreen::resizeEvent(int w, int h) { }
