#include "gamescreen.h"
#include "game/coggame.h"
#include "entity/ambiententity.h"
#include "entity/enemyshotentity.h"
#include "entity/playerentity.h"
#include "entity/playershotentity.h"
#include "entity/enemy/cubeaenemy.h"
#include "entity/enemy/diamondaenemy.h"
#include "engine/primitive.h"
#include "engine/sound.h"
#include "engine/world.h"
#include "engine/graphics/bloommodule.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/particlemodule.h"
#include "engine/graphics/shadermodule.h"
#include "engine/graphics/uishadermodule.h"
#include "csm/csm.h"
#include "csm/csm_collide.h"
#include "glm/gtx/rotate_vector.hpp"
#include <QKeyEvent>
#include <chrono>
#include <list>

using namespace COG;

int PARTS_W = 256;
int PARTS_H = 256;

GameScreen::GameScreen(CS1972Engine::Game *parent)
    : Screen(parent)
{
    if (!graphics().hasTexture("cube1"))
        graphics().putTexture("cube1", graphics().loadTextureFromQRC(":/images/cube1.png", GL_LINEAR));
    if (!graphics().hasTexture("cube2"))
        graphics().putTexture("cube2", graphics().loadTextureFromQRC(":/images/cube2.png", GL_LINEAR));
    if (!graphics().hasTexture("diamond1"))
        graphics().putTexture("diamond1", graphics().loadTextureFromQRC(":/images/diamond1.png", GL_LINEAR));
    if (!graphics().hasTexture("particle"))
        graphics().putTexture("particle", graphics().loadTextureFromQRC(":/images/particle1.png", GL_LINEAR));
    if (!graphics().hasTexture("reticle"))
        graphics().putTexture("reticle", graphics().loadTextureFromQRC(":/images/reticle.png", GL_LINEAR));
    if (!graphics().hasTexture("enemyshot"))
        graphics().putTexture("enemyshot", graphics().loadTextureFromQRC(":/images/enemyshot.png", GL_LINEAR));
    if (!graphics().hasTexture("target"))
        graphics().putTexture("target", graphics().loadTextureFromQRC(":/images/target.png", GL_NEAREST));
    if (!graphics().hasTexture("hud"))
        graphics().putTexture("hud", graphics().loadTextureFromQRC(":/images/hud.png", GL_LINEAR));

    graphics().deferred()->initGbuffer(parent->width(), parent->height());
    graphics().bloom()->initBuffers(parent->width(), parent->height());
    graphics().particle()->init(PARTS_W, PARTS_H);

    if (!audio().hasSound("handclap.aif"))
        audio().putSound("handclap.aif", audio().createSoundSample("sound/handclap.aif"));
    if (!audio().hasSound("808hh08.aif"))
        audio().putSound("808hh08.aif", audio().createSoundSample("sound/808hh08.aif"));
    if (!audio().hasSound("noisy1.aif"))
        audio().putSound("noisy1.aif", audio().createSoundSample("sound/noisy1.aif"));
    if (!audio().hasSound("supergate-snare.aif"))
        audio().putSound("supergate-snare.aif", audio().createSoundSample("sound/supergate-snare.aif"));
    if (!audio().hasSound("popslap.aif"))
        audio().putSound("popslap.aif", audio().createSoundSample("sound/popslap.aif"));

    m_bgm = audio().createSoundStream("sound/1.mp3");
    m_bgm->setMusicParams(150.f, 0.052245f);
    m_bgm->setLoop(true);
    m_bgm->setLoopBeats(32.f, 96.f);

    m_world = new CS1972Engine::World(parent, NUM_LAYERS);

    m_player = new PlayerEntity();
    GAME->controller(m_player);

    m_world->addEntity(LAYER_CONTROLLER, m_player);
    m_world->addEntity(LAYER_AMBIENCE, new AmbientEntity(0.f));
}

GameScreen::~GameScreen() {
    graphics().deferred()->cleanupGbuffer();
    graphics().bloom()->cleanupBuffers();
    graphics().particle()->cleanup();

    m_world->deleteEntitiesOnDeconstruct(true);
    delete m_world;

    delete m_bgm;
}

