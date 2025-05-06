#include "slowrandom.h"
#include <vector>
#include <random>

#include <unistd.h>

SlowRandom::SlowRandom() 
    : Agent<Passo>("Slow-Random")
{ }

Passo::Move SlowRandom::calculateMove(const Passo &game) {
    std::random_device rd;
    std::mt19937 mt(rd());

    std::vector<Passo::Move> legalMoves = game.getLegalMoves();
    sleep(1);

    return legalMoves[mt() % legalMoves.size()];
}
