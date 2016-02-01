#include "mainmenu.h"
#include "gamescreen.h"
#include "../util/CommonIncludes.h"

using namespace WarmupGame;

MainMenu::MainMenu(CS1972Engine::Game *parent)
    : Screen(parent)
{ }

MainMenu::~MainMenu() { }

void MainMenu::tick() { }
void MainMenu::draw() { }
void MainMenu::mousePressEvent(QMouseEvent *event) { }
void MainMenu::mouseMoveEvent(QMouseEvent *event) { }
void MainMenu::mouseReleaseEvent(QMouseEvent *event) { }
void MainMenu::wheelEvent(QWheelEvent *event) { }
void MainMenu::keyPressEvent(QKeyEvent *event) {
    parent->popScreen();
    parent->pushScreen(new GameScreen(this->parent));
}
void MainMenu::keyReleaseEvent(QKeyEvent *event) { }
void MainMenu::resizeEvent(int w, int h) { }
