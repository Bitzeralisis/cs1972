#include "gamescreen.h"
#include "mainmenuscreen.h"
#include "entity/playerentity.h"
#include "../util/obj.h"
#include "../engine/geometric/geometricmanager.h"
#include <QMouseEvent>
#include <glm/gtx/rotate_vector.hpp>

using namespace Platformer;

GameScreen::GameScreen(CS1972Engine::Game *parent, const char *map)
    : Screen(parent)
    , m_world(parent)
{
    m_terrain = new CS1972Engine::GeometricManager(&m_world, map);
    m_world.useTerrain(m_terrain);

    m_player = new PlayerEntity(glm::vec3(0.f, 1.f, 0.f));
    m_world.addEntity(m_player);
    m_world.tick(0.f);

    graphics().camera->position(m_player->position());
}

GameScreen::~GameScreen() {
    m_world.deleteEntitiesOnDeconstruct(true);
}

void GameScreen::tick(float seconds) {
    m_world.tick(seconds);

    glm::vec3 walk(0.f);
    if (m_keysHeld[0]) walk.x += 1.f;
    if (m_keysHeld[1]) walk.z -= 1.f;
    if (m_keysHeld[2]) walk.x -= 1.f;
    if (m_keysHeld[3]) walk.z += 1.f;
    walk = glm::rotate(walk, graphics().camera->yaw(), glm::vec3(0.f, -1.f, 0.f));
    m_player->walk(seconds, walk, m_keysHeld[6], m_keysHeld[7]);
}

void GameScreen::draw() {
    graphics().useDefaultShader();
    graphics().camera->fovy((0.75f+0.125f*m_player->dashEffect()/5.f)*glm::half_pi<float>());
    graphics().camera->near(0.1f);
    graphics().camera->far(200.f);
    graphics().camera->position(m_player->position() + glm::vec3(0.f, 1.5f, 0.f));
    // Cheater way to make camera not clip through walls
    glm::vec3 intersect, normal;
    if (m_tpcNoclip && m_terrain->rayCast(graphics().camera->position(), -1.f*graphics().camera->lookVector(), 11.f, intersect, normal))
        graphics().camera->tpdistance(glm::distance(graphics().camera->position(), intersect) - 1.f);
    else
        graphics().camera->tpdistance(10.f);
    graphics().shaderPvTransformFromCamera();
    graphics().shaderUseLight(true, 2, m_player->position() + glm::rotate(glm::vec3(0.75f, 1.f, 0.5f), m_player->yaw(), glm::vec3(0.f, -1.f, 0.f)));
    graphics().shaderColor(glm::vec4(1.f));
    graphics().shaderUseFog(true, 150.f, 200.f, glm::vec3(0.f));

    m_player->yaw(graphics().camera->yaw());

    m_world.draw();

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

    case Qt::Key_1:
        m_terrain->slideHack(!m_terrain->slideHack());
        break;
    case Qt::Key_2:
        m_player->rampBounceHack(!m_player->rampBounceHack());
        break;
    case Qt::Key_3:
        m_terrain->deflectionHack(!m_terrain->deflectionHack());
        break;
    case Qt::Key_9:
        m_tpcNoclip = !m_tpcNoclip;
        break;
    case Qt::Key_0:
        m_player->position(glm::vec3(0.f, 1.f, 0.f));
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

    case Qt::Key_Return:
        parent->popScreen();
        parent->pushScreen(new MainMenuScreen(parent));
        delete this;
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