void GameScreen::boundMouse() {
    float W = parent->width();
    float H = parent->height();
    float hSize = glm::round(H*RETICLE_SIZE_FACTOR * 0.5f);
    float viewSensitivity = graphics().camera()->fovy() / W;

    if (m_mousePosition.x < hSize) {
        graphics().camera()->yaw(graphics().camera()->yaw() + viewSensitivity*(m_mousePosition.x - hSize));
        m_mousePosition.x = hSize;
    } else if (m_mousePosition.x > W-hSize) {
        graphics().camera()->yaw(graphics().camera()->yaw() + viewSensitivity*(m_mousePosition.x + hSize-W));
        m_mousePosition.x = W-hSize;
    }
    graphics().camera()->yaw(glm::min(glm::max(-1.f*m_maxYaw, graphics().camera()->yaw()), m_maxYaw));

    if (m_mousePosition.y < hSize) {
        graphics().camera()->pitch(graphics().camera()->pitch() - viewSensitivity*(m_mousePosition.y - hSize));
        m_mousePosition.y = hSize;
    } else if (m_mousePosition.y > H-hSize) {
        graphics().camera()->pitch(graphics().camera()->pitch() - viewSensitivity*(m_mousePosition.y + hSize-H));
        m_mousePosition.y = H-hSize;
    }
    graphics().camera()->pitch(glm::min(glm::max(-1.f*m_maxPitch, graphics().camera()->pitch()), m_maxPitch));

    m_mousePosition = glm::round(m_mousePosition);
}

void GameScreen::startShooting(float beat) {
    audio().playSound("handclap.aif");

    float thisBeat = glm::round(beat);
    if (m_prevBeatChecked < thisBeat)
        m_prevBeatChecked = thisBeat - 1.f;

    // Check this beat, either assigning it a good combo or bad combo
    if (m_prevBeatChecked < thisBeat) {
        float timing = glm::abs(beat-thisBeat) / audio().bgm()->bpm() * 60.f;
        if (timing <= PERFECT_TIMING_WINDOW)
            goodCombo(thisBeat);
        else
            badCombo(thisBeat);
        m_prevBeatChecked += 1.f;
    }

    m_prevShot = glm::max(m_prevShot, glm::round(beat*4.f)/4.f);
}

void GameScreen::keepShooting(float beat) {
    m_shootUntil = beat + 2.f;

    // Check combo up to current beat
    float thisBeat = glm::round(beat);
    while (m_prevBeatChecked < thisBeat-1.f) {
        m_prevBeatChecked += 1.f;
        missCombo(beat);
    }

    // Then check this beat: if we've gone past the perfect timing window, then this beat is a miss
    if (m_prevBeatChecked < thisBeat) {
        if ((beat-thisBeat)/audio().bgm()->bpm()*60.f > PERFECT_TIMING_WINDOW && thisBeat > m_prevPerfectShot + 1.f) {
            missCombo(thisBeat);
            m_prevBeatChecked += 1.f;
        }
    }
}

void GameScreen::goodCombo(float beat) {
    if (m_combo < MAX_COMBO)
        m_combo += 1;
    m_prevPerfectShot = beat;
    m_prevJudge = 1;
}

void GameScreen::badCombo(float) {
    if (m_combo > 1) {
        m_combo -= 1;
        m_prevJudge = 2;
    }
}

void GameScreen::missCombo(float beat) {
    if (m_combo > 1) {
        m_combo -= 1;
        m_prevJudge = 3;
        m_prevMiss = beat;
    }
}

void GameScreen::takeDamage() {
    if (m_iframes <= 0.f) {
        m_health -= 1;
        m_iframes = 4.f;
        m_defenseDisable = 0.f;
    }
}

void GameScreen::defend(float beat, int lane) {
    for (std::deque<EnemyShotEntity *>::iterator it = m_player->attachedShots()->begin(); it != m_player->attachedShots()->end(); ++it) {
        if ((*it)->approachLane() != lane)
            continue;
        float timing = ((*it)->hitBeat()-beat) / audio().bgm()->bpm() * 60.f;
        std::cout << timing;
        std::cout.flush();
        if (timing < -1.f * DEFENSE_TIMING_WINDOW)
            continue;
        else if (timing <= DEFENSE_TIMING_WINDOW) {
            m_world->deleteEntity(*it);
            m_player->attachedShots()->erase(it);
            break;
        } else
            m_defenseDisable = 2.f;
    }
}

