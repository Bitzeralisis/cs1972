#include "warmupgame.h"
#include "mainmenu.h"
#include "../util/CommonIncludes.h"

WarmupGame::WarmupGame::WarmupGame() {
    pushScreen(new MainMenu());
}

WarmupGame::WarmupGame::~WarmupGame() {

}

void WarmupGame::WarmupGame::initializeGL() {

}
