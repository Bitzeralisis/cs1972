#include "coggame.h"
#include "gamescreen.h"
#include "engine/primitive.h"

using namespace COG;

COGGame::~COGGame() { }

void COGGame::initGL() {
    GLfloat circleLoop[30*8];
    for (int i = 0; i < 30; ++i) {
        float d = glm::pi<float>() * i / 15.f;
        circleLoop[8*i+0] = glm::sin(d);
        circleLoop[8*i+1] = -1.f*glm::cos(d);
        circleLoop[8*i+2] = 0.f;
        circleLoop[8*i+3] = 0.f;
        circleLoop[8*i+4] = 0.f;
        circleLoop[8*i+5] = -1.f;
    }
    g.putPrimitive("uis_circleLoop", new CS1972Engine::Primitive(30, sizeof(circleLoop), GL_LINE_LOOP, circleLoop, 0));
    pushScreen(new GameScreen(this));
}
