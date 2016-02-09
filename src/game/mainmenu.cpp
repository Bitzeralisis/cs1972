#include "mainmenu.h"
#include "gamescreen.h"
#include "../engine/game.h"
#include "../util/CommonIncludes.h"
#include <QKeyEvent>

using namespace WarmupGame;

MainMenu::MainMenu(CS1972Engine::Game *parent)
    : Screen(parent)
{ }

void MainMenu::tick() { }

void MainMenu::draw() {
    graphics().useUiShader();
    graphics().uisOrthoTransform(0.f, parent->width(), parent->height(), 0.f);

    graphics().shaderUseTexture(true);
    graphics().shaderColor(glm::vec3(1.f, 1.f, 1.f));
    graphics().shaderBindTexture("howto");
    graphics().uisQuad(0.f, parent->width(), 0.f, parent->height());

    // Clean up
    graphics().shaderUnbindTexture();
    graphics().useShader(0);
}

void MainMenu::mousePressEvent(QMouseEvent *) { }

void MainMenu::mouseMoveEvent(QMouseEvent *) { }

void MainMenu::mouseReleaseEvent(QMouseEvent *) { }

void MainMenu::wheelEvent(QWheelEvent *) { }

void MainMenu::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return) {
        parent->popScreen();
        parent->pushScreen(new GameScreen(this->parent));
        delete this;
    }
}

void MainMenu::keyReleaseEvent(QKeyEvent *) { }

void MainMenu::resizeEvent(int, int) { }
