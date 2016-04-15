#pragma once

#include "engine/screen.h"

namespace CS1972Engine {

class Sound;
class World;

}

namespace COG {

class InitScreen : public CS1972Engine::Screen {
public:
    InitScreen(CS1972Engine::Game *parent);
    virtual ~InitScreen();

private:
    CS1972Engine::World *m_world;

    bool m_firstTick = true;
    float m_timestep;
    float m_time = 0.f;

    float m_prevHat = -1.f;

    CS1972Engine::Sound *m_bgm;
    CS1972Engine::Sound *m_sfx1;
    CS1972Engine::Sound *m_sfx2;

    bool m_mouseHeld[3] = { false, false, false };

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
