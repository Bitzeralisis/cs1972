#pragma once

#include "game/enginedef.h"

#ifndef GAME_TYPE
namespace CS1972Engine {

class Game;

}
#define GAME_TYPE CS1972Engine::Game
#endif

extern GAME_TYPE *GAME;
