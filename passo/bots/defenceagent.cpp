#include "defenceagent.h"
#include <random>

DefenceAgent::DefenceAgent() 
    : Agent<Passo>("DefenceAgent")
{ }

Passo::Move DefenceAgent::calculateMove(const Passo &game) {
    std::random_device rd;
    std::mt19937 mt(rd());

    std::vector<Passo::Move> legalMoves = game.getLegalMoves();
    std::vector<Passo::Move> goodMoves;
    Passo::Move badMove;
    for (auto move : legalMoves) {
        Passo copy = game;
        copy.makeMove(move);

        if (copy.getGameResult() == (game.getPlayerTurn() 
            ? Passo::State::P2Win : Passo::State::P1Win)) {
            return move;
        }
        if (copy.getGameResult() == Passo::State::Running) {
            goodMoves.push_back(move);
        } else {
            badMove = move;
        }
    }

    if (goodMoves.empty()) {
        return badMove;
    }
    return goodMoves[mt() % goodMoves.size()];
}
