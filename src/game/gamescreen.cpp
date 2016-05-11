#include "gamescreen.h"
#include "mainmenuscreen.h"
#include "scriptparser.h"
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
    graphics().deferred()->initGbuffer(parent->width(), parent->height());
    graphics().bloom()->initBuffers(parent->width(), parent->height());
    graphics().particle()->init(PARTS_W, PARTS_H);

    m_mousePosition = 0.5f*glm::vec2(parent->width(), parent->height());

    m_script = ScriptParser(":/level/level.cogs").parse();

    m_world = new CS1972Engine::World(parent, NUM_LAYERS);

    m_player = new PlayerEntity();
    m_player->giveBehavior(m_script->behaviors["main"]);
    GAME->controller(m_player);
    m_world->addEntity(LAYER_CONTROLLER, m_player);

    AmbientEntity *ambience = new AmbientEntity(0.f);
    m_player->m_ambience = ambience;
    m_world->addEntity(LAYER_AMBIENCE, m_player->m_ambience);

    // Tick once to add all entities
    m_world->tick(0.f);
}

GameScreen::~GameScreen() {
    graphics().deferred()->cleanupGbuffer();
    graphics().bloom()->cleanupBuffers();
    graphics().particle()->cleanup();

    m_world->deleteEntitiesOnDeconstruct(true);
    delete m_world;
    delete m_script;
}

void GameScreen::boundMouse(float seconds) {
    float W = parent->width();
    float H = parent->height();
    float hSize = glm::round(H*RETICLE_SIZE_FACTOR * 0.5f);
    float viewSensitivity = graphics().camera()->fovy() / W;

    m_mousePosition += m_mouseMove;
    m_mouseMove = glm::vec2(0.f);

    glm::vec2 mouseDisplace(0.f);

    if (m_mousePosition.x < 0.25f*W)
        mouseDisplace.x = (1.f-glm::pow(0.01f, seconds)) * (m_mousePosition.x - 0.25f*W);
    else if (m_mousePosition.x > 0.75*W)
        mouseDisplace.x = (1.f-glm::pow(0.01f, seconds)) * (m_mousePosition.x - 0.75f*W);
    if (m_mousePosition.y < 0.25f*H)
        mouseDisplace.y = (1.f-glm::pow(0.01f, seconds)) * (m_mousePosition.y - 0.25f*H);
    else if (m_mousePosition.y > 0.75*H)
        mouseDisplace.y = (1.f-glm::pow(0.01f, seconds)) * (m_mousePosition.y - 0.75f*H);

    glm::vec2 cameraBase(m_baseYaw, m_basePitch);
    glm::vec2 cameraPos = glm::vec2(graphics().camera()->yaw(), graphics().camera()->pitch()) + glm::vec2(1.f, -1.f)*viewSensitivity*mouseDisplace;
    while (cameraPos.x < cameraBase.x - glm::pi<float>()) cameraPos.x += 2.f*glm::pi<float>();
    while (cameraPos.x > cameraBase.x + glm::pi<float>()) cameraPos.x -= 2.f*glm::pi<float>();
    glm::vec2 boundedCameraPos = glm::min(glm::max(-1.f*glm::vec2(m_maxYaw, m_maxPitch)+cameraBase, cameraPos), glm::vec2(m_maxYaw, m_maxPitch)+cameraBase);
    graphics().camera()->yaw(boundedCameraPos.x);
    graphics().camera()->pitch(boundedCameraPos.y);

    mouseDisplace += glm::vec2(1.f, -1.f)*(boundedCameraPos-cameraPos)/viewSensitivity;
    m_mousePosition -= mouseDisplace;
    m_mousePosition = glm::min(glm::max(glm::vec2(0.f, 0.f), m_mousePosition), glm::vec2(W, H));
}

void GameScreen::startShooting(float beat) {
    if (m_gameOver)
        return;

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
    if (m_gameOver)
        return;

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
    if (!m_gameOver && m_iframes <= 0.f) {
        m_health -= 1;
        m_iframes = 4.f;
        m_defenseDisable = 0.f;
    }
}

