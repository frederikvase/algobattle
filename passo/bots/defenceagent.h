#pragma once
#include "../../agent.h"
#include "../passo.h"

class DefenceAgent : public Agent<Passo> {
public:
    DefenceAgent();
    Passo::Move calculateMove(const Passo &game) override;
};
