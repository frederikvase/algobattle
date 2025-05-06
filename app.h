#pragma once
#include "ui.h"
#include "agent.h"

#include <iostream>

#include <vector>
#include <array>

#include <future>
#include <chrono>
using namespace std::chrono_literals;

template<class Game>
class App {
public:
    App(std::vector<Agent<Game>*> agents, std::chrono::milliseconds agentTimeLimit);
    void run();

private:
    void update();
    void handleEvents();
    void draw();

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

    std::chrono::milliseconds mTimeLimit;
    std::chrono::time_point<std::chrono::steady_clock> mLastTimePoint;
    std::chrono::nanoseconds mP1RemainingTime;
    std::chrono::nanoseconds mP2RemainingTime;

    typename Game::State mGameState;

    std::future<typename Game::Move> mFuture;
    bool mCalculating;
};

template<class Game>
App<Game>::App(std::vector<Agent<Game>*> agents, std::chrono::milliseconds agentTimeLimit)
    : mGame(), mUi(mGame.getTitle())
    , mAgents(agents)
    , mWindow(sf::VideoMode(Ui::screenSize.x, Ui::screenSize.y), "AlgoBattle", sf::Style::Fullscreen)
    , mGameView(sf::FloatRect({0.f, 0.f}, mGame.getScreenSize()))
    , mUiView(sf::FloatRect({0.f, 0.f}, Ui::screenSize))
    , mPlayerIndex({int(agents.size()), int(agents.size())})
    , mTimeLimit(agentTimeLimit)
    , mLastTimePoint()
    , mP1RemainingTime()
    , mP2RemainingTime()
    , mGameState(Game::Defualt)
    , mFuture()
    , mCalculating(false)
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
        update();
        draw();
    }
}

template<class Game>
void App<Game>::update() {
    const int turn = mGame.getPlayerTurn();

    const auto now = std::chrono::steady_clock::now();

    if (mPlayerIndex[turn] != int(mAgents.size()) && mGameState == Game::Running) {
        (turn ? mP2RemainingTime : mP1RemainingTime) -= now - mLastTimePoint;
        if ((turn ? mP2RemainingTime : mP1RemainingTime) <= 0s) {
            mGameState = turn ? Game::State::P1WinTime : Game::State::P2WinTime;
            return;
        }
    }
    mLastTimePoint = now;

    if (mPlayerIndex[turn] != int(mAgents.size()) && mGameState == Game::Running && !mCalculating) {
        mCalculating = true;
        mFuture = std::async(std::launch::async, &Agent<Game>::calculateMove, mAgents[mPlayerIndex[turn]], mGame);
    }

    if (mCalculating) {
        if (mFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            typename Game::Move move = mFuture.get();

            if (!mGame.makeMove(move, true)) {
                exit(0);
            }
            mGameState = mGame.getGameResult();
            mCalculating = false;
        }
    }
}

std::string timeToString(std::chrono::nanoseconds timeLeft) {
    if (timeLeft <= 0s) {
        return "00:00";
    }

    std::string res;
    if (timeLeft.count() / 1000000000ull < 10) {
        res += "0";
    }
    res += std::to_string(timeLeft.count() / 1000000000ull);
    res += ":";
    if (timeLeft.count() % 1000000000ull / 10000000ull < 10) {
        res += "0";
    }
    res += std::to_string(timeLeft.count() % 1000000000ull / 10000000ull);
    return res;
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
                    mCalculating = false;

                    mLastTimePoint = std::chrono::steady_clock::now();

                    if (mPlayerIndex[0] == int(mAgents.size())) {
                        mUi.setPlayerOneName("Human");
                        mUi.setPlayerOneTime("--:--");
                    } else {
                        mUi.setPlayerOneName(mAgents[mPlayerIndex[0]]->getName());
                        mP1RemainingTime = mTimeLimit;
                    }
                    if (mPlayerIndex[1] == int(mAgents.size())) {
                        mUi.setPlayerTwoName("Human");
                        mUi.setPlayerTwoTime("--:--");
                    } else {
                        mUi.setPlayerTwoName(mAgents[mPlayerIndex[1]]->getName());
                        mP2RemainingTime = mTimeLimit;
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

template<class Game>
void App<Game>::draw() {
    mWindow.clear(sf::Color::Black);

    mWindow.setView(mGameView);
    mGame.draw(mWindow);

    mWindow.setView(mUiView);
    if (mPlayerIndex[0] != int(mAgents.size())) {
        mUi.setPlayerOneTime(timeToString(mP1RemainingTime));
    }
    if (mPlayerIndex[1] != int(mAgents.size())) {
        mUi.setPlayerTwoTime(timeToString(mP2RemainingTime));
    }
    mUi.draw(mWindow);

    auto DrawGameResult = [&](std::string text, sf::Color textColor) {
        sf::RectangleShape rect(sf::Vector2f(0.8f * Ui::gameViewportSize.x, 0.3f * Ui::gameViewportSize.y) );
        rect.setPosition(Ui::screenSize / 2.f);
        rect.setOrigin(rect.getSize() / 2.f);
        rect.setFillColor(sf::Color(0,0,0,180));
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
    } else if (mGameState == Game::State::Draw) {
        DrawGameResult("Draw by repetition!", sf::Color(125, 125, 125));
    }

    mWindow.display();
}
