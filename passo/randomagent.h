#pragma once
#include "../agent.h"
#include "passo.h"

class RandomAgent : public Agent<Passo> {
public:
    RandomAgent();
    Passo::Move calculateMove(const Passo &game) override;
};
