#pragma once

#include "../engine/game.h"

namespace Minecraft {

class MinecraftGame : public CS1972Engine::Game {
public:
    MinecraftGame();
    virtual ~MinecraftGame();

public:
    virtual void initGL();
};

}
