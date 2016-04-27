#pragma once

#include "engine/screen.h"

namespace CS1972Engine {

class Sound;
class World;

}

namespace COG {

class EnemyEntity;
class PlayerEntity;

class GameScreen : public CS1972Engine::Screen {
public:
    enum {
        DRAW_GEOMETRY,
        DRAW_LIGHTS,
        DRAW_ORTHOGRAPHIC,
        NUM_DRAWS
    };
    enum {
        LAYER_CONTROLLER,
        LAYER_AMBIENCE,
        LAYER_ENEMIES,
        LAYER_ENEMY_PROJECTILES,
        NUM_LAYERS
    };

public:
    GameScreen(CS1972Engine::Game *parent);
    virtual ~GameScreen();

private:
    const float RETICLE_SIZE_FACTOR = 1.f/8.f;
    const float RETICLE_SIZE_FACTOR_DRAW = 1.f/6.f;
    const float RETICLE_SIZE_FACTOR_HIT = 1.f/16.f;
    const float PERFECT_TIMING_WINDOW = 0.090f;
    const float DEFENSE_TIMING_WINDOW = 0.090f;
    const int MAX_COMBO = 8;

    const float GRAPHICS_OFFSET = 0.f;
    const float INPUT_OFFSET = 0.f;

private:
    CS1972Engine::World *m_world;
    glm::vec3 m_particleOffset = glm::vec3(0.f);

    bool m_firstTick = true;
    float m_beatstep;
    float m_beats = 0.f;

    CS1972Engine::Sound *m_bgm;

    float m_maxYaw = 0.25f*glm::pi<float>();
    float m_maxPitch = 0.25f*glm::pi<float>();

    PlayerEntity *m_player;
    glm::vec2 m_mousePosition;
    EnemyEntity *m_nextTarget = 0;
    float m_prevShot = -1.f;
    float m_shootUntil = -1.f;
    float m_prevBeatChecked = -1.f;
    float m_prevPerfectShot = -1.f;
    float m_prevMiss = -1.f;
    int m_combo = 1;
    int m_prevJudge = 0;

    int m_health = 3;
    float m_iframes = 0.f;
    float m_defenseDisable = 0.f;

    bool m_mouseHeld[3] = { false, false, false };
    bool m_keysHeld[4] = { false, false, false, false };

private:
    void boundMouse();
    void startShooting(float beat);
    void keepShooting(float beat);
    void goodCombo(float beat);
    void badCombo(float beat);
    void missCombo(float beat);

    void takeDamage();
    void defend(float beat, int lane);

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
