#include "gamescreen.h"
#include "entity/testentity.h"
#include "../engine/voxel/voxelmanager.h"
#include "../engine/voxel/block.h"
#include "pnchunkgenerator.h"
#include <QKeyEvent>
#include <time.h>

using namespace Minecraft;

#define VIEW_DISTANCE (1000.f)
#define FOG_DISTANCE (750.f)

using CS1972Engine::Voxel::BlockType;
const BlockType BLOCK_DEFINITIONS[4] = {
    BlockType("air", true, true, 0.f, 15.f/16.f, 0.f, 15.f/16.f, 0.f, 15.f/16.f),
    BlockType("grass", false, false, 0.f, 15.f/16.f, 3.f/16.f, 15.f/16.f, 2.f/16.f, 15.f/16.f),
    BlockType("stone", false, false, 1.f/16.f, 15.f/16.f, 1.f/16.f, 15.f/16.f, 1.f/16.f, 15.f/16.f),
    BlockType("dirt", false, false, 2.f/16.f, 15.f/16.f, 2.f/16.f, 15.f/16.f, 2.f/16.f, 15.f/16.f)
};

GameScreen::GameScreen(CS1972Engine::Game *parent)
    : Screen(parent)
    , m_world(parent)
{
    CS1972Engine::Voxel::ChunkGenerator *gen = new PNChunkGenerator(time(NULL));
    CS1972Engine::Voxel::VoxelManager *m = new CS1972Engine::Voxel::VoxelManager(BLOCK_DEFINITIONS, gen);
    m_world.useTerrain(m);

    m_player = new TestEntity(glm::vec3(0.f, 66.f, 0.f));
    m_world.addEntity(m_player);
    m_world.tick(0.f);
}

GameScreen::~GameScreen() {
    m_world.deleteEntitiesOnDeconstruct(true);
}

void GameScreen::tick(float seconds) {
    // Move the camera using WASD, RF, Shift
    glm::vec3 walk(0.f);
    if (m_keysHeld[0]) walk.x += 1.f;
    if (m_keysHeld[1]) walk.z -= 1.f;
    if (m_keysHeld[2]) walk.x -= 1.f;
    if (m_keysHeld[3]) walk.z += 1.f;
    bool dashing = m_keysHeld[6];
    bool jumping = m_keysHeld[7];
    m_player->walk(walk, dashing, jumping);

    // Tick world
    m_world.tick(seconds);
}

void GameScreen::draw() {
    // Prepare to draw
    graphics().useDefaultShader();
    graphics().camera->position(m_player->position());
    graphics().camera->position_y(m_player->position().y + 1.5f);
    graphics().camera->near(0.1f);
    graphics().camera->far(VIEW_DISTANCE);
    graphics().shaderPvTransformFromCamera();
    graphics().shaderUseFog(true, FOG_DISTANCE, VIEW_DISTANCE, glm::vec3(0.f, 0.f, 0.f));//glm::vec3(161.f/255.f, 190.f/255.f, 1.f));

    // Draw world
    m_world.draw();

    // Clean up
    graphics().shaderUnbindTexture();
    graphics().useShader(0);
}

void GameScreen::mousePressEvent(QMouseEvent *) { }

void GameScreen::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - parent->width() / 2;
    int dy = event->y() - parent->height() / 2;
    graphics().camera->yaw(graphics().camera->yaw() + (float)(dx)/500.f);
    graphics().camera->pitch(graphics().camera->pitch() - (float)(dy)/500.f);
}

void GameScreen::mouseReleaseEvent(QMouseEvent *) { }

void GameScreen::wheelEvent(QWheelEvent *) { }

void GameScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_F1:
        if (graphics().camera->mode() == CS1972Engine::Camera::Mode::FIRST_PERSON) graphics().camera->mode(CS1972Engine::Camera::Mode::THIRD_PERSON);
        else graphics().camera->mode(CS1972Engine::Camera::Mode::FIRST_PERSON);
        break;
    case Qt::Key_W:
        m_keysHeld[0] = true;
        break;
    case Qt::Key_A:
        m_keysHeld[1] = true;
        break;
    case Qt::Key_S:
        m_keysHeld[2] = true;
        break;
    case Qt::Key_D:
        m_keysHeld[3] = true;
        break;
    case Qt::Key_R:
        m_keysHeld[4] = true;
        break;
    case Qt::Key_F:
        m_keysHeld[5] = true;
        break;
    case Qt::Key_Shift:
        m_keysHeld[6] = true;
        break;
    case Qt::Key_Space:
        m_keysHeld[7] = true;
        break;
    default:
        break;
    }
}

void GameScreen::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        m_keysHeld[0] = false;
        break;
    case Qt::Key_A:
        m_keysHeld[1] = false;
        break;
    case Qt::Key_S:
        m_keysHeld[2] = false;
        break;
    case Qt::Key_D:
        m_keysHeld[3] = false;
        break;
    case Qt::Key_R:
        m_keysHeld[4] = false;
        break;
    case Qt::Key_F:
        m_keysHeld[5] = false;
        break;
    case Qt::Key_Shift:
        m_keysHeld[6] = false;
        break;
    case Qt::Key_Space:
        m_keysHeld[7] = false;
        break;
    default:
        break;
    }
}

void GameScreen::resizeEvent(int, int) { }
