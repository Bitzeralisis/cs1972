#pragma once

#include "screen.h"
#include "util/CommonIncludes.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <stack>

namespace CS1972Engine {

/**
 * @brief The Game class
 * Base class to be extended by every game. Contains a simple stack of screens,
 * and passes events to the topmost one, except for draw and resizeEvent,
 * which are passed to every screen starting from the bottom-most one.
 */
class Game {
public:
    virtual ~Game() {}

private:
    std::deque<Screen*> screenStack;

public: // Screen stack management
    void pushScreen(Screen *s);
    Screen* popScreen();

public: // Events
    virtual void initializeGL() = 0;

    void tick();
    void draw();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void resizeEvent(int w, int h);
};

}
