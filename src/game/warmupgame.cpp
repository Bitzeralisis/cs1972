#include "warmupgame.h"
#include "mainmenu.h"
#include "../util/CommonIncludes.h"

WarmupGame::WarmupGame::WarmupGame() {
    pushScreen(new MainMenu(this));
}

WarmupGame::WarmupGame::~WarmupGame() {

}

void WarmupGame::WarmupGame::initGL() {
    g.putTexture("grass", g.loadTextureFromQRC(":/images/grass.png"));
}