float prevBeat = -1.f;

void GameScreen::tick(float seconds) {
    float beat = audio().getBeat();

    // Sometimes the very first tick will be like 10000 seconds due to all that time being used initializing
    // Always bound the first tick to at most 0.017f
    if (m_firstTick) {
        m_firstTick = false;
        if (seconds > 0.017f)
            seconds = 0.017f;
        audio().queueBgmOnBeat(m_bgm, 0.f);
    }

    float beats = seconds*audio().bgm()->bpm()/60.f;

    boundMouse();

    // Check all shooting related things
    float m_nextShot = glm::ceil(beat*4.f)/4.f;
    if (m_mouseHeld[0] || m_keysHeld[3])
        keepShooting(beat);

    glm::vec2 mousePosition = glm::vec2(2.f, -2.f) * m_mousePosition/glm::vec2(parent->width(), parent->height()) + glm::vec2(-1.f, 1.f);
    glm::vec3 pos = graphics().camera()->position();
    glm::vec3 dir = graphics().camera()->inverseOrthoProject(glm::vec3(mousePosition.x, mousePosition.y, 0.1));
    csm::cone cone(pos, dir, RETICLE_SIZE_FACTOR_HIT);
    while (m_prevShot < m_nextShot && m_nextShot < m_shootUntil) {
        m_prevShot += 0.25f;

        // Determine the closest valid (still has enough health to be locked-on) enemy under the reticle
        float best = std::numeric_limits<float>::infinity();
        EnemyEntity *bestEnemy = 0;
        std::list<EnemyEntity *> *enemies = (std::list<EnemyEntity *> *)m_world->getEntities(LAYER_ENEMIES);
        for (std::list<EnemyEntity *>::iterator it = enemies->begin(); it != enemies->end(); ++it) {
            if (!(*it)->targetable())
                continue;
            bool hit = csm::intersect_cone_ellipsoid(cone, (*it)->getEllipsoid() + (*it)->position());
            if (hit && (*it)->futureHealth() > 0) {
                float dist = glm::distance2(pos, (*it)->position());
                if (dist < best) {
                    best = dist;
                    bestEnemy = *it;
                }
            }
        }

        if (bestEnemy != 0) {
            glm::vec3 behind = -1.f * graphics().camera()->lookVector();
            glm::vec3 angle = glm::normalize(glm::rotate(graphics().camera()->upVector(), glm::pi<float>()*glm::mod(m_prevShot, 2.f), graphics().camera()->lookVector()));
            m_world->addEntity(new PlayerShotEntity(m_player, m_prevShot, bestEnemy, behind, angle));
            audio().queueSoundOnBeat("808hh08.aif", m_prevShot);
        }
    }

    m_player->combo(m_combo);

    // Check enemy shots
    for (std::deque<EnemyShotEntity *>::iterator it = m_player->attachedShots()->begin(); it != m_player->attachedShots()->end(); ) {
        if ((beat-(*it)->hitBeat())/audio().bgm()->bpm()*60.f > DEFENSE_TIMING_WINDOW) {
            takeDamage();
            m_world->deleteEntity(*it);
            it = m_player->attachedShots()->erase(it);
        } else
            ++it;
    }
    if (m_iframes > 0.f)
        m_iframes -= beats;
    if (m_defenseDisable > 0.f)
        m_defenseDisable -= beats;

    // Update the world
    while (prevBeat < beat) {
        prevBeat += 1.f;
        float x = csm::rand(-12.f, 12.f);
        float y = csm::rand(-3.f, 3.f);
        m_world->addEntity(LAYER_ENEMIES, new CubeaEnemy(prevBeat, glm::vec3(40.f, y, x)));
        if (glm::mod(prevBeat, 16.f) == 3.f)
            m_world->addEntity(LAYER_ENEMIES, new DiamondaEnemy(prevBeat, glm::vec3(csm::rand(5.f, 6.f), 20.f, csm::rand(-3.f, 3.f))));
    }

    m_world->tick(beat);

    // Offset everything by the player's position so that player is always at origin
    int layers[2] = { LAYER_AMBIENCE, LAYER_ENEMIES };
    for (int i = 0; i < 2; ++i) {
        const std::list<CS1972Engine::Entity *> *e = m_world->getEntities(layers[i]);
        for (std::list<CS1972Engine::Entity *>::const_iterator it = e->begin(); it != e->end(); ++it)
            ((COGEntity *) (*it))->offsetPosition(m_player->position());
    }
    m_particleOffset -= m_player->position();
    m_player->offsetPosition(m_player->position());

    m_beatstep += beats;
    m_beats += beats;
}

