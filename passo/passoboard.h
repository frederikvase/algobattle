#pragma once
#include <SFML/Graphics.hpp>
#include "passo.h"

#include <optional>
#include <set>

class PassoBoard {
public:
    PassoBoard();

    void draw(sf::RenderWindow &window, const Passo &game);
    std::optional<Passo::Move> handleInput(sf::Vector2f mousePos, const Passo &game);

    void reset();

private:
    void deselect();

private:
    std::array<sf::Sprite, 25> mSprites;
    sf::Texture mTileSpritesheet;

    int mSelectedTile;
    std::set<int> mLegalMoves;

public: // Required methods:
    sf::Vector2f getScreenSize() const;
    std::string getTitle() const;

private: // Required attributes
    sf::Vector2f mScreenSize;
    std::string mTitle;
};

