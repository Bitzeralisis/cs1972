#pragma once

#include "../engine/screen.h"
#include "../engine/world.h"

namespace Minecraft {

class GameScreen : public CS1972Engine::Screen {
public:
    explicit GameScreen(CS1972Engine::Game *parent);
    virtual ~GameScreen();

private:
    CS1972Engine::World m_world;

    bool m_keysHeld[7] = {false, false, false, false, false, false, false};

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
