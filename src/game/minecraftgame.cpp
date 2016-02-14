#include "minecraftgame.h"
#include "gamescreen.h"

using namespace Minecraft;

MinecraftGame::MinecraftGame() {
}

MinecraftGame::~MinecraftGame() {
    for (CS1972Engine::Screen *s = popScreen(); s != 0; s = popScreen())
        delete s;
}

void MinecraftGame::initGL() {
    pushScreen(new GameScreen(this));
}
