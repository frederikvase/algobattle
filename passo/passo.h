#pragma once
#include "../game.h"
#include <array>
#include <set>

class PassoState : public GameState {
public:
    std::array<uint8_t, 25> tiles;
    bool turn;
};

class Passo : public Game {
public:
    Passo();

    void draw(sf::RenderWindow &window) override;
    void handleInput(sf::Vector2f mousePos) override;
    
    GameState* getGameState() override;

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
};
