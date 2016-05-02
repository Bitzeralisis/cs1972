#include "coggame.h"
#include "gamescreen.h"
#include "engine/primitive.h"

using namespace COG;

COGGame::COGGame() { }

COGGame::~COGGame() { }

void COGGame::initGL() {
    GLfloat *circleLoop = new GLfloat[60*8];
    for (int i = 0; i < 60; ++i) {
        float d = glm::pi<float>() * i / 15.f;
        circleLoop[8*i+0] = glm::sin(d);
        circleLoop[8*i+1] = -1.f*glm::cos(d);
        circleLoop[8*i+2] = 0.f;
        circleLoop[8*i+3] = 0.f;
        circleLoop[8*i+4] = 0.f;
        circleLoop[8*i+5] = -1.f;
    }
    g.putPrimitive("uis_circleLoop", new CS1972Engine::Primitive(60, 60*8*sizeof(GLfloat), GL_LINE_LOOP, circleLoop, 0));
    delete[] circleLoop;

    GLfloat cubeLinesData[] = {
         .5f, .5f, .5f, 0.f, 1.f, 1.f, 0.f,0.f,
        -.5f, .5f, .5f, 0.f, 1.f, 1.f, 1.f,1.f,
         .5f, .5f, .5f, 1.f, 0.f, 1.f, 0.f,0.f,
         .5f,-.5f, .5f, 1.f, 0.f, 1.f, 1.f,1.f,
         .5f, .5f, .5f, 1.f, 1.f, 0.f, 0.f,0.f,
         .5f, .5f,-.5f, 1.f, 1.f, 0.f, 1.f,1.f,
        -.5f, .5f, .5f,-1.f, 0.f, 1.f, 0.f,0.f,
        -.5f,-.5f, .5f,-1.f, 0.f, 1.f, 1.f,1.f,
        -.5f, .5f, .5f,-1.f, 1.f, 0.f, 0.f,0.f,
        -.5f, .5f,-.5f,-1.f, 1.f, 0.f, 1.f,1.f,
         .5f,-.5f, .5f, 0.f,-1.f, 1.f, 0.f,0.f,
        -.5f,-.5f, .5f, 0.f,-1.f, 1.f, 1.f,1.f,
         .5f,-.5f, .5f, 1.f,-1.f, 0.f, 0.f,0.f,
         .5f,-.5f,-.5f, 1.f,-1.f, 0.f, 1.f,1.f,
         .5f, .5f,-.5f, 0.f, 1.f,-1.f, 0.f,0.f,
        -.5f, .5f,-.5f, 0.f, 1.f,-1.f, 1.f,1.f,
         .5f, .5f,-.5f, 1.f, 0.f,-1.f, 0.f,0.f,
         .5f,-.5f,-.5f, 1.f, 0.f,-1.f, 1.f,1.f,
        -.5f,-.5f, .5f,-1.f,-1.f, 0.f, 0.f,0.f,
        -.5f,-.5f,-.5f,-1.f,-1.f, 0.f, 1.f,1.f,
        -.5f, .5f,-.5f,-1.f, 0.f,-1.f, 0.f,0.f,
        -.5f,-.5f,-.5f,-1.f, 0.f,-1.f, 1.f,1.f,
         .5f,-.5f,-.5f, 0.f,-1.f,-1.f, 0.f,0.f,
        -.5f,-.5f,-.5f, 0.f,-1.f,-1.f, 1.f,1.f,
    };
    g.putPrimitive("cubeLines", new CS1972Engine::Primitive(24, sizeof(cubeLinesData), GL_LINES, cubeLinesData, 0));

    GLfloat diamondData[] = {
         .5f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f,0.f,
         0.f, .5f, 0.f, 1.f, 1.f, 1.f, 0.f,1.f,
         0.f, 0.f, .5f, 1.f, 1.f, 1.f, 0.f,0.f,
         .5f, 0.f, 0.f, 1.f, 1.f,-1.f, 1.f,0.f,
         0.f, 0.f,-.5f, 1.f, 1.f,-1.f, 0.f,0.f,
         0.f, .5f, 0.f, 1.f, 1.f,-1.f, 0.f,1.f,
         .5f, 0.f, 0.f, 1.f,-1.f,-1.f, 1.f,0.f,
         0.f,-.5f, 0.f, 1.f,-1.f,-1.f, 0.f,1.f,
         0.f, 0.f,-.5f, 1.f,-1.f,-1.f, 0.f,0.f,
         .5f, 0.f, 0.f, 1.f,-1.f, 1.f, 1.f,0.f,
         0.f, 0.f, .5f, 1.f,-1.f, 1.f, 0.f,0.f,
         0.f,-.5f, 0.f, 1.f,-1.f, 1.f, 0.f,1.f,
        -.5f, 0.f, 0.f,-1.f, 1.f, 1.f, 1.f,0.f,
         0.f, 0.f, .5f,-1.f, 1.f, 1.f, 0.f,0.f,
         0.f, .5f, 0.f,-1.f, 1.f, 1.f, 0.f,1.f,
        -.5f, 0.f, 0.f,-1.f, 1.f,-1.f, 1.f,0.f,
         0.f, .5f, 0.f,-1.f, 1.f,-1.f, 0.f,1.f,
         0.f, 0.f,-.5f,-1.f, 1.f,-1.f, 0.f,0.f,
        -.5f, 0.f, 0.f,-1.f,-1.f,-1.f, 1.f,0.f,
         0.f, 0.f,-.5f,-1.f,-1.f,-1.f, 0.f,0.f,
         0.f,-.5f, 0.f,-1.f,-1.f,-1.f, 0.f,1.f,
        -.5f, 0.f, 0.f,-1.f,-1.f, 1.f, 1.f,0.f,
         0.f,-.5f, 0.f,-1.f,-1.f, 1.f, 0.f,1.f,
         0.f, 0.f, .5f,-1.f,-1.f, 1.f, 0.f,0.f,
    };
    g.putPrimitive("diamond", new CS1972Engine::Primitive(24, sizeof(diamondData), diamondData));

    pushScreen(new GameScreen(this));
}
