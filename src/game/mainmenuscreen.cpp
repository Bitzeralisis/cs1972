#include "mainmenuscreen.h"
#include "gamescreen.h"
#include "engine/graphics/shadermodule.h"
#include "engine/graphics/uishadermodule.h"

using namespace COG;

MainMenuScreen::MainMenuScreen(CS1972Engine::Game *game)
    : CS1972Engine::Screen(game)
{
    if (!graphics().hasTexture("mainmenu"))
        graphics().putTexture("mainmenu", graphics().loadTextureFromQRC(":/images/mainmenu.png", GL_LINEAR));
    if (!graphics().hasTexture("cube1"))
        graphics().putTexture("cube1", graphics().loadTextureFromQRC(":/images/cube1.png", GL_LINEAR));
    if (!graphics().hasTexture("cube2"))
        graphics().putTexture("cube2", graphics().loadTextureFromQRC(":/images/cube2.png", GL_LINEAR));
    if (!graphics().hasTexture("diamond1"))
        graphics().putTexture("diamond1", graphics().loadTextureFromQRC(":/images/diamond1.png", GL_LINEAR));
    if (!graphics().hasTexture("enemy2"))
        graphics().putTexture("enemy2", graphics().loadTextureFromQRC(":/images/enemy2.png", GL_LINEAR));
    if (!graphics().hasTexture("particle"))
        graphics().putTexture("particle", graphics().loadTextureFromQRC(":/images/particle1.png", GL_LINEAR));
    if (!graphics().hasTexture("blur"))
        graphics().putTexture("blur", graphics().loadTextureFromQRC(":/images/particle2.png", GL_NEAREST));
    if (!graphics().hasTexture("reticle"))
        graphics().putTexture("reticle", graphics().loadTextureFromQRC(":/images/reticle.png", GL_LINEAR));
    if (!graphics().hasTexture("enemyshot"))
        graphics().putTexture("enemyshot", graphics().loadTextureFromQRC(":/images/enemyshot.png", GL_LINEAR));
    if (!graphics().hasTexture("target"))
        graphics().putTexture("target", graphics().loadTextureFromQRC(":/images/target.png", GL_NEAREST));
    if (!graphics().hasTexture("hud"))
        graphics().putTexture("hud", graphics().loadTextureFromQRC(":/images/hud.png", GL_LINEAR));

    if (!audio().hasSound("handclap.aif"))
        audio().putSound("handclap.aif", audio().createSoundSample("sound/handclap.aif"));
    if (!audio().hasSound("808hh08.aif"))
        audio().putSound("808hh08.aif", audio().createSoundSample("sound/808hh08.aif"));
    if (!audio().hasSound("noisy1.aif"))
        audio().putSound("noisy1.aif", audio().createSoundSample("sound/noisy1.aif"));
    if (!audio().hasSound("supergate-snare.aif"))
        audio().putSound("supergate-snare.aif", audio().createSoundSample("sound/supergate-snare.aif"));
    if (!audio().hasSound("popslap.aif"))
        audio().putSound("popslap.aif", audio().createSoundSample("sound/popslap.aif"));
    if (!audio().hasSound("tambourine-simple1.aif"))
        audio().putSound("tambourine-simple1.aif", audio().createSoundSample("sound/tambourine-simple1.aif"));
    if (!audio().hasSound("chewie_scratch.aif"))
        audio().putSound("chewie_scratch.aif", audio().createSoundSample("sound/chewie_scratch.aif"));
}

MainMenuScreen::~MainMenuScreen() { }

void MainMenuScreen::tick(float seconds) {

}

void MainMenuScreen::draw() {
    float W = parent->width();
    float H = parent->height();

    graphics().uishader()->use();
    graphics().uishader()->orthoTransform(0.f, W, H, 0.f);

    float ratio = glm::min(glm::min(W/1920.f, H/1080.f), 1.f);

    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture("mainmenu");
    graphics().uishader()->color(glm::vec4(1.f));
    graphics().uishader()->drawQuad(0.5f*glm::vec2(W, H), ratio*glm::vec2(1920.f, 1080.f), 0.f, 0.f, 1.f, 0.f, 1.f);

    graphics().useShader(0);
}


void MainMenuScreen::mousePressEvent(QMouseEvent *event) {
    parent->popScreen();
    parent->pushScreen(new GameScreen(parent));
}

void MainMenuScreen::mouseMoveEvent(QMouseEvent *event) { }

void MainMenuScreen::mouseReleaseEvent(QMouseEvent *event) { }

void MainMenuScreen::wheelEvent(QWheelEvent *event) { }

void MainMenuScreen::keyPressEvent(QKeyEvent *event) { }

void MainMenuScreen::keyReleaseEvent(QKeyEvent *event) { }

void MainMenuScreen::resizeEvent(int w, int h) { }
