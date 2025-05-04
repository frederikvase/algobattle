#include "randomagent.h"

RandomAgent::RandomAgent() 
    : Agent<Passo>("Random-Agent")
{ }

Passo::Move RandomAgent::calculateMove(const Passo &game) {
    return Passo::Move();
}
