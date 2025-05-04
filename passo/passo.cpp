#include "passo.h"
#include <iostream>

const float left = 120;
const float top = 120;
const float spacing = -10;

Passo::Passo()
    : Game(sf::Vector2f(2200.f, 2200.f), "Passo")
    , mTiles()
    , mSprites()
    , mTileSpritesheet()
    , mSelectedTile(-1)
    , mLegalMoves()
{
    mTileSpritesheet.loadFromFile("passo/textures/tiles.png");
    mTileSpritesheet.setSmooth(true);

    // Setup starting position
    for (int i = 0; i < 5; i++) {
        mTiles[i] = 3;
        mTiles[i+20] = 2;
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            mSprites[i*5+j] = sf::Sprite(mTileSpritesheet, 
                              sf::IntRect(400 * mTiles[i*5+j], 0, 400, 400));
            mSprites[i*5+j].setPosition(left+(400+spacing)*j, top+(400+spacing)*i);
        }
    }
}

void Passo::draw(sf::RenderWindow &window) {
    for (int i = 0; i < 25; i++) {
        if (i == mSelectedTile) {
            mSprites[i].setColor(sf::Color(150, 150, 150));
        }
        window.draw(mSprites[i]);
    }

    for (auto tile : mLegalMoves) {
        int radius = 80;

        sf::CircleShape dot(radius);
        dot.setFillColor(sf::Color(0, 0, 0, 90));
        dot.setOrigin(radius, radius);

        int x = tile % 5;
        int y = tile / 5;

        dot.setPosition(left + x * (400+spacing) + 200, top + (400+spacing) * y + 200);
        window.draw(dot);
    }
}

// Returns true if the current player has won the game
bool Passo::updateBoard() {
    // Remove islands
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            int tile = 5*y+x;
            if (mTiles[tile] == 1) continue;

            bool island = true;
            for (int i = std::max(y-1,0); i <= std::min(y+1, 4); i++) {
                for (int j = std::max(x-1, 0); j <= std::min(x+1, 4); j++) {
                    if (i*5+j != tile && mTiles[i*5+j] != 1) {
                        island = false;
                    }
                }
            }
            if (island) {
                mTiles[tile] = 1;
                updateTile(tile);
            }
        }
    }

    // Get back rank
    int backRank;
    if (mPlayerTurn) {
        for (int i = 4; i >= 0; i--) {
            int sum = 0;
            for (int j = 0; j < 5; j++) {
                sum += mTiles[i*5+j];
            }
            if (sum == 5) continue;
            backRank = i;
            break;
        }
    } else {
        for (int i = 0; i < 5; i++) {
            int sum = 0;
            for (int j = 0; j < 5; j++) {
                sum += mTiles[i*5+j];
            }
            if (sum == 5) continue;
            backRank = i;
            break;
        }
    }

    for (int i = 0; i < 5; i++) {
        int state = mTiles[backRank*5+i];
        if (state > 1 && (state & 1) == mPlayerTurn) {
            return true;
        }
    }

    return false;
}

void Passo::updateTile(int tile) {
    mSprites[tile].setTextureRect(sf::IntRect(mTiles[tile]*400, 0, 400, 400));
}

void Passo::deselect() {
    if (mSelectedTile == -1) return;
    mSprites[mSelectedTile].setColor(sf::Color::White);

    mSelectedTile = -1;
    mLegalMoves.clear();
}

void Passo::handleInput(sf::Vector2f mousePos) {
    mousePos -= sf::Vector2f(left, top);
    if (mousePos.x < 0.f || mousePos.x > (400.f + spacing) * 5.f ||
        mousePos.y < 0.f || mousePos.y > (400.f + spacing) * 5.f) {
        deselect();
        return;
    }

    int x = mousePos.x / (400.f + spacing);
    int y = mousePos.y / (400.f + spacing);

    int tile = y*5+x;
    if (mLegalMoves.count(tile)) {
        if (!mTiles[tile]) mTiles[tile] = 1;

        mTiles[tile] = (mTiles[tile] << 1) | (mTiles[mSelectedTile] & 1);
        mTiles[mSelectedTile] >>= 1; 

        updateTile(tile);
        updateTile(mSelectedTile);

        deselect();
        mPlayerTurn ^= 1;
        if (updateBoard()) {
            std::cout << "Congratulations " << (mPlayerTurn ? "blue" : "red") << "! You win\n";
        }
        return;
    }

    if (mSelectedTile == tile || mTiles[tile] <= 1 || (mTiles[tile] & 1) != mPlayerTurn) {
        deselect();
        return;
    }

    deselect();
    mSelectedTile = tile;

    // Find legal moves
    for (int i = std::max(0, y-1); i <= std::min(y+1, 4); i++) {
        for (int j = std::max(0, x-1); j <= std::min(x+1, 4); j++) {
            if (5*i+j == tile) continue;
            int state = mTiles[5*i+j];
            if (state != 1 && state < 8) {
                mLegalMoves.insert(5*i+j);
            }
        }
    }
}

GameState* Passo::getGameState() {
    PassoState* state = new PassoState();
    state->tiles = mTiles;
    state->turn = mPlayerTurn;

    return state;
}
