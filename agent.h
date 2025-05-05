#pragma once
#include <string>

template<class Game> 
class Agent {
public:
    Agent(std::string name);

    virtual typename Game::Move calculateMove(const Game &game);
    std::string getName();

private:
    std::string mName;
};

template<class Game>
Agent<Game>::Agent(std::string name)
    : mName(name) {}

template<class Game>
typename Game::Move Agent<Game>::calculateMove(const Game &game) {
    return {}; // Override
}

template<class Game>
std::string Agent<Game>::getName() {
    return mName;
}
