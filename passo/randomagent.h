#pragma once
#include "../agent.h"
#include "passo.h"

class RandomAgent : public Agent {
public:
    void calculateMove(GameState *gamestate, std::chrono::seconds remainingTime) override;
};
