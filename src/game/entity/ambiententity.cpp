#include "ambiententity.h"
#include "game/coggame.h"
#include "game/gamescreen.h"
#include "engine/primitive.h"
#include "engine/graphics/deferredmodule.h"
#include "engine/graphics/shadermodule.h"

using namespace COG;

AmbientEntity::AmbientEntity(float beat)
    : COGEntity(beat)
{ }

void AmbientEntity::tickBeats(float beats) {
    while (m_position.x < -18.f)
        m_position.x += 18.f;
}

void AmbientEntity::draw(int pass, float beat) {
    switch (pass) {
    case GameScreen::DRAW_GEOMETRY: {
        float brightness = 0.2f*(1.f - 1.f*glm::mod(beat, 1.f));
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("cube2");
        graphics().shader()->color(glm::vec4(1.f));
        graphics().deferred()->useGlowTexture(true);
        graphics().deferred()->bindGlowTexture("cube1");
        graphics().deferred()->glowColor(glm::vec4(brightness));
        for (float x = -4.f; x <= -4.f; x += 1.f)
            for (float y = -6.f; y <= 6.f; y += 1.2f)
                for (float z = -10.f; z <= 60.f; z += 1.2f) {
                    graphics().shader()->mTransform(glm::translate(glm::mat4(1.f), glm::vec3(m_position.x+z, x, y)));
                    graphics().pBox()->drawArray();
                }

        /*
        graphics().shader()->useTexture(true);
        graphics().shader()->bindTexture("cube2");
        graphics().deferred()->bindGlowTexture("cube1");
        float brightness = 0.2f*(1.f - 1.f*glm::mod(beat, 1.f));
        for (int x = -4; x < 4; ++x)
            for (int y = -4; y < 4; ++y)
                for (int i = -1; i < 12; ++i) {
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
                        graphics().deferred()->glowColor(glm::vec4(brightness));
                    }
                    graphics().shader()->mTransform(glm::translate(glm::mat4(1.f), glm::vec3(m_position.x+6.f*i, 6.f*x+3.f, 6.f*y+3.f)));
                    graphics().pBox()->drawArray();
                }
        */
    }

    case GameScreen::DRAW_LIGHTS: {
        graphics().deferred()->lightDirectional(glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.15f, 0.15f, 0.1f));
        /*
        int x = 1;
        int y = 1;
        for (int i = -06; i < 12; i += 3) {
            graphics().deferred()->lightPoint(glm::vec3(m_position.x+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(0.f, 0.f, 2.3f), 5.f);
        }
        x = -2;
        y = -2;
        for (int i = -06; i < 12; i += 3) {
            graphics().deferred()->lightPoint(glm::vec3(m_position.x+6.f*i, 6.f*x+3.f, 6.f*y+3.f), glm::vec3(2.3f, 0.f, 0.f), 5.f);
        }
        */
    }
    }
}
