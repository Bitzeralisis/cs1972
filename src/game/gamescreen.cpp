#include "gamescreen.h"
#include "mainmenuscreen.h"
#include "entity/enemybossentity.h"
#include "entity/enemyentity.h"
#include "entity/playerentity.h"
#include "entity/playershotentity.h"
#include "../engine/primitive.h"
#include "../engine/geometric/geometricmanager.h"
#include "../util/obj.h"
#include "../csm/csm_collide.h"
#include <QMouseEvent>
#include <glm/gtx/rotate_vector.hpp>

using namespace Platformer;

GameScreen::GameScreen(CS1972Engine::Game *parent, const char *map, const char *navmesh)
    : Screen(parent)
    , m_world(parent)
{
    m_terrain = new CS1972Engine::GeometricManager(&m_world, map, navmesh);
    m_world.useTerrain(m_terrain);

    m_player = new PlayerEntity(glm::vec3(0.f, 1.f, 0.f));
    m_boss = new EnemyBossEntity(m_player);
    m_world.addEntity(m_player);
    m_world.addEntity(m_boss);
    m_world.tick(0.f);

    graphics().dr_init(parent->width(), parent->height());
    graphics().camera->position(m_player->position());
}

GameScreen::~GameScreen() {
    delete m_boss;

    graphics().dr_cleanup();

    m_world.deleteEntitiesOnDeconstruct(true);
}

void GameScreen::tick(float seconds) {
    // Check win/loss conditions
    if (m_player->health() <= 0.f)
        m_gameOver = 2;
    if (m_boss->health() <= 0.f)
        m_gameOver = 1;

    if (m_gameOver == 2)
        graphics().camera->pitch(graphics().camera->pitch()+2.f*seconds);

    if (!m_gameOver) {
        glm::vec3 walk(0.f);
        if (m_keysHeld[0]) walk.x += 1.f;
        if (m_keysHeld[1]) walk.z -= 1.f;
        if (m_keysHeld[2]) walk.x -= 1.f;
        if (m_keysHeld[3]) walk.z += 1.f;
        walk = glm::rotate(walk, graphics().camera->yaw(), glm::vec3(0.f, -1.f, 0.f));
        m_player->walk(seconds, walk, m_keysHeld[6], m_keysHeld[7]);

        if (m_mouseHeld[0])
            m_player->shoot(graphics().camera->lookVector());
    } else
        m_player->walk(seconds, glm::vec3(0.f), false, false);

    m_world.tick(seconds);
}

int m_renderMode = 2;

void GameScreen::draw() {
    if (m_renderMode == 2) {
        graphics().dr_bindGbuffer();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        graphics().dr_useGbufferShader();
    }

    if (m_renderMode == 1)
        graphics().useDefaultShader();
    graphics().camera->fovy((0.75f+0.125f*m_player->dashEffect()/5.f)*glm::half_pi<float>());
    graphics().camera->near(0.1f);
    graphics().camera->far(200.f);
    graphics().camera->position(m_player->position() + glm::vec3(0.f, 1.5f, 0.f));
    // Cheater way to make camera not clip through walls
    glm::vec3 intersect, normal;
    if (m_tpcNoclip && m_terrain->rayCast(graphics().camera->position(), -1.f*graphics().camera->lookVector(), 11.f, intersect, normal) >= 0.f)
        graphics().camera->tpdistance(glm::distance(graphics().camera->position(), intersect) - 1.f);
    else
        graphics().camera->tpdistance(10.f);
    graphics().shaderPvTransformFromCamera();
    if (m_renderMode == 1) {
        graphics().shaderUseLight(true, 1, glm::vec3(0.5f, 1.f, 1.f));
        graphics().shaderUseFog(true, 150.f, 200.f, glm::vec3(0.f));
    }
    graphics().shaderColor(glm::vec4(1.f));

    m_player->yaw(graphics().camera->yaw());

    // Draw pass 0 (lit geometry)
    m_world.draw(0);

    graphics().shaderUnbindTexture();
    graphics().useShader(0);

    if (m_renderMode == 2)
        graphics().dr_unbindGbuffer();

    if (m_renderMode == 2) {
        graphics().dr_useDeferredShader();
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDisable(GL_DEPTH_TEST);
        graphics().shaderPvTransformFromCamera();
        graphics().uis_orthoTransform(0.f, 1.f, 1.f, 0.f);
        graphics().shaderMTransform(glm::mat4(1.f));

        // Draw pass 1 (lighting pass)
        m_world.draw(1);

        graphics().dr_lightDirectional(glm::vec3(0.5f, 1.f, 1.f), glm::vec3(0.5f));
        graphics().dr_drawLight();
        graphics().dr_lightAmbient(glm::vec3(0.25f));
        graphics().dr_drawLight();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        graphics().useShader(0);
    }

    graphics().useDefaultShader();
    graphics().shaderPvTransformFromCamera();

    // When using deferred shading, have to copy depth buffer from gbuffer to backbuffer
    if (m_renderMode == 2)
        graphics().dr_blitGbufferDepthToBb(parent->width(), parent->height());

    // Draw pass 2 (non-lit geometry)
    graphics().shaderUseLight(false);
    m_world.draw(2);

    // Draw the pathfinding line
    if (m_player->doPathfind()) {
        m_terrain->draw(3);

        CS1972Engine::GeometricManager::NavPath *p = m_terrain->nav_getPathFrom(m_player->position()+glm::vec3(0.f, 1.4f, 0.f), m_player->pfPosition()+glm::vec3(0.f, 1.5f, 0.f));
        if (p) {
            glDisable(GL_DEPTH_TEST);
            int length = 0;
            for (CS1972Engine::GeometricManager::NavPath *p2 = p; p2; p2 = p2->next)
                ++length;
            GLfloat *v = new GLfloat[8*length];
            for (int i = 0; p; ++i) {
                v[8*i  ] = p->p.x;
                v[8*i+1] = p->p.y;
                v[8*i+2] = p->p.z;
                p = p->next;
            }
            glLineWidth(8.f);
            graphics().shaderUseLight(false);
            graphics().shaderUseFog(false);
            graphics().shaderColor(glm::vec4(1.f, 0.f, 1.f, 1.f));
            graphics().shaderUseTexture(false);
            graphics().shaderMTransform(glm::mat4(1.f));
            CS1972Engine::Primitive(length, 8*length*sizeof(GLfloat), v).drawArray(GL_LINE_STRIP, 0, length);
            delete v;
            delete p;
            glEnable(GL_DEPTH_TEST);
        }
    }

    graphics().useShader(0);

    // Draw UI
    {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float W = parent->width();
        float H = parent->height();
        float hW = 0.5f*W;
        float hH = 0.5f*H;
        graphics().uis_useShader();
        graphics().uis_orthoTransform(0.f, parent->width(), parent->height(), 0.f);

        // Draw pass 4 - orthographic components
        m_world.draw(4);

        // Draw health
        graphics().shaderUseTexture(false);
        graphics().shaderColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
        for (int i = m_player->health(); i > 0; --i) {
            graphics().uis_quad(40.f*i-20.f, 40.f*i+10.f, H-50.f, H-20.f);
        }

        // Draw recticle
        glLineWidth(1.f);
        graphics().shaderUseTexture(false);
        graphics().shaderColor(glm::vec4(1.f));
        glm::mat4 m(1.f);
        m = glm::translate(m, glm::vec3(hW-5.f, hH-5.f, 0.f));
        m = glm::scale(m, glm::vec3(10.f));
        graphics().shaderMTransform(m);
        graphics().getPrimitive("uis_triangle")->drawArray(GL_LINE_LOOP, 0, 3);

        // Win/loss graphics
        if (m_gameOver == 1) {
            graphics().uis_color(glm::vec4(1.f));
            graphics().shaderUseTexture(true);
            graphics().shaderBindTexture("win");
            graphics().uis_quad(hW-400.f, hW+400.f, hH-100.f, hH+100.f);
        } else if (m_gameOver == 2) {
            graphics().shaderUseTexture(false);
            graphics().uis_color(glm::vec4(0.5f, 0.f, 0.f, 0.5f));
            graphics().uis_quad(0.f, W, 0.f, H);

            graphics().uis_color(glm::vec4(1.f));
            graphics().shaderUseTexture(true);
            graphics().shaderBindTexture("lose");
            graphics().uis_quad(hW-400.f, hW+400.f, hH-100.f, hH+100.f);
        }

        graphics().useShader(0);
    }
}

