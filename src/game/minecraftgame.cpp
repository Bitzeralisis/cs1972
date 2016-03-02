#include "minecraftgame.h"
#include "mainmenu.h"

using namespace Minecraft;

MinecraftGame::MinecraftGame() {
}

MinecraftGame::~MinecraftGame() {
    for (CS1972Engine::Screen *s = popScreen(); s != 0; s = popScreen())
        delete s;
}

void MinecraftGame::initGL() {
    g.putTexture("atlas", g.loadTextureFromQRC(":/images/terrain.png"));
    g.putTexture("win", g.loadTextureFromQRC(":/images/win.png"));
    g.putTexture("lose", g.loadTextureFromQRC(":/images/lose.png"));
    g.putTexture("chaostime", g.loadTextureFromQRC(":/images/chaostime.png"));
    g.putTexture("howto1", g.loadTextureFromQRC(":/images/howto1.png"));
    g.putTexture("howto2", g.loadTextureFromQRC(":/images/howto2.png"));

    pushScreen(new MainMenu(this));
}
