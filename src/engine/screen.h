#pragma once

#include "game.h"
#include "../util/HeaderUtils.h"

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

namespace CS1972Engine {

class Graphics;

/**
 * @brief The Screen class
 * Point of access to events for users of this engine.
 */
class Screen {
public:
    explicit Screen(Game *game): parent(game) {}
    virtual ~Screen() {}

protected:
    inline REFERENCE_ACCESSOR_DEEP(Graphics,graphics,parent->g)

public:
    Game *const parent;

    virtual void tick(float seconds) = 0;
    virtual void draw() = 0;

    virtual void mousePressEvent(QMouseEvent *event) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void wheelEvent(QWheelEvent *event) = 0;

    virtual void keyPressEvent(QKeyEvent *event) = 0;
    virtual void keyReleaseEvent(QKeyEvent *event) = 0;

    virtual void resizeEvent(int w, int h) = 0;
};

}
