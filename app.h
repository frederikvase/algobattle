#pragma once
#include "ui.h"
#include "agent.h"

#include <vector>

template<class Game>
class App {
public:
    App(std::vector<Agent<Game>> &agents);
    void run();

private:
    Game mGame;
    Ui mUi;

    std::vector<Agent<Game>> mAgents;

    sf::RenderWindow mWindow;
    sf::View mGameView;
    sf::View mUiView;

private:
    int mPlayerOneIndex, mPlayerTwoIndex;
    bool mReleasedButton;
};

template<class Game>
App<Game>::App(std::vector<Agent<Game>> &agents)
    : mGame(), mUi(mGame.getTitle())
    , mAgents(agents)
    , mWindow(sf::VideoMode(Ui::screenSize.x, Ui::screenSize.y), "AlgoBattle", sf::Style::Fullscreen)
    , mGameView(sf::FloatRect({0.f, 0.f}, mGame.getScreenSize()))
    , mUiView(sf::FloatRect({0.f, 0.f}, Ui::screenSize))
    , mPlayerOneIndex(agents.size()), mPlayerTwoIndex(agents.size())
{
    const float fracWidth = mUi.gameViewportSize.x/mUi.screenSize.x;
    const float fracHeight = mUi.gameViewportSize.y/mUi.screenSize.y;
    mGameView.setViewport(sf::FloatRect({0.5f - fracWidth / 2.f, 0.5f - fracHeight / 2.f}, 
                                        {fracWidth, fracHeight}));
    std::vector<std::string> agentNames;
    for (auto agent : agents) {
        agentNames.push_back(agent.getName());
    }
    mUi.init(agentNames);
}

template<class Game>
void App<Game>::run() {
    while (mWindow.isOpen()) {
        sf::Event event;
        while (mWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mWindow.close();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (mReleasedButton) {
                    sf::Vector2f mousePos = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow), mGameView);
                    if (mousePos.x >= 0.f && mousePos.x <= mGame.getScreenSize().x && 
                        mousePos.y >= 0.f && mousePos.y <= mGame.getScreenSize().y) {
                        mGame.handleInput(mousePos);
                    } else if (mUi.handleInput(sf::Vector2f(sf::Mouse::getPosition(mWindow)))) {
                        mGame.reset();
                        mPlayerOneIndex = mUi.getPlayerOne();
                        mPlayerTwoIndex = mUi.getPlayerTwo();
                    }
                }
                mReleasedButton = false;
            }
            else {
                mReleasedButton = true;
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
