#pragma once

#include "warmupgame.h"
#include "../engine/screen.h"

namespace WarmupGame {

class MainMenu : public CS1972Engine::Screen {
public:
    explicit MainMenu(CS1972Engine::Game *parent);
    virtual ~MainMenu();

public:
    virtual void tick() override;
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
