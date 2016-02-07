#pragma once

#include "../engine/screen.h"
#include "../engine/world.h"

namespace WarmupGame {

class PlayerEntity;

class GameScreen : public CS1972Engine::Screen {
public:
    explicit GameScreen(CS1972Engine::Game *parent);
    virtual ~GameScreen();

private:
    CS1972Engine::World m_world;
    PlayerEntity *m_player;

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
