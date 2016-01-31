#include "game.h"

using namespace CS1972Engine;

void Game::pushScreen(Screen *s) {
    screenStack.push_back(s);
}

Screen* Game::popScreen() {
    if (screenStack.size()) {
        Screen *retval = this->screenStack.back();
        screenStack.pop_back();
        return retval;
    }
    return 0;
}

void Game::tick() {
    if (screenStack.size()) screenStack.back()->tick();
}

void Game::draw() {
    for (std::deque<Screen*>::iterator it = screenStack.begin(); it != screenStack.end(); ++it) {
        (*it)->draw();
    }
}

void Game::mousePressEvent(QMouseEvent *event) {
    if (screenStack.size()) screenStack.back()->mousePressEvent(event);
}

void Game::mouseMoveEvent(QMouseEvent *event) {
    if (screenStack.size()) screenStack.back()->mouseMoveEvent(event);
}

void Game::mouseReleaseEvent(QMouseEvent *event) {
    if (screenStack.size()) screenStack.back()->mouseReleaseEvent(event);
}

void Game::wheelEvent(QWheelEvent *event) {
    if (screenStack.size()) screenStack.back()->wheelEvent(event);
}

void Game::keyPressEvent(QKeyEvent *event) {
    if (screenStack.size()) screenStack.back()->keyPressEvent(event);
}

void Game::keyReleaseEvent(QKeyEvent *event) {
    if (screenStack.size()) screenStack.back()->keyReleaseEvent(event);
}

void Game::resizeEvent(int w, int h) {
    for (std::deque<Screen*>::iterator it = screenStack.begin(); it != screenStack.end(); ++it) {
        (*it)->resizeEvent(w, h);
    }
}
