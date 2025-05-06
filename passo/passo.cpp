#include "passo.h"
#include <math.h>

#include <iostream>

Passo::Passo()
    : mPlayerTurn(0) {
    reset();
}

void Passo::reset() {
    mPlayerTurn = 0;

    // Setup starting position
    for (int i = 0; i < 5; i++) {
        tiles[i] = 3;
        tiles[i+20] = 2;
    }
    for (int i = 5; i < 20; i++) {
        tiles[i] = 0;
    }
}

bool Passo::makeMove(Passo::Move move) {
    if (tiles[move.startTile] <= 1 || (tiles[move.startTile]&1) != mPlayerTurn ||
        tiles[move.endTile] == 1 || tiles[move.endTile] >= 8) {
        return false;
    }

    if (std::max(std::abs(move.startTile / 5 - move.endTile / 5), 
                 std::abs(move.startTile % 5 - move.endTile % 5)) != 1) {
        return false;
    }

    if (!tiles[move.endTile]) tiles[move.endTile] = 1;
    tiles[move.endTile] = (tiles[move.endTile] << 1) | (tiles[move.startTile] & 1);
    tiles[move.startTile] >>= 1;

    updateBoard();
    mPlayerTurn ^= 1;

    return true;
}

Passo::State Passo::getGameResult() const {
    int backRank;
    if (mPlayerTurn) {
        for (int i = 4; i >= 0; i--) {
            int sum = 0;
            for (int j = 0; j < 5; j++) {
                sum += tiles[i*5+j];
            }
            if (sum == 5) continue;
            backRank = i;
            break;
        }
    } else {
        for (int i = 0; i < 5; i++) {
            int sum = 0;
            for (int j = 0; j < 5; j++) {
                sum += tiles[i*5+j];
            }
            if (sum == 5) continue;
            backRank = i;
            break;
        }
    }

    for (int i = 0; i < 5; i++) {
        int state = tiles[backRank*5+i];
        if (state > 1 && (state & 1) == mPlayerTurn) {
            return mPlayerTurn ? Passo::State::P2Win : Passo::State::P1Win;
        }
    }

    if (getLegalMoves().size() == 0) {
        return mPlayerTurn ? Passo::State::P1Win : Passo::State::P2Win;
    }

    return Passo::State::Running;
}

void Passo::updateBoard() {
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            int tile = 5*y+x;
            if (tiles[tile] == 1) continue;

            bool island = true;
            for (int i = std::max(y-1,0); i <= std::min(y+1, 4); i++) {
                for (int j = std::max(x-1, 0); j <= std::min(x+1, 4); j++) {
                    if (i*5+j != tile && tiles[i*5+j] != 1) {
                        island = false;
                    }
                }
            }
            if (island) {
                tiles[tile] = 1;
            }
        }
    }
}

std::vector<Passo::Move> Passo::getLegalMoves() const {
    std::vector<Passo::Move> ret;
    for (int i = 0; i < 25; i++) {
        if (tiles[i] <= 1 || (tiles[i]&1) != mPlayerTurn) continue;
        for (int j = 0; j < 25; j++) {
            if (tiles[j] == 1 || tiles[j] >= 8) continue;
            if (std::max(std::abs(i / 5 - j / 5), std::abs(i % 5 - j % 5)) == 1) {
                ret.push_back({i, j});
            }
        }
    }
    return ret;
}

bool Passo::getPlayerTurn() const {
    return mPlayerTurn;
}
