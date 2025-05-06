#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <set>

class Passo {
public: // Required struct and enum
    struct Move {
        int startTile;
        int endTile;
    };

    enum State {
        Running,
        P1Win, P1WinTime,
        P2Win, P2WinTime,
        Defualt
    };

public: // Required methods
    Passo();

    void draw(sf::RenderWindow &window);
    bool handleInput(sf::Vector2f mousePos);
    void reset();

    bool makeMove(Passo::Move move, bool updateVisuals=false);
    Passo::State getGameResult() const;

public:
    std::vector<Passo::Move> getLegalMoves() const;

private:
    void updateBoard();

    void updateTile(int tile);
    void deselect();

private:
    std::array<uint8_t, 25> mTiles;
    std::array<sf::Sprite, 25> mSprites;
    sf::Texture mTileSpritesheet;

    int mSelectedTile;
    std::set<int> mLegalMoves;

public: // Required methods:
    sf::Vector2f getScreenSize() const;
    std::string getTitle() const;
    bool getPlayerTurn() const;

private: // Required attributes
    sf::Vector2f mScreenSize;
    std::string mTitle;

    bool mPlayerTurn;
};