void GameScreen::defend(float beat, int lane) {
    if (m_gameOver)
        return;

    if (m_defenseDisable <= 0.f) {
        for (std::deque<EnemyShotEntity *>::iterator it = m_player->attachedShots()->begin(); it != m_player->attachedShots()->end(); ++it) {
            if ((*it)->approachLane() != lane)
                continue;
            float timing = ((*it)->hitBeat()-beat) / audio().bgm()->bpm() * 60.f;
            if (timing < -1.f * DEFENSE_TIMING_WINDOW)
                continue;
            else if (timing <= DEFENSE_TIMING_WINDOW) {
                audio().queueSoundOnBeat("tambourine-simple1.aif", (*it)->hitBeat());
                m_prevSuccessfulBlock[(*it)->approachLane()] = (*it)->hitBeat();
                m_player->makeParticles(32, (*it)->visualPosition(), 0.1f, 2.f*m_player->velocity(), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.25f, 0.5f));
                m_world->deleteEntity(*it);
                m_player->attachedShots()->erase(it);
                return;
            }
        }
    }

    audio().getSound("chewie_scratch.aif")->stop();
    audio().playSound("chewie_scratch.aif");
    if (m_iframes <= 0.f)
        m_defenseDisable = 2.f;
    m_prevMissedBlock[lane] = beat;
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
        // Tick the controller or else it'll never be updated because there's no music by default
        m_player->tickBeats(0.f);
    }

    float beats = 0.f;
    if (audio().bgm())
        beats = seconds*audio().bgm()->bpm()/60.f;

    boundMouse(seconds);

    if (m_iframes > 0.f)
        m_iframes -= beats;
    if (m_defenseDisable > 0.f)
        m_defenseDisable -= beats;

    // Update the world
    m_world->tick(beat);

    // Update the camera
    if (m_player->m_cameraBehavior == 1)
        m_player->m_baseYaw += beats/12.f;
    while (glm::pi<float>()*m_player->m_baseYaw < m_baseYaw-glm::pi<float>()) m_player->m_baseYaw += 2.f;
    while (glm::pi<float>()*m_player->m_baseYaw > m_baseYaw+glm::pi<float>()) m_player->m_baseYaw -= 2.f;
    float newYaw = (1.f-glm::pow(0.01f, seconds)) * (glm::pi<float>()*m_player->m_baseYaw-m_baseYaw) + m_baseYaw;
    graphics().camera()->yaw(graphics().camera()->yaw() + newYaw-m_baseYaw);
    graphics().camera()->pitch(graphics().camera()->pitch() + glm::pi<float>()*m_player->m_basePitch-m_basePitch);
    m_baseYaw = newYaw;
    m_basePitch = glm::pi<float>()*m_player->m_basePitch;
    m_maxYaw = glm::pi<float>()*m_player->m_yawLimit;
    m_maxPitch = glm::pi<float>()*m_player->m_pitchLimit;

    // Offset everything by the player's position so that player is always at origin
    int layers[2] = { LAYER_AMBIENCE };
    for (int i = 0; i < 2; ++i) {
        const std::list<CS1972Engine::Entity *> *e = m_world->getEntities(layers[i]);
        for (std::list<CS1972Engine::Entity *>::const_iterator it = e->begin(); it != e->end(); ++it)
            ((COGEntity *) (*it))->offsetPosition(m_player->position());
    }
    m_particleOffset -= m_player->position();
    m_player->offsetPosition(m_player->position());

    // Check all shooting related things
    float m_nextShot = glm::ceil(beat*4.f)/4.f;
    if (m_mouseHeld[0] || m_keysHeld[3])
        keepShooting(beat);

    if (m_nextShot < m_shootUntil) {
        // Determine the closest valid (still has enough health to be locked-on) enemy under the reticle
        glm::vec2 mousePosition = glm::vec2(2.f, -2.f) * m_mousePosition/glm::vec2(parent->width(), parent->height()) + glm::vec2(-1.f, 1.f);
        glm::vec3 pos = graphics().camera()->position();
        glm::vec3 dir = graphics().camera()->inverseOrthoProject(glm::vec3(mousePosition.x, mousePosition.y, 0.1));
        csm::cone cone(pos, dir, RETICLE_SIZE_FACTOR_HIT);

        float best = std::numeric_limits<float>::infinity();
        EnemyEntity *bestEnemy = 0;
        std::list<EnemyEntity *> *enemies = (std::list<EnemyEntity *> *)m_world->getEntities(LAYER_ENEMIES);
        for (auto it = enemies->begin(); it != enemies->end(); ++it) {
            if (!(*it)->targetable() || (*it)->futureHealth() <= 0 || (*it)->totalBeats() <= 0.f)
                continue;
            bool hit = csm::intersect_cone_ellipsoid(cone, (*it)->getEllipsoid() + (*it)->position());
            if (hit && glm::dot((*it)->position(), dir) > 0.f) {
                float dist = glm::distance2(pos, (*it)->position());
                if (dist < best) {
                    best = dist;
                    bestEnemy = *it;
                }
            }
        }
        if (bestEnemy)
            m_nextTarget = bestEnemy;
    }

    while (m_prevShot < m_nextShot && m_nextShot < m_shootUntil) {
        m_prevShot += 0.25f;

        if (m_nextTarget != 0) {
            glm::vec3 behind = -1.f * graphics().camera()->lookVector();
            glm::vec3 angle = glm::normalize(glm::rotate(graphics().camera()->upVector(), glm::pi<float>()*glm::mod(m_prevShot, 2.f), graphics().camera()->lookVector()));
            m_world->addEntity(new PlayerShotEntity(m_player, m_prevShot, m_nextTarget, behind, angle));
            audio().queueSoundOnBeat("808hh08.aif", m_prevShot);
            m_nextTarget = 0;
        }
    }

    m_player->m_combo = m_combo;

    // Check enemy shots
    for (std::deque<EnemyShotEntity *>::iterator it = m_player->attachedShots()->begin(); it != m_player->attachedShots()->end(); ) {
        if ((beat-(*it)->hitBeat())/audio().bgm()->bpm()*60.f > DEFENSE_TIMING_WINDOW) {
            takeDamage();
            m_world->deleteEntity(*it);
            it = m_player->attachedShots()->erase(it);
        } else
            ++it;
    }
    while (m_player->m_dealtDamage) {
        m_player->m_dealtDamage--;
        takeDamage();
    }

    // Check game over status
    if (m_health <= 0) {
        m_shootUntil = beat;
        m_gameOver = 1;
    } else if (m_player->m_win) {
        m_gameOver = 2;
    }

    if (m_gameOver == 0) {
        m_fade -= 0.5f*seconds;
        if (m_fade < 0.f)
            m_fade = 0.f;
    } else {
        m_mousePosition = 0.5f*glm::vec2(parent->width(), parent->height());
        if ((m_gameOver == 1 && m_iframes <= 0.f) || m_fade > 0.f)
            m_fade += 0.5f*seconds;
        if (m_fade >= 1.f) {
            audio().bgm()->stop();
            audio().clearBgm();
            parent->popScreen();
            parent->pushScreen(new MainMenuScreen(parent));
        }
    }

    m_beatstep += beats;
    m_beats += beats;
}

