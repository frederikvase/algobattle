#include "game.h"

Game::Game(sf::Vector2f size, std::string title)
    : mScreenSize(size)
    , mTitle(title)
    , mPlayerTurn(0)
{}

void Game::draw(sf::RenderWindow &window) {
    // Override in other class 
}

void Game::handleInput(sf::Vector2f mousePos) {
    // Override in other class
}

GameState* Game::getGameState() {
    // Override in other class
    return nullptr; 
}

sf::Vector2f Game::getScreenSize() const {
    return mScreenSize;
}

std::string Game::getTitle() const {
    return mTitle;
}

bool Game::getPlayerTurn() const {
    return mPlayerTurn;
}
