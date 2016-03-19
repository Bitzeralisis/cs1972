#pragma once

#include "../engine/screen.h"
#include "../engine/world.h"

namespace CS1972Engine {

class GeometricManager;

}

namespace Platformer {

class PlayerEntity;

class GameScreen : public CS1972Engine::Screen {
public:
    GameScreen(CS1972Engine::Game *parent, const char *map);
    virtual ~GameScreen();

private:
    CS1972Engine::World m_world;
    CS1972Engine::GeometricManager *m_terrain;

    PlayerEntity *m_player;

    bool m_tpcNoclip = true;
    bool m_keysHeld[8] = {false, false, false, false, false, false, false, false};

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