void GameScreen::draw() {
    float beat = 0.f;
    if (audio().bgm())
        beat = audio().getBeat() + GRAPHICS_OFFSET*audio().bgm()->bpm()/60.f;
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
    graphics().camera()->position(glm::vec3(0.f));
    graphics().camera()->fovy(0.75f*glm::half_pi<float>());
    graphics().camera()->near(0.1f);
    graphics().camera()->far(2.f*m_player->m_maxFog);

    // Draw to g-buffer
    graphics().deferred()->bindGbuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render geometry to g-buffer
    graphics().deferred()->useGbufferShader();
    graphics().shader()->pvTransformFromCamera();
    graphics().shader()->useFog(true, m_player->m_minFog, m_player->m_maxFog, glm::vec3(0.f));
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    {
        // Blend in additive geometry
        glEnable(GL_DEPTH_TEST);
        graphics().shader()->use();
        graphics().deferred()->blitGbufferDepthTo(parent->width(), parent->height(), graphics().bloom()->hdr());
        graphics().shader()->pvTransformFromCamera();
        graphics().shader()->useLight(false);
        graphics().shader()->useFog(true, m_player->m_minFog, m_player->m_maxFog, glm::vec3(0.f));

        glDepthMask(false);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        m_world->draw(DRAW_ADDITIVE);

        graphics().useShader(0);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(true);
        glDisable(GL_DEPTH_TEST);
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
        graphics().shader()->bindTexture(graphics().particle()->colorTex());
        graphics().uishader()->drawQuad(2.f*W, 3.f*W, 0.f, H);
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

    m_world->draw(DRAW_ORTHOGRAPHIC);
    if (m_gameOver == 0)
        drawReticle(beat);
    drawDefenseRing(beat);
    drawHudComponents(beat);

    graphics().useShader(0);
}

void GameScreen::drawReticle(float beat) {
    float W = parent->width();
    float H = parent->height();

    glm::vec2 mousePos = glm::round(m_mousePosition);
    glm::vec2 size(H*RETICLE_SIZE_FACTOR_DRAW);
    graphics().shader()->useTexture(true);
    graphics().shader()->bindTexture("reticle");

    // Draw shooting pulsing
    graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, glm::max(0.f, glm::min(0.5f*(m_shootUntil-beat), 1.f-glm::mod(beat, 1.f)))));
    graphics().uishader()->drawQuad(
        mousePos, size * (1.f+glm::mod(beat, 1.f)),
        0.25f, 0.5f, 0.5f, 1.f
    );

    // Draw needles that pop out while shooting
    /*
    if (beat < m_shootUntil+0.25f) {
        float rotation = glm::mod(beat-glm::mod(beat, 0.25f), 2.f);
        graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, 1.f-glm::abs(8.f*glm::mod(beat, 0.25f)-1.f)));
        glm::vec2 pos = glm::rotate(glm::vec2(0.f, (-0.25f-2.f*glm::mod(beat, 0.25f))*size.y), rotation*glm::pi<float>());
        graphics().uishader()->drawQuad(
            mousePos+pos, size, -180.f*rotation,
            0.5f, 0.75f, 0.f, 0.5f
        );
    }
    */

    // Draw combo ticks
    for (int i = 0; i < m_combo; ++i) {
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().uishader()->drawQuad(
            mousePos, size, -45.f*i,
            0.5f, 0.75f, 0.5f, 1.f
        );
    }

    // Draw the main part of the reticle
    graphics().uishader()->color(glm::vec4(1.f));
    graphics().uishader()->drawQuad(
        mousePos, size,
        0.f, 0.25f, 0.f, 0.5f
    );

    // Draw the spinning white thing
    graphics().uishader()->color(glm::vec4(1.f));
    graphics().uishader()->drawQuad(
        mousePos, size, -180.f * glm::mod(beat, 2.f),
        0.25f, 0.5f, 0.f, 0.5f
    );

    // Draw the hold-down effects
    if (m_mouseHeld[0] || m_keysHeld[3] ||  beat-m_prevMiss < 2.f*PERFECT_TIMING_WINDOW) {
        size *= 1.05f;
        if (m_prevJudge == 2 || m_prevJudge == 3)
            graphics().uishader()->color(glm::vec4(1.f, 0.f, 0.f, 1.f));
        else
            graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, 0.5f));
        graphics().uishader()->drawQuad(
            mousePos, size,
            0.f, 0.25f, 0.5f, 1.f
        );
        for (int i = 0; i < m_combo; ++i) {
            graphics().uishader()->drawQuad(
                mousePos, size, -45.f * i,
                0.75f, 1.f, 0.5f, 1.f
            );
        }
    }
}

