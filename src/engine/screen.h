#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

namespace CS1972Engine {

/**
 * @brief The Screen class
 * Point of access to events for users of this engine.
 */
class Screen {
public:
    virtual ~Screen() {}

public:
    virtual void tick() = 0;
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
