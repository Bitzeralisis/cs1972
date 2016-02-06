#pragma once

#include "warmupgame.h"
#include "../engine/screen.h"

namespace WarmupGame {

class GameScreen : public CS1972Engine::Screen {
public:
    explicit GameScreen(CS1972Engine::Game *parent);
    virtual ~GameScreen();

private:
    const float HEAD_HEIGHT = 5.f;
    const float RUN_SPEED = 0.25f;
    const float DASH_SPEED = 0.5f;
    const float JUMP_SPEED = 0.33f;
    const float FALL_ACCEL = -0.017f;

    bool m_keysHeld[6] = {false, false, false, false, false, false};
    int m_dash = 0;
    float m_height = 0.f;
    float m_fallSpeed = 0.f;

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
