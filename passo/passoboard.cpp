#include "passoboard.h"

const float tileSize = 165.f;
const float spacing = -10;
const float left = (1000.f - 5 * tileSize - 4 * spacing) / 2.f;
const float top = (1000.f - 5 * tileSize - 4 * spacing) / 2.f;

PassoBoard::PassoBoard()
    : mScreenSize(sf::Vector2f(1000.f, 1000.f))
    , mTitle("Passo")
    , mSprites()
    , mTileSpritesheet()
    , mSelectedTile()
    , mLegalMoves()
{
    mTileSpritesheet.loadFromFile("passo/textures/tiles.png");
    mTileSpritesheet.setSmooth(true);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            mSprites[i*5+j] = sf::Sprite(mTileSpritesheet, 
                              sf::IntRect(0, 0, 400, 400));
            mSprites[i*5+j].setScale(sf::Vector2f(tileSize / 400.f, tileSize / 400.f));
            mSprites[i*5+j].setPosition(left+(tileSize+spacing)*j, top+(tileSize+spacing)*i);
        }
    }
    reset();
}

void PassoBoard::reset() {
    deselect();
}

void PassoBoard::deselect() {
    if (mSelectedTile == -1) return;
    mSprites[mSelectedTile].setColor(sf::Color::White);

    mSelectedTile = -1;
    mLegalMoves.clear();
}

void PassoBoard::draw(sf::RenderWindow &window, const Passo &game) {
    for (int i = 0; i < 25; i++) {
        if (i == mSelectedTile) {
            mSprites[i].setColor(sf::Color(150, 150, 150));
        }
        mSprites[i].setTextureRect(sf::IntRect(game.tiles[i]*400, 0, 400, 400));
        window.draw(mSprites[i]);
    }

    for (auto tile : mLegalMoves) {
        float radius = tileSize * 0.2f;

        sf::CircleShape dot(radius);
        dot.setFillColor(sf::Color(0, 0, 0, 90));
        dot.setOrigin(radius, radius);

        int x = tile % 5;
        int y = tile / 5;

        dot.setPosition(left + x * (tileSize+spacing) + tileSize/2.f, top + (tileSize+spacing) * y + tileSize/2.f);
        window.draw(dot);
    }
}

std::optional<Passo::Move> PassoBoard::handleInput(sf::Vector2f mousePos, const Passo &game) {
    mousePos -= sf::Vector2f(left, top);
    if (mousePos.x < 0.f || mousePos.x > (tileSize + spacing) * 5.f ||
        mousePos.y < 0.f || mousePos.y > (tileSize + spacing) * 5.f) {
        deselect();
        return {};
    }

    int x = mousePos.x / (tileSize + spacing);
    int y = mousePos.y / (tileSize + spacing);

    int tile = y*5+x;
    if (mLegalMoves.count(tile)) {
        Passo::Move move = {mSelectedTile, tile};
        deselect();
        return move;
    }

    if (mSelectedTile == tile || game.tiles[tile] <= 1 || (game.tiles[tile] & 1) != game.getPlayerTurn()) {
        deselect();
        return {};
    }

    deselect();
    mSelectedTile = tile;

    // Find legal moves
    for (int i = std::max(0, y-1); i <= std::min(y+1, 4); i++) {
        for (int j = std::max(0, x-1); j <= std::min(x+1, 4); j++) {
            if (5*i+j == tile) continue;
            int state = game.tiles[5*i+j];
            if (state != 1 && state < 8) {
                mLegalMoves.insert(5*i+j);
            }
        }
    }
    return {};
}

sf::Vector2f PassoBoard::getScreenSize() const {
    return mScreenSize;
}

std::string PassoBoard::getTitle() const {
    return mTitle;
}
