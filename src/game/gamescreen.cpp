#include "gamescreen.h"
#include "entity/obstacleentity.h"
#include "entity/playerentity.h"
#include <QMouseEvent>
#include <QPoint>

using namespace WarmupGame;

GameScreen::GameScreen(CS1972Engine::Game *parent)
    : Screen(parent)
    , m_world(parent)
{
    m_player = new PlayerEntity();
    m_world.addEntity(m_player);
    m_world.addEntity(new ObstacleEntity());
    m_world.addEntity(new ObstacleEntity());
    m_world.addEntity(new ObstacleEntity());
    m_world.addEntity(new ObstacleEntity());
    m_world.addEntity(new ObstacleEntity());
    m_world.addEntity(new ObstacleEntity());

    // Tick once to actually add entities
    m_world.tick();
}

GameScreen::~GameScreen() {
    delete m_player;
}

void GameScreen::tick() {
    // Move the player using ASDF, Shift, Space
    glm::vec3 walk(0.f);
    if (m_keysHeld[0]) walk.x += 1.f;
    if (m_keysHeld[1]) walk.z -= 1.f;
    if (m_keysHeld[2]) walk.x -= 1.f;
    if (m_keysHeld[3]) walk.z += 1.f;
    bool jumping = m_keysHeld[4];
    bool dashing = m_keysHeld[5];
    m_player->move(walk, jumping, dashing);

    // Add a random cylinder!! FUN TIMES
    //m_world.addEntity(new ObstacleEntity());

    // Tick world
    m_world.tick();
}

void GameScreen::draw() {
    // Put camera where player is
    glm::vec3 pos = m_player->position();
    pos.y += m_player->HEAD_HEIGHT;
    graphics().camera->position(pos);

    // Daaaaaashing effect
    graphics().camera->fovy(((m_player->dash()/5.f)*0.25f + 0.75f) * glm::half_pi<float>());

    // Prepare to draw
    graphics().useDefaultShader();
    graphics().shaderPvTransformFromCamera();
    graphics().shaderColor(glm::vec3(1.f, 1.f, 1.f));
    graphics().shaderUseTexture(true);
    graphics().shaderBindTexture(graphics().getTexture("grass"));

    // Tile the floor really far idk
    float minx = graphics().camera->position().x - graphics().camera->far();
    float maxx = graphics().camera->position().x + graphics().camera->far();
    float miny = graphics().camera->position().z - graphics().camera->far();
    float maxy = graphics().camera->position().z + graphics().camera->far();
    for (int i = (int)(minx/30.f)-2; i <= (int)(maxx/30.f)+3; ++i) {
        for (int j = (int)(miny/30.f)-2; j <= (int)(maxy/30.f)+3; ++j) {
            glm::mat4 m(1.f);
            m = glm::translate(m, glm::vec3(30.f*i, 0.f, 30.f*j));
            m = glm::scale(m, glm::vec3(30.f, 30.f, 30.f));
            graphics().shaderMTransform(m);
            graphics().drawQuad();
        }
    }

    // Draw world
    m_world.draw();

    // Clean up
    graphics().shaderUnbindTexture();
    graphics().useShader(0);
}

void GameScreen::mousePressEvent(QMouseEvent *event) { }
void GameScreen::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - parent->width() / 2;
    int dy = event->y() - parent->height() / 2;
    graphics().camera->yaw(graphics().camera->yaw() + (float)(dx)/500.f);
    graphics().camera->pitch(graphics().camera->pitch() - (float)(dy)/500.f);
}
void GameScreen::mouseReleaseEvent(QMouseEvent *event) { }
void GameScreen::wheelEvent(QWheelEvent *event) {
    float zoom = -0.01f * event->angleDelta().y();
    float dist = graphics().camera->tpdistance();
    dist += zoom;
    dist = glm::max(5.f, glm::min(dist, 30.f));
    graphics().camera->tpdistance(dist);
}
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
    case Qt::Key_Space:
        m_keysHeld[4] = true;
        break;
    case Qt::Key_Shift:
        m_keysHeld[5] = true;
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
    case Qt::Key_Space:
        m_keysHeld[4] = false;
        break;
    case Qt::Key_Shift:
        m_keysHeld[5] = false;
        break;
    default:
        break;
    }
}
void GameScreen::resizeEvent(int w, int h) { }
