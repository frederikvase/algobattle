#include "randomagent.h"

void RandomAgent::calculateMove(GameState* _state, std::chrono::seconds remainingTime) {
    PassoState* state = dynamic_cast<PassoState*>(_state);
    if (!state) return;

    delete state;
    delete _state;
}
