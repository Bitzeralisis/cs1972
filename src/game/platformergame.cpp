#include "platformergame.h"
#include "mainmenuscreen.h"
#include "../engine/primitive.h"

using namespace Platformer;

void PlatformerGame::initGL() {
    g.putTexture("mainmenu", g.loadTextureFromQRC(":/images/mainmenu.png"));
    g.putTexture("win", g.loadTextureFromQRC(":/images/win.png"));
    g.putTexture("lose", g.loadTextureFromQRC(":/images/lose.png"));

    {
        GLfloat triangle[24] = {
            0.f, 1.f,0.f, 0.f,0.f,-1.f, 0.f, 0.f,
            1.f, 1.f,0.f, 0.f,0.f,-1.f, 0.f, 1.f,
            0.5f,0.f,0.f, 0.f,0.f,-1.f, 0.5f,1.f,
        };
        g.putPrimitive("uis_triangle", new CS1972Engine::Primitive(3, sizeof(triangle), triangle));

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
        g.putPrimitive("uis_circleLoop", new CS1972Engine::Primitive(360, sizeof(circleLoop), circleLoop));
        g.putPrimitive("uis_ringStrip", new CS1972Engine::Primitive(2*362, sizeof(ringStrip), ringStrip));
    }

    pushScreen(new MainMenuScreen(this));
}
