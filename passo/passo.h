#pragma once
#include <vector>
#include <cstdint>
#include <array>

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
        Draw,
        Defualt
    };

public: // Required methods
    Passo();

    void reset();

    bool makeMove(Passo::Move move);
    Passo::State getGameResult() const;

private:
    void updateBoard();

public:
    std::vector<Passo::Move> getLegalMoves() const;
    std::array<uint8_t, 25> tiles;

public: // Required methods:
    bool getPlayerTurn() const;

private: // Required attributes
    bool mPlayerTurn;
};
