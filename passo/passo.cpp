#include "passo.h"

const float tileSize = 165.f;
const float spacing = -10;
const float left = (1000.f - 5 * tileSize - 4 * spacing) / 2.f;
const float top = (1000.f - 5 * tileSize - 4 * spacing) / 2.f;

Passo::Passo()
    : mScreenSize(sf::Vector2f(1000.f, 1000.f))
    , mTitle("Passo")
    , mPlayerTurn(0)
    , mTiles()
    , mSprites()
    , mTileSpritesheet()
    , mSelectedTile()
    , mLegalMoves()
{
    mTileSpritesheet.loadFromFile("passo/textures/tiles.png");
    mTileSpritesheet.setSmooth(true);

    reset();
}

void Passo::reset() {
    deselect();
    mLegalMoves.clear();
    mPlayerTurn = 0;

    // Setup starting position
    for (int i = 0; i < 5; i++) {
        mTiles[i] = 3;
        mTiles[i+20] = 2;
    }
    for (int i = 5; i < 20; i++) {
        mTiles[i] = 0;
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            mSprites[i*5+j] = sf::Sprite(mTileSpritesheet, 
                              sf::IntRect(400 * mTiles[i*5+j], 0, 400, 400));
            mSprites[i*5+j].setScale(sf::Vector2f(tileSize / 400.f, tileSize / 400.f));
            mSprites[i*5+j].setPosition(left+(tileSize+spacing)*j, top+(tileSize+spacing)*i);
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

void Passo::updateBoard() {
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

bool Passo::handleInput(sf::Vector2f mousePos) {
    mousePos -= sf::Vector2f(left, top);
    if (mousePos.x < 0.f || mousePos.x > (tileSize + spacing) * 5.f ||
        mousePos.y < 0.f || mousePos.y > (tileSize + spacing) * 5.f) {
        deselect();
        return false;
    }

    int x = mousePos.x / (tileSize + spacing);
    int y = mousePos.y / (tileSize + spacing);

    int tile = y*5+x;
    if (mLegalMoves.count(tile)) {
        makeMove({mSelectedTile, tile}, true);
        deselect();
        return true;
    }

    if (mSelectedTile == tile || mTiles[tile] <= 1 || (mTiles[tile] & 1) != mPlayerTurn) {
        deselect();
        return false;
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
    return false;
}

bool Passo::makeMove(Passo::Move move, bool updateVisuals) {
    if (mTiles[move.startTile] <= 1 || (mTiles[move.startTile]&1)!=mPlayerTurn ||
        mTiles[move.endTile] == 1 || mTiles[move.endTile] >= 8) {
        return false;
    }

    if (std::max(std::abs(move.startTile / 5 - move.endTile / 5), 
                 std::abs(move.startTile % 5 - move.endTile % 5)) != 1) {
        return false;
    }

    if (!mTiles[move.endTile]) mTiles[move.endTile] = 1;
    mTiles[move.endTile] = (mTiles[move.endTile] << 1) | (mTiles[move.startTile] & 1);
    mTiles[move.startTile] >>= 1;

    if (updateVisuals) {
        updateTile(move.startTile);
        updateTile(move.endTile);
        updateBoard();
    }
    mPlayerTurn ^= 1;

    return true;
}

Passo::State Passo::getGameResult() const {
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
            return mPlayerTurn ? Passo::State::P2Win : Passo::State::P1Win;
        }
    }

    if (getLegalMoves().size() == 0) {
        return mPlayerTurn ? Passo::State::P1Win : Passo::State::P2Win;
    }

    return Passo::State::Running;
}

std::vector<Passo::Move> Passo::getLegalMoves() const {
    std::vector<Passo::Move> ret;
    for (int i = 0; i < 25; i++) {
        if (mTiles[i] <= 1 || (mTiles[i]&1) != mPlayerTurn) continue;
        for (int j = 0; j < 25; j++) {
            if (mTiles[j] == 1 || mTiles[j] >= 8) continue;
            if (std::max(std::abs(i / 5 - j / 5), std::abs(i % 5 - j % 5)) == 1) {
                ret.push_back({i, j});
            }
        }
    }
    return ret;
}

sf::Vector2f Passo::getScreenSize() const {
    return mScreenSize;
}

std::string Passo::getTitle() const {
    return mTitle;
}

bool Passo::getPlayerTurn() const {
    return mPlayerTurn;
}
