#include "platformergame.h"
#include "mainmenuscreen.h"

using namespace Platformer;

void PlatformerGame::initGL() {
    g.putTexture("mainmenu", g.loadTextureFromQRC(":/images/mainmenu.png"));

    pushScreen(new MainMenuScreen(this));
}
