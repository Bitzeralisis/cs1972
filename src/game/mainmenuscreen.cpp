#include "mainmenuscreen.h"
#include "gamescreen.h"
#include <QKeyEvent>

using namespace Platformer;

MainMenuScreen::MainMenuScreen(CS1972Engine::Game *parent)
    : Screen(parent)
{ }

MainMenuScreen::~MainMenuScreen() { }


void MainMenuScreen::tick(float) { }

void MainMenuScreen::draw() {
    graphics().useUiShader();
    graphics().uisOrthoTransform(0.f, parent->width(), parent->height(), 0.f);

    graphics().uisColor(glm::vec4(1.f));
    graphics().shaderUseTexture(true);
    graphics().shaderBindTexture("mainmenu");
    graphics().uisQuad(0.f, 256.f, parent->height()-64.f, parent->height());

    graphics().shaderUnbindTexture();
    graphics().useShader(0);
}

void MainMenuScreen::mousePressEvent(QMouseEvent *) { }
void MainMenuScreen::mouseMoveEvent(QMouseEvent *) { }
void MainMenuScreen::mouseReleaseEvent(QMouseEvent *) { }
void MainMenuScreen::wheelEvent(QWheelEvent *) { }

void MainMenuScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_1:
        parent->popScreen();
        parent->pushScreen(new GameScreen(parent, ":/obj/level_easy.obj"));
        delete this;
        break;
    case Qt::Key_2:
        parent->popScreen();
        parent->pushScreen(new GameScreen(parent, ":/obj/level_hard.obj"));
        delete this;
        break;
    case Qt::Key_3:
        parent->popScreen();
        parent->pushScreen(new GameScreen(parent, ":/obj/level_island.obj"));
        delete this;
        break;
    default:
        break;
    }
}

void MainMenuScreen::keyReleaseEvent(QKeyEvent *) { }
void MainMenuScreen::resizeEvent(int, int) { }
