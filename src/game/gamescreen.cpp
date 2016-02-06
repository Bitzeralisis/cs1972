#include "gamescreen.h"
#include "../engine/game.h"
#include <QMouseEvent>

using namespace WarmupGame;

GameScreen::GameScreen(CS1972Engine::Game *parent)
    : Screen(parent)
{ }

GameScreen::~GameScreen() { }

void GameScreen::tick() {
    // Walk using ASDF
    glm::vec3 walk(0.f);
    if (m_keysHeld[0]) walk.x += 1.f;
    if (m_keysHeld[1]) walk.z -= 1.f;
    if (m_keysHeld[2]) walk.x -= 1.f;
    if (m_keysHeld[3]) walk.z += 1.f;
    if (walk != glm::vec3(0.f)) {
        walk = glm::normalize(walk);
        if (m_keysHeld[5]) {
            walk = DASH_SPEED * glm::normalize(walk);
            if (m_dash < 5) ++m_dash;
        } else {
            walk = RUN_SPEED * glm::normalize(walk);
            if (m_dash > 0) --m_dash;
        }
        parent->g.camera->walk(walk);
    } else {
        if (m_dash > 0) --m_dash;
    }

    // Jump on ground if space held
    if (m_height <= 0.f && m_keysHeld[4]) {
        m_fallSpeed = JUMP_SPEED;
    }

    // Fall down
    m_height += m_fallSpeed;
    m_fallSpeed += FALL_ACCEL;
    if (m_height < 0.f) {
        m_height = 0.f;
        m_fallSpeed = 0.f;
    }
    parent->g.camera->position.y = m_height + HEAD_HEIGHT;
}

void GameScreen::draw() {
    parent->g.camera->fovy = ((m_dash/5.f)*0.25f + 0.75f) * glm::half_pi<float>();

    parent->g.useDefaultShader();
    parent->g.shaderPvTransformFromCamera();
    parent->g.shaderColor(glm::vec3(1.f, 1.f, 1.f));
    parent->g.shaderUseTexture(true);
    parent->g.shaderBindTexture(parent->g.getTexture("grass"));

    // Tile the floor as far as the eye can see (literally)
    float minx = parent->g.camera->position.x - parent->g.camera->far;
    float maxx = parent->g.camera->position.x + parent->g.camera->far;
    float miny = parent->g.camera->position.z - parent->g.camera->far;
    float maxy = parent->g.camera->position.z + parent->g.camera->far;
    for (int i = (int)(minx/30.f)-2; i <= (int)(maxx/30.f)+3; ++i) {
        for (int j = (int)(miny/30.f)-2; j <= (int)(maxy/30.f)+3; ++j) {
            glm::mat4 m(1.f);
            m = glm::translate(m, glm::vec3(30.f*i, 0.f, 30.f*j));
            m = glm::scale(m, glm::vec3(30.f, 30.f, 30.f));
            parent->g.shaderMTransform(m);
            parent->g.drawQuad();
        }
    }

    parent->g.shaderUnbindTexture();
    parent->g.useShader(0);
}

void GameScreen::mousePressEvent(QMouseEvent *event) { }
void GameScreen::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - parent->width() / 2;
    int dy = event->y() - parent->height() / 2;
    parent->g.camera->yaw += (float)(dx)/500.f;
    parent->g.camera->pitch -= (float)(dy)/500.f;
}
void GameScreen::mouseReleaseEvent(QMouseEvent *event) { }
void GameScreen::wheelEvent(QWheelEvent *event) { }
void GameScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
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
