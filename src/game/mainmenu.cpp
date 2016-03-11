#include "mainmenu.h"
#include "gamescreen.h"
#include "../engine/game.h"
#include "../util/CommonIncludes.h"
#include <QKeyEvent>

using namespace Minecraft;

MainMenu::MainMenu(CS1972Engine::Game *parent)
    : Screen(parent)
{ }

void MainMenu::tick(float) { }

void MainMenu::draw() {
    graphics().useUiShader();
    graphics().uisOrthoTransform(0.f, parent->width(), parent->height(), 0.f);

    float W = parent->width();
    float H = parent->height();

    float ratio = glm::min(glm::min(W/1280.f, H/960.f), 1.f);

    graphics().shaderUseTexture(true);
    graphics().shaderColor(glm::vec4(1.f));
    if (m_state == 0)
        graphics().shaderBindTexture("howto1");
    else
        graphics().shaderBindTexture("howto2");
    graphics().uisQuad((0.5f*W)-ratio*640.f, (0.5f*W)+ratio*640.f, (0.5f*H)-ratio*480.f, (0.5f*H)+ratio*480.f);

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
        if (m_state == 0)
            m_state = 1;
        else {
            parent->popScreen();
            parent->pushScreen(new GameScreen(this->parent));
            delete this;
        }
    }
}

void MainMenu::keyReleaseEvent(QKeyEvent *) { }

void MainMenu::resizeEvent(int, int) { }
