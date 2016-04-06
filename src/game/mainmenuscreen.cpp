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
    graphics().uis_useShader();
    graphics().uis_orthoTransform(0.f, parent->width(), parent->height(), 0.f);

    graphics().uis_color(glm::vec4(1.f));
    graphics().shaderUseTexture(true);
    graphics().shaderBindTexture("mainmenu");
    graphics().uis_quad(parent->width()/2 - 311.f, parent->width()/2 + 312.f, parent->height()/2 - 201.f, parent->height()/2 + 201.f);

    graphics().shaderUnbindTexture();
    graphics().useShader(0);
}

void MainMenuScreen::mousePressEvent(QMouseEvent *) { }
void MainMenuScreen::mouseMoveEvent(QMouseEvent *) { }
void MainMenuScreen::mouseReleaseEvent(QMouseEvent *) { }
void MainMenuScreen::wheelEvent(QWheelEvent *) { }

void MainMenuScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    //case Qt::Key_1:
    case Qt::Key_Return:
        parent->popScreen();
        parent->pushScreen(new GameScreen(parent, ":/obj/level_easy.obj", ":/obj/level_easy_navmesh.obj"));
        delete this;
        break;
    /*case Qt::Key_2:
        //parent->popScreen();
        //parent->pushScreen(new GameScreen(parent, ":/obj/level_hard.obj"));
        //delete this;
        break;
    case Qt::Key_3:
        parent->popScreen();
        parent->pushScreen(new GameScreen(parent, ":/obj/level_island.obj", ":/obj/level_island_navmesh.obj"));
        delete this;
        break;*/
    default:
        break;
    }
}

void MainMenuScreen::keyReleaseEvent(QKeyEvent *) { }
void MainMenuScreen::resizeEvent(int, int) { }
