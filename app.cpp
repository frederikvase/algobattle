#include "app.h"

App::App(Game &game) 
    : mGame(game)
    , mUi(game.getTitle())
    , mWindow(sf::VideoMode(Ui::screenSize.x, Ui::screenSize.y), "AlgoBattle", sf::Style::Fullscreen)
    , mGameView(sf::FloatRect({0.f, 0.f}, game.getScreenSize()))
    , mUiView(sf::FloatRect({0.f, 0.f}, Ui::screenSize)) 
{
    const float fracWidth = mUi.gameViewportSize.x/mUi.screenSize.x;
    const float fracHeight = mUi.gameViewportSize.y/mUi.screenSize.y;
    mGameView.setViewport(sf::FloatRect({0.5f - fracWidth / 2.f, 0.5f - fracHeight / 2.f}, 
                                        {fracWidth, fracHeight}));
}

void App::run() {
    while (mWindow.isOpen()) {
        sf::Event event;
        while (mWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mWindow.close();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2f mousePos = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow), mGameView);
                if (mousePos.x >= 0.f && mousePos.x <= mGame.getScreenSize().x && 
                    mousePos.y >= 0.f && mousePos.y <= mGame.getScreenSize().y) {
                    mGame.handleInput(mousePos);
                }
            }
        }

        mWindow.clear(sf::Color::Black);

        mWindow.setView(mGameView);
        mGame.draw(mWindow);

        mWindow.setView(mUiView);
        mUi.draw(mWindow);

        mWindow.display();
    }
}