void GameScreen::draw() {
    float beat = audio().getBeat() + GRAPHICS_OFFSET*audio().bgm()->bpm()/60.f;
    GAME->beat(beat);
    drawScene(beat);
    drawHud(beat);

    m_beatstep = 0.f;
}

int bloom = 0;
int deferred = 0;
int particles = 0;

void GameScreen::drawScene(float beat) {
    // Update particles
    if (m_beatstep > 0.f) {
        graphics().particle()->usePhysicsShader();
        graphics().particle()->globalVelocity(m_particleOffset/m_beatstep);
        graphics().particle()->updateParticles(m_beatstep);
        graphics().useShader(0);
        m_particleOffset = glm::vec3(0.f);
    }

    // Set up camera
    boundMouse();
    graphics().camera()->position(glm::vec3(0.f));
    graphics().camera()->fovy(0.75f*glm::half_pi<float>());
    graphics().camera()->near(0.1f);
    graphics().camera()->far(50.f);

    // Draw to g-buffer
    graphics().deferred()->bindGbuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render geometry to g-buffer
    graphics().deferred()->useGbufferShader();
    graphics().shader()->pvTransformFromCamera();
    graphics().shader()->useFog(true, 30.f, 40.f, glm::vec3(0.f));
    m_world->draw(DRAW_GEOMETRY);
    graphics().useShader(0);

    if (particles == 0 || particles == 2) {
        // Draw particles to g-buffer
        graphics().particle()->useParticleShader();
        graphics().particle()->particleStyle("particle", 0.2f);
        graphics().particle()->drawParticles();
        graphics().useShader(0);
    }

    // Stop drawing to g-buffer
    graphics().deferred()->unbindGbuffer();

    // Draw to hdr buffer
    graphics().bloom()->bindHdrBuffer();
    glClear(GL_COLOR_BUFFER_BIT);

    // Prepare to do full-screen filters
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    if (deferred == 0 || deferred == 2) {
        graphics().deferred()->resetLightCounts();

        // Render lights in deferred pass
        graphics().deferred()->useDeferredShader();
        graphics().shader()->pvTransformFromCamera();
        m_world->draw(DRAW_LIGHTS);
        graphics().useShader(0);

        //graphics().deferred()->printLightCounts();
    }

    if (deferred == 0 || deferred == 1) {
        // Blend in glow texture
        graphics().uishader()->use();
        graphics().uishader()->orthoTransform(0.f, 1.f, 1.f, 0.f);
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture(graphics().deferred()->glowTex());
        graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
        graphics().useShader(0);
    }

    // Stop drawing to hdr buffer
    graphics().bloom()->unbindBuffer();

    if (bloom == 0 || bloom == 2) {
        // Draw to blur buffer
        graphics().bloom()->bindBlurBuffer();
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw bright parts of hdr buffer to blur buffer
        graphics().bloom()->useBrightShader();
        graphics().uishader()->orthoTransform(0.f, 1.f, 1.f, 0.f);
        graphics().shader()->bindTexture(graphics().bloom()->hdrTex());
        graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
        graphics().useShader(0);

        // Stop drawing to blur buffer
        graphics().bloom()->unbindBuffer();

        // Blur the blur buffer
        graphics().bloom()->blurBlurBuffer();
    }

    graphics().uishader()->use();
    if (bloom == 0 || bloom == 1) {
        // Draw hdr buffer to screen
        graphics().uishader()->orthoTransform(0.f, 1.f, 1.f, 0.f);
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture(graphics().bloom()->hdrTex());
        graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
    }

    if (bloom == 0 || bloom == 2) {
        // Draw blur buffer to screen
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().shader()->bindTexture(graphics().bloom()->blurTex());
        graphics().uishader()->drawQuad(0.f, 1.f, 0.f, 1.f);
    }

    // Draw particle textures (debug)
    if (particles == 2) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float W = graphics().particle()->width();
        float H = graphics().particle()->height();
        graphics().uishader()->orthoTransform(0.f, parent->width(), parent->height(), 0.f);
        graphics().uishader()->color(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
        graphics().shader()->bindTexture(graphics().particle()->posLifeTex());
        graphics().uishader()->drawQuad(0.f, W, 0.f, H);
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().shader()->bindTexture(graphics().particle()->velTex());
        graphics().uishader()->drawQuad(W, 2.f*W, 0.f, H);
    }

    glDisable(GL_BLEND);
    graphics().useShader(0);

    // Stop doing full-screen filters
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void GameScreen::drawHud(float beat) {
    float W = parent->width();
    float H = parent->height();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    graphics().uishader()->use();
    graphics().uishader()->orthoTransform(0.f, W, H, 0.f);

    // Draw orthographic ui

    m_world->draw(DRAW_ORTHOGRAPHIC);

    // Draw the reticle

    float hSize = H*RETICLE_SIZE_FACTOR_DRAW * 0.5f;
    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture("reticle");

    float hSize2 = hSize * (1.f+glm::mod(beat, 1.f));
    graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, glm::max(0.f, glm::min(0.5f*(m_shootUntil-beat), 1.f-glm::mod(beat, 1.f)))));
    graphics().uishader()->drawQuad(
        m_mousePosition.x-hSize2, m_mousePosition.x+hSize2, m_mousePosition.y-hSize2, m_mousePosition.y+hSize2,
        0.25f, 0.5f, 0.5f, 1.f
    );

    //for (float f = glm::ceil(beat*4.f)/4.f; f < m_shootUntil; f += 0.25f) {
    for (int i = 0; i < m_combo; ++i) {
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().uishader()->drawQuad(
            m_mousePosition.x-hSize, m_mousePosition.x+hSize, m_mousePosition.y-hSize, m_mousePosition.y+hSize,
            //-180.f * glm::mod(f, 2.f),
            -45.f * i,
            0.5f, 0.75f, 0.5f, 1.f
        );
    }

    graphics().uishader()->color(glm::vec4(1.f));
    graphics().uishader()->drawQuad(
        m_mousePosition.x-hSize, m_mousePosition.x+hSize, m_mousePosition.y-hSize, m_mousePosition.y+hSize,
        0.f, 0.25f, 0.f, 0.5f
    );

    /*
    graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, glm::max(0.f, 0.5f*(m_shootUntil-beat))));
    graphics().uishader()->drawQuad(
        m_mousePosition.x-hSize, m_mousePosition.x+hSize, m_mousePosition.y-hSize, m_mousePosition.y+hSize,
        -180.f * glm::mod(beat, 2.f),
        0.25f, 0.5f, 0.f, 0.5f
    );
    */

    if (m_mouseHeld[0] || m_keysHeld[3] ||  beat-m_prevMiss < 2.f*PERFECT_TIMING_WINDOW) {
        hSize *= 1.05f;
        if (m_prevJudge == 2 || m_prevJudge == 3)
            graphics().uishader()->color(glm::vec4(1.f, 0.f, 0.f, 1.f));
        else
            graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, 0.5f));
        graphics().uishader()->drawQuad(
            m_mousePosition.x-hSize, m_mousePosition.x+hSize, m_mousePosition.y-hSize, m_mousePosition.y+hSize,
            0.f, 0.25f, 0.5f, 1.f
        );
        for (int i = 0; i < m_combo; ++i) {
            graphics().uishader()->drawQuad(
                m_mousePosition.x-hSize, m_mousePosition.x+hSize, m_mousePosition.y-hSize, m_mousePosition.y+hSize,
                -45.f * i,
                0.75f, 1.f, 0.5f, 1.f
            );
        }
    }

    // Draw the HUD

    graphics().shader()->bindTexture("hud");
    graphics().uishader()->color(glm::vec4(1.f));
    graphics().uishader()->drawQuad(
        0.f, W, H-(0.125f*W), H,
        0.f, 1.f, 0.f, 0.25f
    );
    graphics().uishader()->drawQuad(
        0.f, W, 0.f, 0.125f*W,
        0.f, 1.f, 0.75f, 1.f
    );

    // Score
    int score = m_player->score();
    for (int i = 0; i < glm::max(1, (int) log10(m_player->score()) + 1); ++i) {
        int digit = score % 10;
        float left = 1894.f/2048.f - 17.f/2048.f*i;
        float right = left + 32.f/2048.f;
        graphics().uishader()->drawQuad(
            left*W, right*W, 32.f/2048.f*W, 64.f/2048.f*W,
            32.f/2048.f*digit, 32.f/2048.f*digit + 32.f/2048.f, 0.5f, 0.5f+32.f/1024.f
        );
        score /= 10;
    }

    // Life ticks
    for (int i = 0; i < m_health; ++i) {
        float left = 1792.f/2048.f - 64.f/2048.f*i;
        float right = left + 64.f/2048.f;
        graphics().uishader()->drawQuad(
            left*W, right*W, H-104.f/2048.f*W, H-40.f/2048.f*W,
            32.f/2048.f, 96.f/2048.f, 0.25f, 0.25f+64.f/1024.f
        );
    }

    // Shot ticks
    int i = 0;
    for (float f = glm::ceil(beat*4.f)/4.f; f < m_shootUntil; f += 0.25f, ++i) {
        float left = 112.f/2048.f + 28.f/2048.f*i;
        float right = left + 32.f/2048.f;
        graphics().uishader()->drawQuad(
            left*W, right*W, H-66.f/2048.f*W, H-34.f/2048.f*W,
            0.f, 32.f/2048.f, 0.25f, 0.25f+32.f/1024.f
        );
    }

    graphics().useShader(0);
}

