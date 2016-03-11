#pragma once

#include "../engine/screen.h"

namespace Minecraft {

class MainMenu : public CS1972Engine::Screen {
public:
    explicit MainMenu(CS1972Engine::Game *parent);
    virtual ~MainMenu() { }

private:
    int m_state = 0;

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
