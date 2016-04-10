#pragma once

#include "../engine/screen.h"

namespace COG {

class InitScreen : public CS1972Engine::Screen {
public:
    InitScreen(CS1972Engine::Game *parent);
    virtual ~InitScreen();

private:
    float m_timestep;
    float m_time = 0.f;

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
