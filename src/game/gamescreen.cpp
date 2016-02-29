#include "gamescreen.h"
#include "entity/enemyentity.h"
#include "entity/playerentity.h"
#include "../engine/primitive.h"
#include "../engine/voxel/block.h"
#include "../engine/voxel/voxelmanager.h"
#include "pnchunkgenerator.h"
#include <QKeyEvent>
#include <time.h>

using namespace Minecraft;

using CS1972Engine::Voxel::BlockType;
const BlockType BLOCK_DEFINITIONS[7] = {
    BlockType("air", true, true, 0.f, 15.f/16.f, 0.f, 15.f/16.f, 0.f, 15.f/16.f),
    BlockType("grass", false, false, 0.f, 15.f/16.f, 3.f/16.f, 15.f/16.f, 2.f/16.f, 15.f/16.f),
    BlockType("stone", false, false, 1.f/16.f, 15.f/16.f, 1.f/16.f, 15.f/16.f, 1.f/16.f, 15.f/16.f),
    BlockType("dirt", false, false, 2.f/16.f, 15.f/16.f, 2.f/16.f, 15.f/16.f, 2.f/16.f, 15.f/16.f),
    BlockType("snow", false, false, 2.f/16.f, 11.f/16.f, 2.f/16.f, 11.f/16.f, 2.f/16.f, 11.f/16.f),
    BlockType("sand", false, false, 2.f/16.f, 14.f/16.f, 2.f/16.f, 14.f/16.f, 2.f/16.f, 14.f/16.f),
    BlockType("chaos", false, false, true)
};

GameScreen::GameScreen(CS1972Engine::Game *parent)
    : Screen(parent)
    , m_world(parent)
{
    m_gen = new PNChunkGenerator(time(NULL));
    m_vox = new CS1972Engine::Voxel::VoxelManager(BLOCK_DEFINITIONS, m_gen);
    m_world.useTerrain(m_vox);

    m_player = new PlayerEntity(glm::vec3(0.f, 66.f, 0.f));
    m_vox->streamChunksAround(m_player->position(), 64.f, 64.f, false);
    m_world.addEntity(m_player);
    m_world.tick(0.f);

    // Make a few primitives for the UI
    {
        GLfloat triangle[24] = {
            0.f, 1.f,0.f, 0.f,0.f,-1.f, 0.f, 0.f,
            1.f, 1.f,0.f, 0.f,0.f,-1.f, 0.f, 1.f,
            0.5f,0.f,0.f, 0.f,0.f,-1.f, 0.5f,1.f,
        };
        m_triangle = new CS1972Engine::Primitive(3, sizeof(triangle), triangle);

        GLfloat circleLoop[362*8];
        GLfloat ringStrip[362*2*8];
        for (int i = 0; i < 362; ++i) {
            float d = glm::pi<float>() * i / 180.f;
            circleLoop[8*i+0] = glm::sin(d);
            circleLoop[8*i+1] = -1.f*glm::cos(d);
            circleLoop[8*i+2] = 0.f;
            circleLoop[8*i+3] = 0.f;
            circleLoop[8*i+4] = 0.f;
            circleLoop[8*i+5] = -1.f;

            ringStrip[16*i+ 0] = glm::sin(d);
            ringStrip[16*i+ 1] = -1.f*glm::cos(d);
            ringStrip[16*i+ 2] = 0.f;
            ringStrip[16*i+ 3] = 0.f;
            ringStrip[16*i+ 4] = 0.f;
            ringStrip[16*i+ 5] = -1.f;
            ringStrip[16*i+ 8] = 0.9f*glm::sin(d);
            ringStrip[16*i+ 9] = -0.9f*glm::cos(d);
            ringStrip[16*i+10] = 0.f;
            ringStrip[16*i+11] = 0.f;
            ringStrip[16*i+12] = 0.f;
            ringStrip[16*i+13] = -1.f;
        }
        m_circleLoop = new CS1972Engine::Primitive(360, sizeof(circleLoop), circleLoop);
        m_ringStrip = new CS1972Engine::Primitive(2*362, sizeof(ringStrip), ringStrip);
    }
}