void GameScreen::mousePressEvent(QMouseEvent *event) {
    float beat = audio().getBeat() + INPUT_OFFSET*audio().bgm()->bpm()/60.f;

    switch (event->button()) {
    case Qt::LeftButton: {
        m_mouseHeld[0] = true;
        startShooting(beat);
        break;
    }

    case Qt::MiddleButton:
        m_mouseHeld[1] = true;
        break;

    case Qt::RightButton:
        m_mouseHeld[2] = true;
        break;

    default:
        break;
    }
}

void GameScreen::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - parent->width() / 2;
    int dy = event->y() - parent->height() / 2;
    m_mousePosition += glm::vec2(dx, dy);
}

void GameScreen::mouseReleaseEvent(QMouseEvent *event) {
    float beat = audio().getBeat() + INPUT_OFFSET*audio().bgm()->bpm()/60.f;

    switch (event->button()) {
    case Qt::LeftButton:
        m_mouseHeld[0] = false;
        keepShooting(beat);
        break;

    case Qt::MiddleButton:
        m_mouseHeld[1] = false;
        break;

    case Qt::RightButton:
        m_mouseHeld[2] = false;
        break;

    default:
        break;
    }
}

void GameScreen::wheelEvent(QWheelEvent *event) { }

void GameScreen::keyPressEvent(QKeyEvent *event) {
    float beat = audio().getBeat() + INPUT_OFFSET*audio().bgm()->bpm()/60.f;

    switch (event->key()) {
    case Qt::Key_W:
        m_keysHeld[0] = true;
        if (!event->isAutoRepeat())
            defend(beat, 0);
        break;

    case Qt::Key_S:
        m_keysHeld[3] = true;
        if (!event->isAutoRepeat())
            startShooting(beat);
        break;

    case Qt::Key_F1:
        deferred = (deferred+1) % 3;
        break;

    case Qt::Key_F2:
        bloom = (bloom+1) % 3;
        break;

    case Qt::Key_F3:
        particles = (particles+1) % 3;
        break;

    default:
        break;
    }
}

void GameScreen::keyReleaseEvent(QKeyEvent *event) {
    float beat = audio().getBeat() + INPUT_OFFSET*audio().bgm()->bpm()/60.f;

    switch (event->key()) {
    case Qt::Key_W:
        m_keysHeld[0] = false;
        break;

    case Qt::Key_S:
        m_keysHeld[3] = false;
        m_shootUntil = beat + 2.f;
        break;

    default:
        break;
    }
}

void GameScreen::resizeEvent(int w, int h) {
    graphics().deferred()->cleanupGbuffer();
    graphics().bloom()->cleanupBuffers();
    graphics().deferred()->initGbuffer(w, h);
    graphics().bloom()->initBuffers(w, h);

    m_mousePosition = glm::vec2(w, h) * 0.5f;
}
