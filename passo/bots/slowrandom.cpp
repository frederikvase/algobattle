#include "slowrandom.h"
#include <random>
#include <unistd.h>

SlowRandom::SlowRandom() 
    : Agent<Passo>("Slow-Random")
{ }

Passo::Move SlowRandom::calculateMove(const Passo &game) { // Passo class has some GUI
    std::random_device rd;
    std::mt19937 mt(rd());

    std::vector<Passo::Move> legalMoves = game.getLegalMoves(); // Const function
    sleep(1);

    return legalMoves[mt() % legalMoves.size()];

    // Simulate segmentation fault
    //return legalMoves[-1];
}
