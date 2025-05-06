#pragma once
#include "ui.h"
#include "agent.h"

#include <vector>
#include <array>

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <chrono>

template<class Game>
class App {
public:
    App(std::vector<Agent<Game>*> agents);
    void run();

private:
    void handleEvents();

private:
    Game mGame;
    Ui mUi;

    std::vector<Agent<Game>*> mAgents;

    sf::RenderWindow mWindow;
    sf::View mGameView;
    sf::View mUiView;

private:
    std::array<int, 2> mPlayerIndex;
    bool mReleasedButton;

    typename Game::State mGameState;
    pid_t mPid;
};

template<class Game>
App<Game>::App(std::vector<Agent<Game>*> agents)
    : mGame(), mUi(mGame.getTitle())
    , mAgents(agents)
    , mWindow(sf::VideoMode(Ui::screenSize.x, Ui::screenSize.y), "AlgoBattle", sf::Style::Fullscreen)
    , mGameView(sf::FloatRect({0.f, 0.f}, mGame.getScreenSize()))
    , mUiView(sf::FloatRect({0.f, 0.f}, Ui::screenSize))
    , mPlayerIndex({int(agents.size()), int(agents.size())})
    , mGameState(Game::Defualt)
    , mPid()
{
    const float fracWidth = mUi.gameViewportSize.x/mUi.screenSize.x;
    const float fracHeight = mUi.gameViewportSize.y/mUi.screenSize.y;
    mGameView.setViewport(sf::FloatRect({0.5f - fracWidth / 2.f, 0.5f - fracHeight / 2.f}, 
                                        {fracWidth, fracHeight}));
    std::vector<std::string> agentNames;
    for (auto agent : mAgents) {
        agentNames.push_back(agent->getName());
    }
    mUi.init(agentNames);
}

template<class Game>
void App<Game>::run() {
    while (mWindow.isOpen()) {
        handleEvents();

        // Update
        const int turn = mGame.getPlayerTurn();
        if (mPlayerIndex[turn] != int(mAgents.size()) && mGameState == Game::Running) {
            typename Game::Move move = mAgents[mPlayerIndex[turn]]->calculateMove(mGame);
            if (!mGame.makeMove(move, true)) {
                exit(0);
            }
            mGameState = mGame.getGameResult();
        }

        // Draw
        mWindow.clear(sf::Color::Black);

        mWindow.setView(mGameView);
        mGame.draw(mWindow);

        mWindow.setView(mUiView);
        mUi.draw(mWindow);

        auto DrawGameResult = [&](std::string text, sf::Color textColor) {
            sf::RectangleShape rect(sf::Vector2f(0.8f * Ui::gameViewportSize.x, 0.5f * Ui::gameViewportSize.y) );
            rect.setPosition(Ui::screenSize / 2.f);
            rect.setOrigin(rect.getSize() / 2.f);
            rect.setFillColor(sf::Color(0,0,0,120));
            mWindow.draw(rect);

            GUI::Text msg(Ui::screenSize / 2.f, mUi.getFont(), text, 56);
            msg.text.setFillColor(textColor);
            msg.text.setOrigin(msg.text.getOrigin().x, msg.text.getGlobalBounds().height / 2.f);
            msg.text.setStyle(sf::Text::Italic);
            msg.draw(mWindow);
        };
        if (mGameState == Game::State::P1Win) {
            DrawGameResult("Player One won the game!", sf::Color(250, 82, 82));
        } else if (mGameState == Game::State::P2Win) {
            DrawGameResult("Player Two won the game!", sf::Color(34, 139, 230));
        } else if (mGameState == Game::State::P1WinTime) {
            DrawGameResult("Player One won on time!", sf::Color(250, 82, 82));
        } else if (mGameState == Game::State::P2WinTime) {
            DrawGameResult("Player Two won on time!", sf::Color(34, 139, 230));
        }

        mWindow.display();
    }
}

template<class Game>
void App<Game>::handleEvents() {
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
                    if (mPlayerIndex[mGame.getPlayerTurn()] == int(mAgents.size()) && mGameState == Game::Running) {
                        if (mGame.handleInput(mousePos)) {
                            mGameState = mGame.getGameResult();
                        }
                    }
                } else if (mUi.handleInput(sf::Vector2f(sf::Mouse::getPosition(mWindow)))) {
                    mGame.reset();
                    mPlayerIndex = {mUi.getPlayerOne(), mUi.getPlayerTwo()};
                    mGameState = Game::Running;

                    if (mPlayerIndex[0] == int(mAgents.size())) {
                        mUi.setPlayerOneName("Human");
                        mUi.setPlayerOneTime("--:--");
                    } else {
                        mUi.setPlayerOneName(mAgents[mPlayerIndex[0]]->getName());
                        mUi.setPlayerOneTime("01:00");
                    }
                    if (mPlayerIndex[1] == int(mAgents.size())) {
                        mUi.setPlayerTwoName("Human");
                        mUi.setPlayerTwoTime("--:--");
                    } else {
                        mUi.setPlayerTwoName(mAgents[mPlayerIndex[1]]->getName());
                        mUi.setPlayerTwoTime("01:00");
                    }
                }
            }
            mReleasedButton = false;
        }
        else {
            mReleasedButton = true;
        }
    }
}
