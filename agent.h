#pragma once
#include "game.h"
#include <chrono>

class Agent {
public:
    Agent();
    virtual void calculateMove(GameState *gamestate, 
                               std::chrono::seconds remainingTime);
};
