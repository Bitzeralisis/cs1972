#pragma once

#include "graphics.h"
#include "audio.h"
#include <deque>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

namespace CS1972Engine {

class Screen;

/**
 * @brief The Game class
 * Base class to be extended by every game. Contains a simple stack of screens,
 * and passes events to the topmost one, except for draw and resizeEvent,
 * which are passed to every screen starting from the bottom-most one.
 */
class Game {
public:
    virtual ~Game();

private:
    int m_width;
    int m_height;
    std::deque<Screen*> m_screenStack;

public:
    Graphics g;
    Audio a;

public:
    int width() { return m_width; }
    int height() { return m_height; }

    // Screen stack management
    void pushScreen(Screen *a);
    Screen* popScreen();

public:
    void initializeGL();
    virtual void initGL() = 0;

    void tick(float seconds);
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
