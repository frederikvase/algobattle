#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <set>

class Passo {
public:
    Passo();

    void draw(sf::RenderWindow &window);
    void handleInput(sf::Vector2f mousePos);

public:
    struct Move {
        int startTile;
        int endTile;
    };

private:
    bool updateBoard();

    void updateTile(int tile);
    void deselect();

private:
    std::array<uint8_t, 25> mTiles;
    std::array<sf::Sprite, 25> mSprites;
    sf::Texture mTileSpritesheet;

    int mSelectedTile;
    std::set<int> mLegalMoves;

    // There functions are required
public:
    sf::Vector2f getScreenSize() const;
    std::string getTitle() const;
    bool getPlayerTurn() const;

private:
    sf::Vector2f mScreenSize;
    std::string mTitle;

    bool mPlayerTurn;
};
