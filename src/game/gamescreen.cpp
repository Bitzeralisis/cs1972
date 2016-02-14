#include "gamescreen.h"
#include "../engine/voxel/voxelmanager.h"
#include "randomchunkgenerator.h"
#include <QKeyEvent>

using namespace Minecraft;

GameScreen::GameScreen(CS1972Engine::Game *parent)
    : Screen(parent)
    , m_world(parent)
{
    RandomChunkGenerator *gen = new RandomChunkGenerator();
    CS1972Engine::Voxel::VoxelManager *m = new CS1972Engine::Voxel::VoxelManager(gen);
    m_world.useTerrain(m);
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
    if (m_keysHeld[4]) walk.y += 1.f;
    if (m_keysHeld[5]) walk.y -= 1.f;
    bool dashing = m_keysHeld[6];
    walk *= 20.f * seconds * (dashing?2.f:1.f);

    graphics().camera->walk(walk);
    graphics().camera->position_y(graphics().camera->position().y+walk.y);

    // Tick world
    m_world.tick(seconds);
}

void GameScreen::draw() {
    // Prepare to draw
    graphics().useDefaultShader();
    graphics().shaderPvTransformFromCamera();
    graphics().shaderColor(glm::vec3(1.f, 1.f, 1.f));

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
    default:
        break;
    }
}

void GameScreen::resizeEvent(int, int) { }