void GameScreen::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        m_mouseHeld[0] = true;
}

void GameScreen::mouseMoveEvent(QMouseEvent *event) {
    if (m_gameOver)
        return;

    int dx = event->x() - parent->width() / 2;
    int dy = event->y() - parent->height() / 2;
    graphics().camera->yaw(graphics().camera->yaw() + (float)(dx)/500.f);
    graphics().camera->pitch(graphics().camera->pitch() - (float)(dy)/500.f);
}

void GameScreen::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        m_mouseHeld[0] = false;
}

void GameScreen::wheelEvent(QWheelEvent *) { }

void GameScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_F1:
        if (graphics().camera->mode() == CS1972Engine::Camera::Mode::FIRST_PERSON) graphics().camera->mode(CS1972Engine::Camera::Mode::THIRD_PERSON);
        else graphics().camera->mode(CS1972Engine::Camera::Mode::FIRST_PERSON);
        break;

    case Qt::Key_F2:
        m_renderMode = m_renderMode == 1 ? 2 : 1;
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

        // Toggle pathfinding
        m_player->doPathfind(!m_player->doPathfind());
        m_terrain->nav_render(!m_terrain->nav_render());

        break;

    case Qt::Key_F:
        m_keysHeld[5] = true;

        // Activate pathfinding and create target point based on raycast
        {
            float best = std::numeric_limits<float>::infinity();

            // Raycast the current target point if there is one
            if (m_player->doPathfind()) {
                glm::vec3 i, n;
                float t = csm::raycast_ellipsoid(csm::ray(graphics().camera->position(), graphics().camera->lookVector()), m_player->getEllipsoid()+m_player->pfPosition(), i, n);
                if (t >= 0.f && t < best) {
                    best = t;
                    m_player->pfPosition(i+n+glm::vec3(0.f, -1.f, 0.f));
                }
            }

            // Also try raycasting onto terrain
            glm::vec3 i, n;
            float t = m_terrain->rayCast(graphics().camera->position(), graphics().camera->lookVector(), 200.f, i, n);
            if (t >= 0.f && t < best)
                m_player->pfPosition(i+n+glm::vec3(0.f, -1.f, 0.f));

            m_player->doPathfind(true);
            m_terrain->nav_render(true);
        }

        break;

    case Qt::Key_Shift:
        m_keysHeld[6] = true;
        break;
    case Qt::Key_Space:
        m_keysHeld[7] = true;
        break;

    case Qt::Key_Return:
        if (m_gameOver) {
            parent->popScreen();
            parent->pushScreen(new MainMenuScreen(parent));
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

void GameScreen::resizeEvent(int w, int h) {
    graphics().dr_cleanup();
    graphics().dr_init(w, h);
}
