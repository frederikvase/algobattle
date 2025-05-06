#pragma once
#include "ui.h"
#include "agent.h"

#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <fcntl.h>

#include <vector>
#include <array>

#include <chrono>
using namespace std::chrono_literals;

template<class Game, class Board>
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
    Board mBoard;

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
    bool mCalculating;

    int mPipefd[2];
    pid_t mChild_pid;
};

template<class Game, class Board>
App<Game, Board>::App(std::vector<Agent<Game>*> agents, std::chrono::milliseconds agentTimeLimit)
    : mGame(), mBoard()
    , mUi(mBoard.getTitle())
    , mAgents(agents)
    , mWindow(sf::VideoMode(Ui::screenSize.x, Ui::screenSize.y), "AlgoBattle", sf::Style::Fullscreen)
    , mGameView(sf::FloatRect({0.f, 0.f}, mBoard.getScreenSize()))
    , mUiView(sf::FloatRect({0.f, 0.f}, Ui::screenSize))
    , mPlayerIndex({int(agents.size()), int(agents.size())})
    , mTimeLimit(agentTimeLimit)
    , mLastTimePoint()
    , mP1RemainingTime()
    , mP2RemainingTime()
    , mGameState(Game::Defualt)
    , mCalculating(false)
    , mPipefd()
    , mChild_pid(-1)
{
    mPipefd[0] = mPipefd[1] = -1;

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

template<class Game, class Board>
void App<Game, Board>::run() {
    while (mWindow.isOpen()) {
        handleEvents();
        update();
        draw();
    }
}

template<class Game, class Board>
void App<Game, Board>::update() {
    const int turn = mGame.getPlayerTurn();

    if (mPlayerIndex[turn] != int(mAgents.size()) && mGameState == Game::Running && !mCalculating) {
        pipe(mPipefd); // [0] = read, [1] = write
        mCalculating = true;

        mChild_pid = fork();
        if (mChild_pid == 0) { // child process
            close(mPipefd[0]);

            auto move = mAgents[mPlayerIndex[turn]]->calculateMove(mGame);
            write(mPipefd[1], &move, sizeof(move));
            close(mPipefd[1]);
            _exit(0); // Breaks if I use exit(0) for some reason
        } else { // parent process
            close(mPipefd[1]);
            fcntl(mPipefd[0], F_SETFL, O_NONBLOCK);
        }
    }

    if (mCalculating) {
        typename Game::Move move;
        ssize_t bytes = read(mPipefd[0], &move, sizeof(move));
        if (bytes == sizeof(move)) {
            close(mPipefd[0]);
            waitpid(mChild_pid, nullptr, 0);
            mChild_pid = -1;

            if (!mGame.makeMove(move)) {
                exit(0);
            }

            mGameState = mGame.getGameResult();
            mCalculating = false;
        }
    }

    const auto now = std::chrono::steady_clock::now();
    if (mPlayerIndex[turn] != int(mAgents.size()) && mGameState == Game::Running) {
        (turn ? mP2RemainingTime : mP1RemainingTime) -= now - mLastTimePoint;
        if ((turn ? mP2RemainingTime : mP1RemainingTime) <= 0s) {
            mGameState = turn ? Game::State::P1WinTime : Game::State::P2WinTime;

            if (mChild_pid > 0) {
                kill(mChild_pid, SIGTERM);
                waitpid(mChild_pid, nullptr, 0);
            }

            return;
        }
    }
    mLastTimePoint = now;
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

template<class Game, class Board>
void App<Game, Board>::handleEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (mReleasedButton) {
                sf::Vector2f mousePos = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow), mGameView);
                if (mousePos.x >= 0.f && mousePos.x <= mBoard.getScreenSize().x && 
                    mousePos.y >= 0.f && mousePos.y <= mBoard.getScreenSize().y) {
                    if (mPlayerIndex[mGame.getPlayerTurn()] == int(mAgents.size()) && mGameState == Game::Running) {
                        if (auto move = mBoard.handleInput(mousePos, mGame)) {
                            mGame.makeMove(*move);
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

template<class Game, class Board>
void App<Game, Board>::draw() {
    mWindow.clear(sf::Color::Black);

    mWindow.setView(mGameView);
    mBoard.draw(mWindow, mGame);

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
