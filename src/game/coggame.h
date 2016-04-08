#pragma once

#include "initscreen.h"
#include "../engine/game.h"

namespace COG {

class COGGame : public CS1972Engine::Game {
public:
    virtual ~COGGame() { }
    virtual void initGL() override { pushScreen(new InitScreen(this)); }
};

}
