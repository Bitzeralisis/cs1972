#pragma once

#include "gamescreen.h"
#include "../engine/game.h"

namespace COG {

class COGGame : public CS1972Engine::Game {
public:
    virtual ~COGGame() { }
    virtual void initGL() override { pushScreen(new GameScreen(this)); }

private:
    float m_beat;

public:
    VALACC_MUT(float,beat)
};

}
