#pragma once
#include "../../agent.h"
#include "../passo.h"

class SlowRandom : public Agent<Passo> {
public:
    SlowRandom();
    Passo::Move calculateMove(const Passo &game) override;
};

