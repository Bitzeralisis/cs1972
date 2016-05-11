#include "ambiententity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "game/entity/playerentity.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"

using namespace COG;

AmbientEntity::AmbientEntity(float beat)
    : ControlledEntity(beat)
{ }

void AmbientEntity::tickBeats(float beats) {
    if (m_doFade) {
        m_fade += 0.35f*beats;
        if (m_fade > 0.7f)
            m_fade = 0.7f;
    } else {
        m_fade -= 0.35f*beats;
        if (m_fade < 0.f)
            m_fade = 0.f;
    }

    while (m_position.x < 0.f)
        m_position.x += 360.f;
}

void AmbientEntity::draw(int pass, float beat) {
    float fading = 1.f-m_fade;

    switch (m_zone) {
    case 0:
        switch (pass) {
        case GameScreen::DRAW_GEOMETRY: {
            graphics().shader()->useFog(false);
            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("skybox");
            graphics().deferred()->glowColor(glm::vec4(0.15f, 0.15f, 0.1f, 1.f));
            graphics().shader()->mTransform(glm::scale(glm::mat4(1.f), 0.99f*glm::vec3(GAME->controller()->maxFog())));
            graphics().getPrimitive("skybox")->drawArray();
            graphics().shader()->useFog(true);

            float basex = glm::mod(m_position.x, 18.f) - 18.f;
            float brightness = 0.2f*(1.f - 1.f*glm::mod(beat, 1.f));
            graphics().shader()->useTexture(true);
            graphics().shader()->bindTexture("cube2");
            graphics().shader()->color(fading*glm::vec4(1.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("cube1");
            graphics().deferred()->glowColor(fading*glm::vec4(brightness));
            for (float x = -4.f; x <= -4.f; x += 1.f)
                for (float y = -6.f; y <= 6.f; y += 1.2f)
                    for (float z = -10.f; z <= 60.f; z += 1.2f) {
                        graphics().shader()->mTransform(glm::translate(glm::mat4(1.f), glm::vec3(basex+z, x, y)));
                        graphics().pBox()->drawArray();
                    }
            break;
        }

        case GameScreen::DRAW_LIGHTS:
            graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.15f, 0.15f, 0.1f));
            break;
        }
        break;

    case 1:
        switch (pass) {
        case GameScreen::DRAW_GEOMETRY: {
            graphics().shader()->useFog(false);
            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("skybox");
            graphics().deferred()->glowColor(glm::vec4(0.3f, 0.3f, 0.2f, 1.f));
            graphics().shader()->mTransform(glm::scale(glm::mat4(1.f), 0.99f*glm::vec3(GAME->controller()->maxFog())));
            graphics().getPrimitive("skybox")->drawArray();
            graphics().shader()->useFog(true);

            float basex = glm::mod(m_position.x, 18.f) - 18.f;
            float brightness = 0.4f*(1.f - 1.f*glm::mod(beat, 1.f));
            graphics().shader()->useTexture(true);
            graphics().shader()->bindTexture("cube2");
            graphics().shader()->color(fading*glm::vec4(1.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("cube1");
            graphics().deferred()->glowColor(fading*glm::vec4(brightness));
            for (float y = -4.f; y <= -1.f; y += 0.85f)
                for (float x = -10.f; x <= 60.f; x += 1.2f) {
                    glm::mat4 m(1.f);
                    m = glm::translate(m, glm::vec3(basex+x, y, -6.7f-y));
                    m = glm::rotate(m, 0.25f*glm::pi<float>(), glm::vec3(1.f, 0.f, 0.f));
                    graphics().shader()->mTransform(m);
                    graphics().pBox()->drawArray();

                    m = glm::mat4(1.f);
                    m = glm::translate(m, glm::vec3(basex+x, y, 6.7f+y));
                    m = glm::rotate(m, 0.25f*glm::pi<float>(), glm::vec3(1.f, 0.f, 0.f));
                    graphics().shader()->mTransform(m);
                    graphics().pBox()->drawArray();
                }
            break;
        }

        case GameScreen::DRAW_LIGHTS:
            graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.3f, 0.3f, 0.2f));
            break;
        }
        break;

    case 2:
    case 21:
    case 22:
    case 23:
    case 24:
        switch (pass) {
        case GameScreen::DRAW_GEOMETRY: {
            graphics().shader()->useFog(false);
            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("skybox");
            graphics().deferred()->glowColor(glm::vec4(0.3f, 0.3f, 0.2f, 1.f));
            graphics().shader()->mTransform(glm::scale(glm::mat4(1.f), 0.99f*glm::vec3(GAME->controller()->maxFog())));
            graphics().getPrimitive("skybox")->drawArray();
            graphics().shader()->useFog(true);

            float basex = glm::mod(m_position.x, 36.f) - 36.f;
            float brightness = 0.3f*(1.f - 1.f*glm::mod(beat, 1.f));
            if (m_zone == 22) brightness = 0.f;
            else if (m_zone == 21) brightness = (glm::mod(beat, 0.25f) < 0.125f) ? 0.4f : 0.f;
            else if (m_zone != 2) brightness = 0.4f;
            graphics().shader()->useTexture(true);
            graphics().shader()->bindTexture("cube2");
            graphics().shader()->color(fading*glm::vec4(1.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("cube1");
            for (float z = -9.f; z <= 9.f; z += 18.f) {
                if ((m_zone == 23 && z == 9.f) || (m_zone == 24 && z == -9.f))
                    graphics().deferred()->glowColor(glm::vec4(0.f));
                else
                    graphics().deferred()->glowColor(fading*glm::vec4(brightness));
                for (int i = -2; i <= 2; i += 1)
                    for (float x = -10.f; x <= 80.f; x += 1.2f) {
                        float y = i*(1.2f*glm::sin(glm::pi<float>() * x/9.f) + 2.4f);
                        //float z2 = z * (1.f+(2-glm::abs(i))*(0.1f*glm::sin(glm::pi<float>()* x/9.f)));
                        glm::mat4 m(1.f);
                        m = glm::translate(m, glm::vec3(basex+x, y, z));
                        graphics().shader()->mTransform(m);
                        graphics().pBox()->drawArray();
                    }
            }
            break;
        }

        case GameScreen::DRAW_LIGHTS:
            if (m_zone == 23)
                graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, -1.f), glm::vec3(0.3f, 0.1f, 0.0f));
            else if (m_zone == 24)
                graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, 1.f), glm::vec3(0.0f, 0.1f, 0.3f));
            else {
                graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, -1.f), glm::vec3(0.15f, 0.15f, 0.1f));
                graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, 1.f), glm::vec3(0.15f, 0.15f, 0.1f));
            }
            break;
        }
        break;

    case 3: {
        float basex = glm::mod(m_position.x, 24.f) - 24.f;
        switch (pass) {
        case GameScreen::DRAW_GEOMETRY: {
            graphics().shader()->useFog(false);
            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("skybox");
            graphics().deferred()->glowColor(glm::vec4(0.3f, 0.f, 0.3f, 1.f));
            graphics().shader()->mTransform(glm::scale(glm::mat4(1.f), 0.99f*glm::vec3(GAME->controller()->maxFog())));
            graphics().getPrimitive("skybox")->drawArray();
            graphics().shader()->useFog(true);

            float brightness = 0.3f*(1.f - 0.5f*glm::mod(beat, 2.f));
            graphics().shader()->useTexture(true);
            graphics().shader()->bindTexture("cube2");
            graphics().shader()->color(fading*glm::vec4(1.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("cube1");
            graphics().deferred()->glowColor(fading*glm::vec4(brightness));
            for (float z = -9.f; z <= 9.f; z += 18.f)
                for (float y = -1.2f; y <= 1.2f; y += 1.2f)
                    for (float x = -30.f; x <= 54.f; x += 1.2f) {
                        glm::mat4 m(1.f);
                        m = glm::translate(m, glm::vec3(basex+x, y, z));
                        graphics().shader()->mTransform(m);
                        graphics().pBox()->drawArray();
                    }

            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(false);
            for (float x = -24.f; x <= 48.f; x += 24.f) {
                graphics().deferred()->glowColor(fading*glm::vec4(3.3f, 0.2f, 0.2f, 1.f));
                glm::mat4 m(1.f);
                m = glm::translate(m, glm::vec3(basex+x, 0.f, -7.f));
                graphics().shader()->mTransform(m);
                graphics().pBox()->drawArray();

                graphics().deferred()->glowColor(fading*glm::vec4(0.2f, 0.2f, 3.3f, 1.f));
                m = glm::mat4(1.f);
                m = glm::translate(m, glm::vec3(basex+x, 0.f, 7.f));
                graphics().shader()->mTransform(m);
                graphics().pBox()->drawArray();
            }
            break;
        }

        case GameScreen::DRAW_LIGHTS:
            for (float x = -24.f; x <= 72.f; x += 24.f) {
                graphics().deferred()->lightPoint(glm::vec3(basex+x, 0.f, -7.f), glm::vec3(1.f, 0.f, 0.f), 6.f);
                graphics().deferred()->lightPoint(glm::vec3(basex+x, 0.f, 7.f), glm::vec3(0.f, 0.f, 1.f), 6.f);
            }
            break;
        }
        break;
    }

    case 4:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
        switch (pass) {
        case GameScreen::DRAW_GEOMETRY: {
            graphics().shader()->useFog(false);
            graphics().shader()->useTexture(false);
            graphics().shader()->color(glm::vec4(0.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("skybox");
            graphics().deferred()->glowColor(glm::vec4(0.3f, 0.0f, 0.3f, 1.f));
            graphics().shader()->mTransform(glm::scale(glm::mat4(1.f), 0.99f*glm::vec3(GAME->controller()->maxFog())));
            graphics().getPrimitive("skybox")->drawArray();
            graphics().shader()->useFog(true);

            float basex = glm::mod(m_position.x, 36.f) - 36.f;
            float brightness = 0.4f*(1.f - 1.f*glm::mod(beat, 1.f));
            float brightness2 =
                (m_zone == 45) ?
                fading*(1.f - 8.f*glm::mod(beat, 0.125f)) :
                fading*(1.f - 4.f*glm::mod(beat, 0.25f));
            graphics().shader()->useTexture(true);
            graphics().shader()->bindTexture("cube2");
            graphics().shader()->color(fading*glm::vec4(1.f));
            graphics().deferred()->useGlowTexture(true);
            graphics().deferred()->bindGlowTexture("cube1");
            graphics().deferred()->glowColor(fading*glm::vec4(brightness));
            if (m_zone != 4)
                graphics().deferred()->glowColor(glm::vec4(brightness2));
            for (float y = -20.f; y <= 20.f; y += 40.f) {
                for (float z = -12.f; z <= 12.f; z += 12.f) {
                    if (y == 20.f) {
                        if      (m_zone == 41 && z == -12.f) graphics().deferred()->glowColor(glm::vec4(0.5));
                        else if (m_zone == 42 && z ==   0.f) graphics().deferred()->glowColor(glm::vec4(0.5));
                        else if (m_zone == 43 && z ==  12.f) graphics().deferred()->glowColor(glm::vec4(0.5));
                        else if (m_zone == 45)               graphics().deferred()->glowColor(glm::vec4(brightness));
                        else if (m_zone != 4)                graphics().deferred()->glowColor(glm::vec4(0.f));
                    }
                    for (float x = -12.f; x <= 100.f; x += 1.2f) {
                        glm::mat4 m(1.f);
                        m = glm::translate(m, glm::vec3(basex+x, y, z));
                        graphics().shader()->mTransform(m);
                        graphics().pBox()->drawArray();
                    }
                }
            }

            for (float z = -20.f; z <= 20.f; z += 40.f) {
                for (float y = -10.f; y <= 10.f; y += 20.f) {
                    if (m_zone != 4) {
                        int hit =
                            (m_zone == 45) ?
                            glm::mod(beat, 0.25f)*16.f :
                            glm::mod(beat, 1.f)*4.f;
                        if ((hit == 0 && y > 0.f && z < 0.f) ||
                            (hit == 1 && y < 0.f && z > 0.f) ||
                            (hit == 2 && y < 0.f && z < 0.f) ||
                            (hit == 3 && y > 0.f && z > 0.f))
                            graphics().deferred()->glowColor(glm::vec4(fading));
                        else
                            graphics().deferred()->glowColor(glm::vec4(0.f));
                    }
                    for (float x = -12.f; x <= 100.f; x += 2.4f) {
                        glm::mat4 m(1.f);
                        m = glm::translate(m, glm::vec3(basex+x, y, z));
                        m = glm::rotate(m, x*glm::normalize(y*z)*glm::pi<float>()/36.f, glm::vec3(-1.f, 0.f, 0.f));
                        graphics().shader()->mTransform(m);
                        graphics().pBox()->drawArray();

                        graphics().shader()->mTransform(glm::translate(m, glm::vec3(0.f, 1.2f, 0.f)));
                        graphics().pBox()->drawArray();
                        graphics().shader()->mTransform(glm::translate(m, glm::vec3(0.f, -1.2f, 0.f)));
                        graphics().pBox()->drawArray();
                    }
                }
            }
            break;
        }

        case GameScreen::DRAW_LIGHTS:
            graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, -1.f), glm::vec3(0.5f, 0.1f, 0.0f));
            graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, 1.f), glm::vec3(0.0f, 0.1f, 0.5f));
            break;
        }
        break;

    case 5: {
        float basex = glm::mod(m_position.x, 18.f) - 18.f;
        switch (pass) {
        case GameScreen::DRAW_GEOMETRY: {
            graphics().shader()->useTexture(true);
            graphics().shader()->bindTexture("cube2");
            graphics().deferred()->bindGlowTexture("cube1");
            float brightness = 0.2f*(1.f - 0.5f*glm::mod(beat, 2.f));
            for (int x = -4; x < 4; ++x)
                for (int y = -4; y < 4; ++y)
                    for (int i = -1; i < 12; ++i) {
                        if (x == -2 && y == -2 && (i % 3 == 0)) {
                            graphics().shader()->color(glm::vec4(0.f));
                            graphics().deferred()->useGlowTexture(false);
                            graphics().deferred()->glowColor(fading*glm::vec4(3.f, 0.1f, 0.1f, 1.f));
                        } else if (x == 1 && y == 1 && (i % 3 == 0)) {
                            graphics().shader()->color(glm::vec4(0.f));
                            graphics().deferred()->useGlowTexture(false);
                            graphics().deferred()->glowColor(fading*glm::vec4(0.1f, 0.1f, 3.f, 1.f));
                        } else {
                            graphics().shader()->color(fading*glm::vec4(1.f));
                            graphics().deferred()->useGlowTexture(true);
                            graphics().deferred()->glowColor(fading*glm::vec4(brightness));
                        }
                        graphics().shader()->mTransform(glm::translate(glm::mat4(1.f), glm::vec3(basex+6.f*i, 6.f*x+3.f, 6.f*y+3.f)));
                        graphics().pBox()->drawArray();
                    }
            break;
        }

        case GameScreen::DRAW_LIGHTS: {
            int x = 1;
            int y = 1;
            for (int i = -06; i < 12; i += 3) {
                graphics().deferred()->lightPoint(glm::vec3(basex+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(0.f, 0.f, 2.3f), 5.f);
            }
            x = -2;
            y = -2;
            for (int i = -06; i < 12; i += 3) {
                graphics().deferred()->lightPoint(glm::vec3(basex+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(2.3f, 0.f, 0.f), 5.f);
            }
            break;
        }
        }
    }
    }
}
