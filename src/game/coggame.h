#pragma once

#include "engine/game.h"

namespace COG {

class PlayerEntity;

class COGGame : public CS1972Engine::Game {
public:
    virtual ~COGGame();
    virtual void initGL() override;

private:
    float m_beat;
    PlayerEntity *m_controller;

public:
    VALACC_MUT(float,beat)
    VALACC_MUT(PlayerEntity *,controller)
};

}
