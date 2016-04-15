#include "initscreen.h"
#include "entity/lightentity.h"
#include "engine/primitive.h"
#include "engine/sound.h"
#include "engine/world.h"
#include "engine/graphics/bloommodule.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/particlemodule.h"
#include "engine/graphics/shadermodule.h"
#include "engine/graphics/uishadermodule.h"
#include <QKeyEvent>

using namespace COG;

int PARTS_W = 256;
int PARTS_H = 256;

InitScreen::InitScreen(CS1972Engine::Game *parent)
    : Screen(parent)
{
    if (!graphics().hasTexture("cube"))
        graphics().putTexture("cube", graphics().loadTextureFromQRC(":/images/cube.png"));
    if (!graphics().hasTexture("particle"))
        graphics().putTexture("particle", graphics().loadTextureFromQRC(":/images/particle1.png", GL_LINEAR));

    graphics().deferred()->initGbuffer(parent->width(), parent->height());
    graphics().bloom()->initBuffers(parent->width(), parent->height());
    graphics().particle()->init(PARTS_W, PARTS_H);

    m_world = new CS1972Engine::World(parent);

    m_sfx1 = audio().createSoundSample("../cs1972/res/sound/handclap.aif");
    m_sfx2 = audio().createSoundSample("../cs1972/res/sound/808hh08.aif");

    m_bgm = audio().createSoundStream("../cs1972/res/sound/1.mp3");
    m_bgm->setMusicParams(150.f, 0.052245f);
    m_bgm->setLoop(true);
    m_bgm->setLoopBeats(32.f, 96.f);
}

InitScreen::~InitScreen() {
    graphics().deferred()->cleanupGbuffer();
    graphics().bloom()->cleanupBuffers();
    graphics().particle()->cleanup();

    m_world->deleteEntitiesOnDeconstruct(true);

    delete m_bgm;
    delete m_sfx1;
    delete m_sfx2;
}

void InitScreen::tick(float seconds) {
    float beat = audio().getBeat();

    // Sometimes the very first tick will be like 10000 seconds due to all that time being used initializing
    // Always bound the first tick to at most 0.017f
    if (m_firstTick) {
        m_firstTick = false;
        if (seconds > 0.017f)
            seconds = 0.017f;
        audio().queueBgmOnBeat(m_bgm, 0.f);
    }

    // Shoot
    if (m_mouseHeld[0]) {
        if (m_prevHat < beat) {
            m_prevHat = glm::ceil(beat*4.f)/4.f;

            audio().queueSoundOnBeat(m_sfx2, m_prevHat);

            // Generate some particles
            for (int i = 0; i < 1; ++i) {
                float xPos = 8.f * (float) rand() / RAND_MAX - 4.f + 10.f;
                float yPos = 8.f * (float) rand() / RAND_MAX - 4.f;
                float zPos = 8.f * (float) rand() / RAND_MAX - 4.f;
                int numParts = 500.f * (float) rand() / RAND_MAX + 1500.f;
                GLfloat *pos = new GLfloat[4*numParts];
                GLfloat *vel = new GLfloat[3*numParts];
                for (int i = 0; i < numParts; ++i) {
                    pos[4*i+0] = 1.f * (float) rand() / RAND_MAX - 0.5f + xPos;
                    pos[4*i+1] = 1.f * (float) rand() / RAND_MAX - 0.5f + yPos;
                    pos[4*i+2] = 1.f * (float) rand() / RAND_MAX - 0.5f + zPos;
                    pos[4*i+3] = 1.f * (float) rand() / RAND_MAX + 1.f;
                }
                for (int i = 0; i < numParts; ++i) {
                    vel[3*i+0] = 0.8f * (float) rand() / RAND_MAX - 0.4f - 2.f;
                    vel[3*i+1] = 0.8f * (float) rand() / RAND_MAX - 0.4f;
                    vel[3*i+2] = 0.8f * (float) rand() / RAND_MAX - 0.4f;
                }
                graphics().particle()->putParticles(numParts, pos, vel);
                delete pos;
                delete vel;

                m_world->addEntity(new LightEntity(glm::vec3(xPos, yPos, zPos), glm::vec3(-2.f, 0.f, 0.f)));
            }
        }
    }

    m_world->tick(seconds);

    m_timestep += seconds;
    m_time += seconds;
}

int bloom = 0;
int deferred = 0;
int particles = 0;

void InitScreen::draw() {
    float dx = -2.f * glm::mod(m_time, 9.f);

    // Update particles
    graphics().particle()->updateParticles(m_timestep);

    // Set up camera
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
    graphics().shader()->useFog(true, 40.f, 50.f, glm::vec3(0.f));
    graphics().shader()->useTexture(false);
    graphics().shader()->bindTexture("cube");
    graphics().deferred()->bindGlowTexture("cube");
    for (int x = -3; x < 3; ++x)
        for (int y = -3; y < 3; ++y)
            for (int i = 0; i < 20; ++i) {
                if (x == -2 && y == -2 && (i % 3 == 0)) {
                    graphics().shader()->color(glm::vec4(0.f));
                    graphics().deferred()->useGlowTexture(false);
                    graphics().deferred()->glowColor(glm::vec4(3.f, 0.1f, 0.1f, 1.f));
                } else if (x == 1 && y == 1 && (i % 3 == 0)) {
                    graphics().shader()->color(glm::vec4(0.f));
                    graphics().deferred()->useGlowTexture(false);
                    graphics().deferred()->glowColor(glm::vec4(0.1f, 0.1f, 3.f, 1.f));
                } else {
                    graphics().shader()->color(glm::vec4(1.f));
                    graphics().deferred()->useGlowTexture(true);
                    graphics().deferred()->glowColor(glm::vec4(-0.05f));
                }
                graphics().shader()->mTransform(glm::translate(glm::mat4(1.f), glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f)));
                graphics().pBox()->drawArray();
            }
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
        // Render lights in deferred pass
        graphics().deferred()->useDeferredShader();
        graphics().shader()->pvTransformFromCamera();
        int x = 1;
        int y = 1;
        for (int i = -06; i < 12; i += 3) {
            graphics().deferred()->lightPoint(glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(0.f, 0.f, 2.3f), glm::vec3(1.f, 2.f/04.f, 1.f/016.f));
        }
        x = -2;
        y = -2;
        for (int i = -06; i < 12; i += 3) {
            graphics().deferred()->lightPoint(glm::vec3(dx+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(2.3f, 0.f, 0.f), glm::vec3(1.f, 2.f/04.f, 1.f/016.f));
        }
        m_world->draw(2);
        graphics().useShader(0);
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

    m_timestep = 0.f;
}

void InitScreen::mousePressEvent(QMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton:
        m_mouseHeld[0] = true;
        audio().playSound(m_sfx1);
        break;

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

void InitScreen::mouseMoveEvent(QMouseEvent *event) { }

void InitScreen::mouseReleaseEvent(QMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton:
        m_mouseHeld[0] = false;
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

void InitScreen::wheelEvent(QWheelEvent *event) { }

void InitScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
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

void InitScreen::keyReleaseEvent(QKeyEvent *event) { }

void InitScreen::resizeEvent(int w, int h) {
    graphics().deferred()->cleanupGbuffer();
    graphics().bloom()->cleanupBuffers();
    graphics().deferred()->initGbuffer(w, h);
    graphics().bloom()->initBuffers(w, h);
}