void GameScreen::drawDefenseRing(float beat) {
    float W = parent->width();
    float H = parent->height();

    // Figure out whether or not we should display the ring, and which lanes to display
    float lanesHave[3] = { 0.f, 0.f, 0.f };
    float hasAny = 0.f;
    for (std::deque<EnemyShotEntity *>::iterator it = m_player->attachedShots()->begin(); it != m_player->attachedShots()->end(); ++it) {
        if ((*it)->hitBeat()-beat > 12.f)
            break;
        else if ((*it)->hitBeat()-beat > 4.f) {
            lanesHave[(*it)->approachLane()] = glm::max(0.5f, lanesHave[(*it)->approachLane()]);
            hasAny = glm::max(0.5f, hasAny);
        } else {
            lanesHave[(*it)->approachLane()] = 1.f;
            hasAny = 1.f;
        }
    }
    for (int i = 0; i < 3; ++i)
        if (beat - m_prevSuccessfulBlock[i] < 2.f) {
            lanesHave[i] = 1.f;
            hasAny = 1.f;
        }
    if (m_iframes > 0.f)
        hasAny = 1.f;

    if (hasAny > 0.f) {
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("enemyshot");

        glm::vec2 size(H*DEFENSERING_SIZE_FACTOR);
        glm::vec2 center(0.5f*W, H-0.15f*H);
        float offset = size.x*2.5f;
        float laneLength = size.x*8.f;
        float fallSpeed = 0.15f;
        float laneOffset = offset-size.x/2.f+laneLength/2.f;
        glm::vec2 laneSize(size.x, laneLength);
        glm::vec2 markerCenters[3] = { center-glm::vec2(0.f, offset), center-glm::vec2(offset, 0.f), center+glm::vec2(offset, 0.f) };
        glm::vec2 laneCenters[3] = { center-glm::vec2(0.f, laneOffset), center-glm::vec2(laneOffset, 0.f), center+glm::vec2(laneOffset, 0.f) };
        glm::vec2 offsets[3] = { glm::vec2(0.f, -1.f), glm::vec2(-1.f, 0.f), glm::vec2(1.f, 0.f) };
        float noteOffset = fallSpeed*laneLength;
        float letterOffset = size.x;
        float rotations[3] = { 0.f, 90.f, -90.f };

        float flicker = hasAny;
        if (m_defenseDisable > 0.f)
            flicker = (glm::mod(beat, 0.5f) < 0.25f) ? 1.f : 0.5f;

        // Draw UI elements
        graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, flicker));
        if (m_iframes > 0.f)
            graphics().uishader()->drawQuad(
                center, 4.f*size, 0.f,
                0.25f, 0.5f, 0.5f, 0.75f
            );
        if (m_defenseDisable <= 0.f) {
            if (m_iframes <= 0.f)
                graphics().uishader()->drawQuad(
                    center, 4.f*size, 0.f,
                    0.f, 0.25f, 0.5, 0.75f
                );
            graphics().uishader()->drawQuad(
                center, 4.f*size, 45.f*glm::mod(beat, 8.f),
                0.5f, 0.75f, 0.f, 0.25f
            );
            graphics().uishader()->drawQuad(
                center, 4.f*size, -45.f*glm::mod(beat, 8.f),
                0.75f, 1.f, 0.f, 0.25f
            );
        } else {
            if (m_iframes <= 0.f)
                graphics().uishader()->drawQuad(
                    center, 4.f*size, 0.f,
                    0.f, 0.25f, 0.75f, 1.f
                );
            graphics().uishader()->drawQuad(
                center, 4.f*size, 0.f,
                0.75f, 1.f, 0.25f, 0.5f
            );
        }

        // Draw lanes
        if (m_defenseDisable <= 0.f)
            for (int i = 0; i < 3; ++i) {
                graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, lanesHave[i]));
                graphics().uishader()->drawQuad(
                    laneCenters[i], laneSize, rotations[i],
                    0.0625f, 0.125f, 0.f, 0.5f
                );
                graphics().uishader()->drawQuad(
                    markerCenters[i] - letterOffset*offsets[i], size, 0.f,
                    0.375f, 0.4375f, 0.125f+0.0625*i, 0.1875f+0.0625*i
                );
                if (lanesHave[i] >= 1.f) {
                    graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, glm::abs(glm::mod(beat, 2.f)-1.f)));
                    graphics().uishader()->drawQuad(
                        markerCenters[i] - letterOffset*offsets[i], size, 0.f,
                        0.4375f, 0.5f, 0.125f+0.0625*i, 0.1875f+0.0625*i
                    );
                }
            }

        // Draw projectiles
        int i = 0;
        GLfloat *lines = new GLfloat[16*m_player->attachedShots()->size()];

        float blink = (glm::mod(beat, 1.f) < 0.5f) ? 0.f : 0.0625f;
        for (auto it = m_player->attachedShots()->begin(); it != m_player->attachedShots()->end(); ++it) {
            if ((*it)->hitBeat()-beat > 12.f)
                break;
            if ((*it)->hitBeat()-beat <= 4.f)
                graphics().uishader()->color(glm::vec4(1.f));
            else
                graphics().uishader()->color(glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
            float offset = 0.f;
            if (glm::mod((*it)->hitBeat(), 1.f) != 0.f)
                offset += 0.125f;
            if (glm::mod((*it)->hitBeat(), 0.5f) != 0.f)
                offset += 0.125f;
            if (glm::mod((*it)->hitBeat(), 0.25f) != 0.f)
                offset += 0.125f;
            int lane = (*it)->approachLane();
            glm::vec2 pos = markerCenters[lane] + offsets[lane]*noteOffset*((*it)->hitBeat()-beat);
            graphics().uishader()->drawQuad(
                pos, size, 0.f,
                0.125f+blink, 0.1875f+blink, 0.f+offset, 0.0625f+offset
            );

            if (pos.x >= -1.f*size.x && pos.x <= W+size.x && pos.y >= -1.f*size.y) {
                glm::vec3 ortho = graphics().uishader()->cameraSpaceToUisSpace((*it)->visualPosition());
                lines[8*i+0] = pos.x;
                lines[8*i+1] = pos.y;
                lines[8*i+2] = 0.f;
                ++i;
                lines[8*i+0] = ortho.x;
                lines[8*i+1] = ortho.y;
                lines[8*i+2] = 0.f;
                ++i;
            }
        }

        if (i) {
            graphics().shader()->mTransform(glm::mat4(1.f));
            graphics().shader()->useTexture(false);
            graphics().uishader()->color(glm::vec4(1.f, 0.f, 0.f, 0.5f));
            glLineWidth(1.f);
            auto line = new CS1972Engine::Primitive(i, 8*i*sizeof(GLfloat), GL_LINES, lines, 0);
            line->drawArray();
            delete line;
        }
        delete[] lines;

        // Draw hit markers && judgements
        graphics().shader()->useTexture(true);
        for (int i = 0; i < 3; ++i) {
            if (m_keysHeld[i]) {
                graphics().uishader()->color(glm::vec4(1.f));
                if (m_defenseDisable <= 0.f && m_prevSuccessfulBlock[i] > m_prevMissedBlock[i])
                    graphics().uishader()->drawQuad(
                        markerCenters[i], size, 0.f,
                        0.f, 0.0625f, 0.0625f, 0.125f
                    );
                else
                    graphics().uishader()->drawQuad(
                        markerCenters[i], size, 0.f,
                        0.f, 0.0625f, 0.125f, 0.1875f
                    );
            } else if (m_defenseDisable <= 0.f) {
                graphics().uishader()->color(glm::vec4(1.f, 1.f, 1.f, lanesHave[i]));
                graphics().uishader()->drawQuad(
                    markerCenters[i], size, 0.f,
                    0.f, 0.0625f, 0.f, 0.0625f
                );
            }

            graphics().uishader()->color(glm::vec4(1.f));
            if (beat-m_prevMissedBlock[i] < 2.f) {
                graphics().uishader()->drawQuad(
                    markerCenters[i], 2.f*size, 0.f,
                    0.375f, 0.5f, 0.f, 0.125f
                );
            } else if (beat-m_prevSuccessfulBlock[i] < 2.f) {
                float offset = 0.f;
                if (glm::mod(m_prevSuccessfulBlock[i], 1.f) != 0.f)
                    offset += 0.125f;
                if (glm::mod(m_prevSuccessfulBlock[i], 0.5f) != 0.f)
                    offset += 0.125f;
                if (glm::mod(m_prevSuccessfulBlock[i], 0.25f) != 0.f)
                    offset += 0.125f;
                graphics().uishader()->drawQuad(
                    markerCenters[i], 2.f*size, 0.f,
                    0.25f, 0.375f, 0.f+offset, 0.125f+offset
                );
            }
        }
    }
}

