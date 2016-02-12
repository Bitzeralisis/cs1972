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
    g.putTexture("win", g.loadTextureFromQRC(":/images/win.png"));
    g.putTexture("lose", g.loadTextureFromQRC(":/images/lose.png"));
    g.putTexture("howto", g.loadTextureFromQRC(":/images/howto.png"));
}
