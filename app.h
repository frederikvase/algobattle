#pragma once
#include "game.h"
#include "ui.h"

class App {
public:
    App(Game &game);
    void run();

private:
    Game &mGame;
    Ui mUi;

    sf::RenderWindow mWindow;
    sf::View mGameView;
    sf::View mUiView;
};
