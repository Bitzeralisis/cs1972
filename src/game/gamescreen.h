#pragma once

#include "../engine/screen.h"
#include "../engine/world.h"

namespace CS1972Engine {
namespace Voxel {

class VoxelManager;

}
}

namespace Minecraft {

class EnemyEntity;
class PlayerEntity;
class PNChunkGenerator;

class GameScreen : public CS1972Engine::Screen {
public:
    explicit GameScreen(CS1972Engine::Game *parent);
    virtual ~GameScreen();

private:
    // Graphics
    CS1972Engine::Primitive *m_triangle;
    CS1972Engine::Primitive *m_circleLoop;
    CS1972Engine::Primitive *m_ringStrip;

    // World-related things
    CS1972Engine::World m_world;
    PNChunkGenerator *m_gen;
    CS1972Engine::Voxel::VoxelManager *m_vox;

    // Entities
    PlayerEntity *m_player;
    std::list<EnemyEntity *> m_enemies;

    // Chaos things
    const float VIEW_DISTANCE = 150.f;
    const float CHAOS_VIEW_DISTANCE = 16.f;
    const float FOG_DISTANCE = 125.f;
    const float CHAOS_FOG_DISTANCE = 2.f;
    const glm::vec3 SKY_COLOR = glm::vec3(161.f/255.f, 190.f/255.f, 1.f);
    const glm::vec3 CHAOS_SKY_COLOR = glm::vec3(1.f, 0.f, 1.f);

    const float MAX_CHAOS_TIMER = 120.f;
    const float CHAOS_TIME_GAIN[5] = { 0.f, 0.5f, 1.f, 2.f, 3.f };
    const float CHAOS_FADE_TIME = 1.f;
    const float SPAWN_INIT_AMOUNT[5] = { 0.f, 5.f, 10.f, 15.f, 20.f };
    const float SPAWN_TIMER[5] = { 10.f, 3.f, 2.f, 1.f, 0.5f };
    const float KILLS_REQUIRED[5] = { 5.f, 5.f, 15.f, 30.f, 40.f };
    const float SPAWN_DIST = 30.f;
    const float DESPAWN_DIST = 35.f;

    float m_viewDistance;
    float m_fogDistance;
    glm::vec3 m_skyColor;

    bool m_chaosTime = false;
    float m_chaosFading = 0.f;
    float m_chaosTimer = 0.f;
    float m_spawnTimer = 0.f;

    // Controls-related stuff
    bool m_hasTargetBlock;
    glm::vec3 m_targetBlock;
    glm::vec3 m_targetFace;

    bool m_keysHeld[8] = {false, false, false, false, false, false, false, false};

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
