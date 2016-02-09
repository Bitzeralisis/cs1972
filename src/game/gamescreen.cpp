#include "gamescreen.h"
#include "mainmenu.h"
#include "entity/bossentity.h"
#include "entity/obstacleentity.h"
#include "entity/playerentity.h"
#include <QMouseEvent>
#include <QPoint>

using namespace WarmupGame;

GameScreen::GameScreen(CS1972Engine::Game *parent)
    : Screen(parent)
    , m_world(parent)
{
    // Put player
    m_player = new PlayerEntity(glm::vec3(-50.f, 0.f, 0.f));
    m_world.addEntity(m_player);

    // Put boss
    m_boss = new BossEntity(720.f, 15.f, 70.f);
    m_world.addEntity(m_boss);

    // Put a bunch of platform cylinder things
    for (float y = 0.f; y < 60.f; y += 10.f) {
        for (int i = 0; i < 3; ++i) {
            float ang = 2.f * glm::pi<float>() * (float)rand() / RAND_MAX;
            float dist = 35.f * (float)rand() / RAND_MAX;
            float x = dist * glm::cos(ang);
            float z = dist * glm::sin(ang);
            float size = 4.f * (float)rand() / RAND_MAX + 8.f;
            ObstacleEntity *obs = new ObstacleEntity(glm::vec3(x, y, z), glm::vec2(size, 10.f));
            m_world.addEntity(obs);
        }
    }

    // Tick once to actually add entities
    m_world.tick();
}

GameScreen::~GameScreen() {
    // Tell world to free all the entities for us
    m_world.deleteEntitiesOnDeconstruct(true);
}

void GameScreen::tick() {
    if (m_gameOver) return;

    // Move the player using ASDF, Shift, Space
    glm::vec3 walk(0.f);
    if (m_keysHeld[0]) walk.x += 1.f;
    if (m_keysHeld[1]) walk.z -= 1.f;
    if (m_keysHeld[2]) walk.x -= 1.f;
    if (m_keysHeld[3]) walk.z += 1.f;
    bool jumping = m_keysHeld[4];
    bool dashing = m_keysHeld[5];
    m_player->move(walk, jumping, dashing);

    // Tick world
    m_world.tick();

    // Check for game over
    if (m_boss->dead())
        m_gameOver = 1;
    if (m_player->dead())
        m_gameOver = 2;
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

    // Draw UI
    glDisable(GL_DEPTH_TEST);
    graphics().useUiShader();
    graphics().uisOrthoTransform(0.f, parent->width(), parent->height(), 0.f);

    // Draw dash bar
    graphics().shaderUseTexture(false);
    graphics().shaderColor(glm::vec3(0.f, 1.f, 0.f));
    float dashHeight = (parent->height()-40.f) * ((float) m_player->dashBar() / m_player->MAX_DASHBAR);
    graphics().uisQuad(20.f, 40.f, parent->height()-20.f-dashHeight, parent->height()-20.f);

    // Draw HP bar
    graphics().shaderUseTexture(false);
    graphics().shaderColor(glm::vec3(1.f, 0.f, 0.f));
    float hpHeight = (parent->height()-40.f) * ((float) m_player->health() / m_player->MAX_HEALTH);
    graphics().uisQuad(parent->width()-40.f, parent->width()-20.f, parent->height()-20.f-hpHeight, parent->height()-20.f);

    // Draw win/lose graphic
    if (m_gameOver) {
        graphics().shaderUseTexture(true);
        graphics().shaderColor(glm::vec3(1.f, 1.f, 1.f));
        if (m_gameOver == 1)
            graphics().shaderBindTexture("win");
        else
            graphics().shaderBindTexture("lose");
        graphics().uisQuad(parent->width()*0.5f-160.f, parent->width()*0.5f+160.f, parent->height()-180.f, parent->height()-20.f);
    }

    // Clean up
    glEnable(GL_DEPTH_TEST);
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

void GameScreen::wheelEvent(QWheelEvent *event) {
    float zoom = -0.01f * event->delta();
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
    case Qt::Key_K:
        m_player->suicide();
        m_gameOver = 2;
        break;
    case Qt::Key_Return:
        if (m_gameOver) {
            parent->popScreen();
            parent->pushScreen(new MainMenu(parent));
            delete this;
        }
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

void GameScreen::resizeEvent(int, int) { }
