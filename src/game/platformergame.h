#pragma once

#include "../engine/game.h"

namespace Platformer {

class PlatformerGame : public CS1972Engine::Game {
public:
    PlatformerGame() { }
    virtual ~PlatformerGame() { }

public:
    virtual void initGL();
};

}
