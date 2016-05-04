#include "coggame.h"
#include "mainmenuscreen.h"
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

    GLfloat gateData1[] = {
        0.f,-.5f,-.5f, -1.f,0.f,0.f, 0.f,0.f,
        0.f, .5f, .5f, -1.f,0.f,0.f, .5f,.5f,
        0.f, .5f,-.5f, -1.f,0.f,0.f, 0.f,.5f,
        0.f,-.5f,-.5f, -1.f,0.f,0.f, 0.f,0.f,
        0.f,-.5f, .5f, -1.f,0.f,0.f, .5f,0.f,
        0.f, .5f, .5f, -1.f,0.f,0.f, .5f,.5f,
    };
    g.putPrimitive("gate1", new CS1972Engine::Primitive(6, sizeof(gateData1), gateData1));

    GLfloat gateData2[] = {
        0.f,-.5f,-.5f, -1.f,0.f,0.f, 0.f,.5f,
        0.f, .5f, .5f, -1.f,0.f,0.f, .5f,1.f,
        0.f, .5f,-.5f, -1.f,0.f,0.f, 0.f,1.f,
        0.f,-.5f,-.5f, -1.f,0.f,0.f, 0.f,.5f,
        0.f,-.5f, .5f, -1.f,0.f,0.f, .5f,.5f,
        0.f, .5f, .5f, -1.f,0.f,0.f, .5f,1.f,
    };
    g.putPrimitive("gate2", new CS1972Engine::Primitive(6, sizeof(gateData2), gateData2));

    GLfloat gateData3[] = {
        0.f,-1.f,-.25f, -1.f,0.f,0.f, 0.5f,0.f,
        0.f, 1.f, .25f, -1.f,0.f,0.f, .75f,1.f,
        0.f, 1.f,-.25f, -1.f,0.f,0.f, 0.5f,1.f,
        0.f,-1.f,-.25f, -1.f,0.f,0.f, 0.5f,0.f,
        0.f,-1.f, .25f, -1.f,0.f,0.f, .75f,0.f,
        0.f, 1.f, .25f, -1.f,0.f,0.f, .75f,1.f,
    };
    g.putPrimitive("gate3", new CS1972Engine::Primitive(6, sizeof(gateData3), gateData3));

    GLfloat gateData4[] = {
        0.f,-1.f,-.25f, -1.f,0.f,0.f, .75f,0.f,
        0.f, 1.f, .25f, -1.f,0.f,0.f, 1.0f,1.f,
        0.f, 1.f,-.25f, -1.f,0.f,0.f, .75f,1.f,
        0.f,-1.f,-.25f, -1.f,0.f,0.f, .75f,0.f,
        0.f,-1.f, .25f, -1.f,0.f,0.f, 1.0f,0.f,
        0.f, 1.f, .25f, -1.f,0.f,0.f, 1.0f,1.f,
    };
    g.putPrimitive("gate4", new CS1972Engine::Primitive(6, sizeof(gateData4), gateData4));

    pushScreen(new MainMenuScreen(this));
}
