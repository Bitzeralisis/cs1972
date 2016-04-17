#pragma once

#include "engine/screen.h"

namespace CS1972Engine {

class Sound;
class World;

}

namespace COG {

class PlayerEntity;

class GameScreen : public CS1972Engine::Screen {
public:
    enum {
        DRAW_GEOMETRY,
        DRAW_LIGHTS,
        NUM_DRAWS
    };

private:
    enum {
        LAYER_CONTROLLER,
        LAYER_AMBIENCE,
        LAYER_ENEMIES,
        NUM_LAYERS
    };

public:
    GameScreen(CS1972Engine::Game *parent);
    virtual ~GameScreen();

private:
    const float RETICLE_SIZE_FACTOR = 1.f/8.f;
    const float RETICLE_SIZE_FACTOR_DRAW = 1.f/6.f;
    const float PERFECT_TIMING_WINDOW = 0.06f;
    const int MAX_COMBO = 8;

private:
    CS1972Engine::World *m_world;

    bool m_firstTick = true;
    float m_timestep;
    float m_time = 0.f;

    CS1972Engine::Sound *m_bgm;
    CS1972Engine::Sound *m_sfx1;
    CS1972Engine::Sound *m_sfx2;

    float m_maxYaw = 0.25f*glm::pi<float>();
    float m_maxPitch = 0.25f*glm::pi<float>();

    PlayerEntity *m_player;
    glm::vec2 m_mousePosition;
    float m_prevShot = -1.f;
    float m_shootUntil = -1.f;
    float m_prevBeatChecked = -1.f;
    float m_prevPerfectShot = -1.f;
    int m_combo = 1;

    bool m_mouseHeld[3] = { false, false, false };

private:
    void boundMouse();
    void checkComboUpTo(float beat);
    void goodCombo(float beat);
    void badCombo(float beat);
    void missCombo(float beat);

public:
    virtual void tick(float seconds) override;
    virtual void draw() override;
    void drawScene(float beat);
    void drawHud(float beat);

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

    virtual void resizeEvent(int w, int h) override;
};

}
