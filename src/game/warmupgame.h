#pragma once

#include "../engine/game.h"

namespace WarmupGame {

class WarmupGame : public CS1972Engine::Game {
public:
    WarmupGame();
    virtual ~WarmupGame();

public:
    virtual void initGL() override;
};

}