GameScreen::~GameScreen() {
    m_world.deleteEntitiesOnDeconstruct(true);
}

void GameScreen::tick(float seconds) {
    glm::vec2 walk(0.f);
    if (m_keysHeld[0]) walk.x += 1.f;
    if (m_keysHeld[1]) walk.y -= 1.f;
    if (m_keysHeld[2]) walk.x -= 1.f;
    if (m_keysHeld[3]) walk.y += 1.f;
    // Set walk from camera-relative to world-absolute
    if (walk != glm::vec2(0.f)) {
        float yaw = graphics().camera->yaw();
        glm::vec2 dir = glm::vec2(glm::cos(yaw), glm::sin(yaw));
        glm::mat2 m(dir.x, dir.y, -1.f*dir.y, dir.x);
        walk = m*walk;
    }
    bool dashing = m_keysHeld[6];
    bool jumping = m_keysHeld[7];
    m_player->walk(seconds, walk, dashing, jumping);

    // Tick world
    m_vox->streamChunksAround(m_player->position(), 175.f, 225.f);
    m_world.tick(seconds);

    // Update chaos time
    if (!m_chaosTime) {
        // Increase chaos timer
        m_chaosTimer += seconds*CHAOS_TIME_GAIN[m_player->chaosLevel()];
        if (m_chaosTimer >= MAX_CHAOS_TIMER) {
            // Fade into chaos time when timer reaches max
            m_chaosTimer = MAX_CHAOS_TIMER;
            m_chaosFading += seconds/CHAOS_FADE_TIME;
            if (m_chaosFading >= 1.f) {
                // Begin chaos time
                m_chaosFading = 1.f;
                m_chaosTime = true;
                m_spawnTimer = -1.f * SPAWN_TIMER[m_player->chaosLevel()] * SPAWN_INIT_AMOUNT[m_player->chaosLevel()];
                for (std::list<EnemyEntity *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
                    (*it)->attack();
            }
        } else {
            // Fade out of chaos time
            m_chaosFading -= seconds/CHAOS_FADE_TIME;
            if (m_chaosFading < 0.f)
                m_chaosFading = 0.f;
        }

        // Remove lingering enemies that are too far away
        for (std::list<EnemyEntity *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it) {
            if (glm::distance2((*it)->position(), m_player->position()) > m_viewDistance*m_viewDistance) {
                m_world.deleteEntity(*it);
                it = m_enemies.erase(it);
            }
        }

    } else {
        // In chaos time - now for some fun

        // Damage player
        m_player->takeChaosDamage(seconds);

        for (std::list<EnemyEntity *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it) {
            // Remove enemies that have been killed or removed from the world
            if ((*it)->parent() == 0) {
                if ((*it)->dead()) m_chaosTimer -= MAX_CHAOS_TIMER/KILLS_REQUIRED[m_player->chaosLevel()];
                delete *it;
                it = m_enemies.erase(it);
                continue;
            }

            // Teleport enemies that are too far away. This counts as spawning an enemy
            glm::vec2 xzPos((*it)->position().x, (*it)->position().z);
            glm::vec2 xzPlayer(m_player->position().x, m_player->position().z);
            if (glm::distance2(xzPos, xzPlayer) > DESPAWN_DIST*DESPAWN_DIST) {
                m_spawnTimer += SPAWN_TIMER[m_player->chaosLevel()];
                float dir = 2.f*glm::pi<float>() * (float)rand2() / RAND2_MAX;
                (*it)->position(m_player->position() + glm::vec3(SPAWN_DIST*glm::cos(dir), 66.f, SPAWN_DIST*glm::sin(dir)));
            }
        }

        // Spawn new enemies
        m_spawnTimer -= seconds;
        while (m_spawnTimer < 0.f) {
            m_spawnTimer += SPAWN_TIMER[m_player->chaosLevel()];
            float dir = 2.f*glm::pi<float>() * (float)rand2() / RAND2_MAX;
            EnemyEntity *e = new EnemyEntity(m_player, m_player->position() + glm::vec3(SPAWN_DIST*glm::cos(dir), 66.f, SPAWN_DIST*glm::sin(dir)));
            m_enemies.push_back(e);
            m_world.addEntity(e);
        }

        if (m_chaosTimer <= 0.f) {
            // Chaos time is over
            m_chaosTimer = 0.f;
            m_chaosTime = false;
            for (std::list<EnemyEntity *>::iterator it = m_enemies.begin(); it != m_enemies.end(); ++it)
                (*it)->runAway();
        }
    }

    m_viewDistance = m_chaosFading*CHAOS_VIEW_DISTANCE + (1.f-m_chaosFading)*VIEW_DISTANCE;
    m_fogDistance = m_chaosFading*CHAOS_FOG_DISTANCE + (1.f-m_chaosFading)*FOG_DISTANCE;
    m_skyColor = m_chaosFading*CHAOS_SKY_COLOR + (1.f-m_chaosFading)*SKY_COLOR;

    m_time += seconds;
}

void GameScreen::draw() {
    // Prepare to draw
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    graphics().useDefaultShader();
    graphics().camera->position(m_player->position());
    graphics().camera->position_y(m_player->position().y + 1.5f);
    graphics().camera->fovy((0.75f+0.125f*m_player->dashEffect()/5.f)*glm::half_pi<float>());
    graphics().camera->near(0.1f);
    graphics().camera->far(2.f*m_viewDistance);
    graphics().shaderPvTransformFromCamera();
    glm::mat4 m;

    // Draw skybox
    graphics().shaderUseFog(false);
    graphics().shaderUseLight(false);
    graphics().shaderColor(glm::vec4(m_skyColor, 1.f));
    m = glm::mat4(1.f);
    m = glm::translate(m, graphics().camera->position());
    m = glm::scale(m, -2.f*glm::vec3(m_viewDistance));
    graphics().shaderMTransform(m);
    graphics().drawBox();

    // Draw world
    graphics().shaderUseFog(true, m_fogDistance, m_viewDistance, m_skyColor);
    graphics().shaderUseLight(true);
    m_world.draw();

    // Highlight targeted block face
    m_hasTargetBlock = m_vox->rayCast(graphics().camera->position(), graphics().camera->lookVector(), 10.f, m_targetBlock, m_targetFace);
    if (m_hasTargetBlock) {
        graphics().shaderColor(glm::vec4(1.f, 1.f, 0.f, 0.5f));
        graphics().shaderUseTexture(false);
        graphics().shaderUseLight(false);
        // I'm actually just protruding a box that's a tiny bit smaller than a block, a tiny bit out of the block.
        // When it's close the tiny distance needs to be real tiny so it's not visible to the eye.
        // But actually if it's too far away then there would be z-fighting, so the tiny distance needs to be less tiny when it's far away.
        // A wise person once told me that graphics code is just a series of hacks (please have mercy)
        float dist = glm::distance(graphics().camera->position(), m_targetBlock+glm::vec3(0.5f));
        m = glm::mat4(1.f);
        m = glm::translate(m, m_targetBlock+glm::vec3(0.5f)+m_targetFace*glm::vec3(0.0002f*dist));
        m = glm::scale(m, glm::vec3(1.f-0.0001f*dist));
        graphics().shaderMTransform(m);
        graphics().drawBox();
    }

    // Draw attack graphic
    {
        glDisable(GL_DEPTH_TEST);
        float yaw = graphics().camera->yaw();
        float eff = m_player->attackEffect();
        graphics().shaderUseTexture(false);
        graphics().shaderUseLight(true);
        graphics().shaderColor(glm::vec4(0.f, 1.f, 0.f, eff));
        m = glm::mat4(1.f);
        m = glm::translate(m, m_player->position() + (0.5f-eff)*glm::vec3(glm::cos(yaw), 0.f, glm::sin(yaw)));
        m = glm::scale(m, -1.f*glm::vec3(1.f, 2.f, 1.f));
        m = glm::translate(m, -1.f*glm::vec3(0.f, 0.5f, 0.f));
        graphics().shaderMTransform(m);
        graphics().drawCylinder();
        glEnable(GL_DEPTH_TEST);
    }

    // Draw UI
    {
        glDisable(GL_DEPTH_TEST);

        float W = parent->width();
        float H = parent->height();
        graphics().useUiShader();
        graphics().uisOrthoTransform(0.f, W, H, 0.f);
        graphics().shaderUseTexture(false);

        // Health bar
        graphics().uisColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
        graphics().uisQuad(30.f, 70.f, H-30.f-m_player->health(), H-30.f);

        // Energy bar
        float energy = glm::max(0.f, m_player->energy()-m_player->ENERGY_LOCK_LOW);
        if (!m_player->energyLocked())
            graphics().uisColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
        else
            graphics().uisColor(glm::vec4(0.75f, 0.75f, 0.75f, 1.f));
        graphics().uisQuad(90.f, 130.f, H-30.f-energy, H-30.f);

        // Dirt bar
        graphics().uisColor(glm::vec4(0.55f, 0.37f, 0.21f, 1.f));
        graphics().uisQuad(150.f, 190.f, H-30.f-m_player->dirt(), H-30.f);

        // Chaos bar
        graphics().uisColor(glm::vec4(1.f, 0.f, 1.f, 1.f));
        graphics().uisQuad(210.f, 250.f, H-30.f-m_player->chaos()/4.f, H-30.f);

        // Chaos clock
        graphics().uisColor(glm::vec4(1.f, 0.f, 1.f, 1.f));
        if (m_chaosTime)
            graphics().uisColor(glm::vec4(0.5f+0.5f*glm::cos(3.f*glm::pi<float>()*m_time), 0.f, 1.f, 1.f));
        m = glm::mat4(1.f);
        m = glm::translate(m, glm::vec3(W-130.f, H-130.f, 0.f));
        m = glm::scale(m, glm::vec3(100.f));
        graphics().shaderMTransform(m);
        m_ringStrip->drawArray(GL_TRIANGLE_STRIP, 0, 2+2*(int)(360.f*m_chaosTimer/MAX_CHAOS_TIMER));

        graphics().uisColor(glm::vec4(0.5f, 0.f, 1.f, 1.f));
        m_circleLoop->drawArray(GL_LINE_LOOP, 0, 360);

        m = glm::mat4(1.f);
        m = glm::translate(m, glm::vec3(W-130.f, H-130.f, 0.f));
        m = glm::scale(m, glm::vec3(90.f));
        graphics().shaderMTransform(m);
        m_circleLoop->drawArray(GL_LINE_LOOP, 0, 360);

        // Compass
        if (m_chaosFading <= 0.f && m_player->chaosLevel() < 4) {
            glm::vec2 next = m_gen->getChaosBlock(m_player->chaosLevel()) + glm::vec2(0.5f);
            glm::vec2 pos = glm::vec2(m_player->position().x, m_player->position().z);
            glm::vec2 dir = next-pos;
            if (dir != glm::vec2(0.f)) {
                float ang = glm::atan(dir.y, dir.x) - graphics().camera->yaw();
                graphics().uisColor(glm::vec4(1.f, 0.f, 1.f, 1.f));
                m = glm::mat4(1.f);
                m = glm::translate(m, glm::vec3(W-130.f, H-130.f, 0.f));
                m = glm::rotate(m, ang, glm::vec3(0.f, 0.f, 1.f));
                m = glm::translate(m, glm::vec3(0.f, -110.f, 0.f));
                m = glm::scale(m, glm::vec3(10.f));
                graphics().shaderMTransform(m);
                m_triangle->drawArray();
            }
        }

        // Chaos blocks
        graphics().uisColor(glm::vec4(1.f, 1.f, 1.f, 0.25f));
        if (m_player->chaosLevel() >= 4)
            graphics().uisColor(glm::vec4(0.5f+0.5f*glm::cos(3.f*glm::pi<float>()*(m_time+2.4f)), 0.f, 1.f, 1.f));
        graphics().uisQuad(W-125.f, W-80.f, H-125.f, H-80.f);
        if (m_player->chaosLevel() >= 3)
            graphics().uisColor(glm::vec4(0.5f+0.5f*glm::cos(3.f*glm::pi<float>()*(m_time+1.8f)), 0.f, 1.f, 1.f));
        graphics().uisQuad(W-180.f, W-135.f, H-125.f, H-80.f);
        if (m_player->chaosLevel() >= 2)
            graphics().uisColor(glm::vec4(0.5f+0.5f*glm::cos(3.f*glm::pi<float>()*(m_time+1.2f)), 0.f, 1.f, 1.f));
        graphics().uisQuad(W-125.f, W-80.f, H-180.f, H-135.f);
        if (m_player->chaosLevel() >= 1)
            graphics().uisColor(glm::vec4(0.5f+0.5f*glm::cos(3.f*glm::pi<float>()*(m_time+0.6f)), 0.f, 1.f, 1.f));
        graphics().uisQuad(W-180.f, W-135.f, H-180.f, H-135.f);
    }

    // Clean up
    graphics().shaderUnbindTexture();
    graphics().useShader(0);
}

void GameScreen::mousePressEvent(QMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton:
        if (event->modifiers() & Qt::AltModifier)
            m_player->shoot();
        else
            m_player->attack();
        break;

    case Qt::RightButton:
        if (event->modifiers() & Qt::AltModifier)
            m_player->storm();
        else if (m_hasTargetBlock) {
            if (m_player->dirt() < m_player->MAX_DIRT) {
                // Remove block
                CS1972Engine::Voxel::Block b = m_vox->getBlock(m_targetBlock.x, m_targetBlock.y, m_targetBlock.z);
                // Can only dig grass, dirt, sand, snow, chaos
                if ((b.type == 1 || b.type == 3 || b.type == 4 || b.type == 5 || b.type == 6) && m_player->dig()) {
                    m_vox->setBlock(m_targetBlock.x, m_targetBlock.y, m_targetBlock.z, CS1972Engine::Voxel::Block{0});
                    if (b.type == 1 || b.type == 3) {
                        // Only gain dirt if grass or dirt was dug
                        m_player->dirt(m_player->dirt() + m_player->DIG_DIRT_GAIN);
                        if (m_player->dirt() > m_player->MAX_DIRT)
                            m_player->dirt(m_player->MAX_DIRT);
                    } else if (b.type == 6) {
                        // Gain chaos when chaos block removed
                        int which = m_gen->whichChaosBlock(glm::vec2(m_targetBlock.x, m_targetBlock.z));
                        m_gen->setChaosBlock(which, false);
                        m_player->chaosLevelUp();
                        m_chaosTimer = MAX_CHAOS_TIMER;
                    }
                }
            } else {
                // Add block of dirt
                // Can't place block intersecting with self
                glm::vec3 newBlock = m_targetBlock + m_targetFace;
                glm::vec3 ppos = m_player->position() + glm::vec3(0.f, 1.f, 0.f);
                glm::vec3 npos = newBlock + glm::vec3(0.5f);
                glm::vec3 dist = glm::abs(npos-ppos);
                if (dist.x >= 1.f || dist.y >= 1.5f || dist.z >= 1.f)
                    if (m_player->place())
                        m_vox->setBlock(newBlock.x, newBlock.y, newBlock.z, CS1972Engine::Voxel::Block{3});
            }
        }
        break;

    default:
        break;
    }
}

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
    /*case Qt::Key_F1:
        if (graphics().camera->mode() == CS1972Engine::Camera::Mode::FIRST_PERSON) graphics().camera->mode(CS1972Engine::Camera::Mode::THIRD_PERSON);
        else graphics().camera->mode(CS1972Engine::Camera::Mode::FIRST_PERSON);
        break;*/
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
