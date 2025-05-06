#include "randomagent.h"
#include <random>

RandomAgent::RandomAgent() 
    : Agent<Passo>("Random-Agent")
{ }

Passo::Move RandomAgent::calculateMove(const Passo &game) {
    std::random_device rd;
    std::mt19937 mt(rd());

    std::vector<Passo::Move> legalMoves = game.getLegalMoves();

    return legalMoves[mt() % legalMoves.size()];
}