void GameScreen::drawHudComponents(float beat) {
    float W = parent->width();
    float H = parent->height();

    graphics().shader()->useTexture(true);
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
    int score = m_player->m_score;
    for (int i = 0; i < glm::max(1, (int) log10(m_player->m_score) + 1); ++i) {
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

    // Full-screen effects
    if (m_iframes > 0.f) {
        graphics().shader()->useTexture(false);
        graphics().uishader()->color(glm::vec4(1.f, 0.f, 0.f, m_iframes/8.f));
        graphics().uishader()->drawQuad(0.f, W, 0.f, H);
    }

    if (m_gameOver == 1) {
        graphics().shader()->useTexture(false);
        graphics().uishader()->color(glm::vec4(0.f, 0.f, 0.f, 1.f-m_iframes/4.f));
        graphics().uishader()->drawQuad(0.f, W, 0.f, H);

        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("hud");
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().uishader()->drawQuad(
            0.5f*glm::vec2(W, H), W/16.f*glm::vec2(16.f, 1.f), 0.f,
            0.5f, 1.f, 0.5f, 0.5625f
        );
    } else if (m_gameOver == 2) {
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("hud");
        graphics().uishader()->color(glm::vec4(1.f));
        graphics().uishader()->drawQuad(
            0.5f*glm::vec2(W, H), W/16.f*glm::vec2(16.f, 2.f), 0.f,
            0.5f, 1.f, 0.5625f, 0.6875f
        );
    }

    if (m_fade > 0.f) {
        graphics().shader()->useTexture(false);
        graphics().uishader()->color(glm::vec4(0.f, 0.f, 0.f, m_fade));
        graphics().uishader()->drawQuad(0.f, W, 0.f, H);
    }
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
    if (m_gameOver)
        return;

    int dx = event->x() - parent->width() / 2;
    int dy = event->y() - parent->height() / 2;
    m_mouseMove += glm::vec2(dx, dy) * MOUSE_SENSITIVITY_FACTOR * (float) parent->height();
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

    case Qt::Key_A:
        m_keysHeld[1] = true;
        if (!event->isAutoRepeat())
            defend(beat, 1);
        break;

    case Qt::Key_D:
        m_keysHeld[2] = true;
        if (!event->isAutoRepeat())
            defend(beat, 2);
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

    case Qt::Key_Return:
        if (m_fade <= 0.f)
            m_fade = 0.01f;
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

    case Qt::Key_A:
        m_keysHeld[1] = false;
        break;

    case Qt::Key_D:
        m_keysHeld[2] = false;
        break;

    case Qt::Key_S:
        m_keysHeld[3] = false;
        keepShooting(beat);
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
