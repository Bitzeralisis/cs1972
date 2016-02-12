#include "game.h"
#include "screen.h"

using namespace CS1972Engine;

void Game::pushScreen(Screen *s) {
    m_screenStack.push_back(s);
}

Screen* Game::popScreen() {
    if (m_screenStack.size()) {
        Screen *retval = this->m_screenStack.back();
        m_screenStack.pop_back();
        return retval;
    }
    return 0;
}

void Game::initializeGL() {
    g.initializeGL();
    initGL();
}

void Game::tick(float seconds) {
    if (m_screenStack.size()) m_screenStack.back()->tick(seconds);
}

void Game::draw() {
    for (std::deque<Screen*>::iterator it = m_screenStack.begin(); it != m_screenStack.end(); ++it) {
        (*it)->draw();
    }
}

void Game::mousePressEvent(QMouseEvent *event) {
    if (m_screenStack.size()) m_screenStack.back()->mousePressEvent(event);
}

void Game::mouseMoveEvent(QMouseEvent *event) {
    if (m_screenStack.size()) m_screenStack.back()->mouseMoveEvent(event);
}

void Game::mouseReleaseEvent(QMouseEvent *event) {
    if (m_screenStack.size()) m_screenStack.back()->mouseReleaseEvent(event);
}

void Game::wheelEvent(QWheelEvent *event) {
    if (m_screenStack.size()) m_screenStack.back()->wheelEvent(event);
}

void Game::keyPressEvent(QKeyEvent *event) {
    if (m_screenStack.size()) m_screenStack.back()->keyPressEvent(event);
}

void Game::keyReleaseEvent(QKeyEvent *event) {
    if (m_screenStack.size()) m_screenStack.back()->keyReleaseEvent(event);
}

void Game::resizeEvent(int w, int h) {
    m_width = w;
    m_height = h;
    g.camera->aspect((float)w / (float)h);
    for (std::deque<Screen*>::iterator it = m_screenStack.begin(); it != m_screenStack.end(); ++it) {
        (*it)->resizeEvent(w, h);
    }
}
