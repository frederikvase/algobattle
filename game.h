#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameState {
    // There has got to be a better way to do this
    virtual void _poly() {}
};

class Game {
public:
    Game(sf::Vector2f size, std::string title);

    virtual void draw(sf::RenderWindow &window);
    virtual void handleInput(sf::Vector2f mousePos);

    virtual GameState* getGameState();

    sf::Vector2f getScreenSize() const;
    std::string getTitle() const;

    bool getPlayerTurn() const;

protected:
    sf::Vector2f mScreenSize;
    std::string mTitle;

    bool mPlayerTurn;
};
