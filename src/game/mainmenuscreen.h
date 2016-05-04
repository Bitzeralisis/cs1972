#pragma once

#include "engine/screen.h"

namespace CS1972Engine {

class Game;

}

namespace COG {

class MainMenuScreen : public CS1972Engine::Screen {
public:
    MainMenuScreen(CS1972Engine::Game *game);
    virtual ~MainMenuScreen();

public:
    virtual void tick(float seconds) override;
    virtual void draw() override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

    virtual void resizeEvent(int w, int h) override;
};

}
