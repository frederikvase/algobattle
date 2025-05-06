#include "ui.h"

const sf::Vector2f Ui::screenSize = sf::Vector2f(1920.f, 1080.f);
const sf::Vector2f Ui::gameViewportSize = sf::Vector2f(750.f, 750.f);

Ui::Ui(std::string gameTitle)
    : mButtonsLeft()
    , mButtonsRight()
    , mPlayButton()
    , mFont() 
    , mGameTitle()
    , mPlayerOneText()
    , mPlayerTwoText()
    , mPlayerOneName()
    , mPlayerOneTime()
    , mPlayerTwoName()
    , mPlayerTwoTime()
{
    while (!mFont.loadFromFile("resources/fonts/comic-shanns.ttf")) {
        // error...
    }

    mGameTitle = GUI::Text(sf::Vector2f(screenSize.x / 2.f, 10.f), mFont, gameTitle, 84);
    mPlayerOneText = GUI::Text(sf::Vector2f(280, 70.f), mFont, "Player one", 60);
    mPlayerTwoText = GUI::Text(sf::Vector2f(1640.f, 70.f), mFont, "Player two", 60);

    mPlayButton = GUI::Button(sf::Vector2f(screenSize.x / 2.f, 1000.f), mFont, "Play/Simulate", 50);

    mPlayerOneName = GUI::Text((screenSize-gameViewportSize) / 2.f + sf::Vector2f(0.f, gameViewportSize.y), mFont, "", 60);
    mPlayerOneName.text.setFillColor(sf::Color(250, 82, 82));
    mPlayerOneTime = GUI::Text((screenSize-gameViewportSize) / 2.f + gameViewportSize, mFont, "", 60);
    mPlayerOneTime.text.setFillColor(sf::Color(250, 82, 82));
    mPlayerTwoName = GUI::Text((screenSize-gameViewportSize) / 2.f, mFont, "", 60);
    mPlayerTwoName.text.setFillColor(sf::Color(34, 139, 230));
    mPlayerTwoTime = GUI::Text((screenSize-gameViewportSize) / 2.f + sf::Vector2f(gameViewportSize.x, 0.f), mFont, "", 60);
    mPlayerTwoTime.text.setFillColor(sf::Color(34, 139, 230));
}

void Ui::init(std::vector<std::string> &agents) {
    mSelectedLeft = agents.size();
    mSelectedRight = agents.size();

    auto AddLeftButton = [&](sf::Vector2f pos, std::string name) {
        mButtonsLeft.push_back(GUI::Button(pos, mFont, name, 50));
        mButtonsLeft.back().rect.setFillColor(sf::Color(250, 82, 82));
        mButtonsLeft.back().rect.setOutlineColor(sf::Color(224, 49, 49));
    };
    auto AddRightButton = [&](sf::Vector2f pos, std::string name) {
        mButtonsRight.push_back(GUI::Button(pos, mFont, name, 50));
        mButtonsRight.back().rect.setFillColor(sf::Color(34, 139, 230));
        mButtonsRight.back().rect.setOutlineColor(sf::Color(25, 113, 194));
    };

    for (int i = 0; i < int(agents.size()); i++) {
        AddLeftButton(sf::Vector2f(280.f, 215.f + 100.f * i), agents[i]);
        AddRightButton(sf::Vector2f(1640.f, 215.f + 100.f * i), agents[i]);
    }

    AddLeftButton(sf::Vector2f(280.f, 875.f), "Human");
    AddRightButton(sf::Vector2f(1640.f, 875.f), "Human");
}

bool Ui::handleInput(sf::Vector2f mousePos) {
    auto IsButtonClicked = [&](GUI::Button &button) {
        sf::FloatRect rect = button.rect.getGlobalBounds();
        return (rect.left <= mousePos.x && rect.left + rect.width >= mousePos.x &&
                rect.top <= mousePos.y && rect.top + rect.height >= mousePos.y);
    };

    for (int i = 0; i < int(mButtonsLeft.size()); i++) {
        if (IsButtonClicked(mButtonsLeft[i])) {
            mSelectedLeft = i;
            break;
        }
    }
    for (int i = 0; i < int(mButtonsRight.size()); i++) {
        if (IsButtonClicked(mButtonsRight[i])) {
            mSelectedRight = i;
            break;
        }
    }

    return IsButtonClicked(mPlayButton);
}

void Ui::draw(sf::RenderWindow &window) {
    auto DarkenRect = [&](sf::RectangleShape &rect) {
        sf::RectangleShape indicator(rect.getSize() + 2.f * 
                                     sf::Vector2f(rect.getOutlineThickness(), 
                                                  rect.getOutlineThickness()));
        indicator.setOrigin(indicator.getSize() / 2.f);
        indicator.setPosition(rect.getPosition());
        indicator.setFillColor(sf::Color(0, 0, 0, 90));
        window.draw(indicator);
    };
    for (int i = 0; i < int(mButtonsLeft.size()); i++) {
        mButtonsLeft[i].draw(window);
        if (i == mSelectedLeft) {
            DarkenRect(mButtonsLeft[i].rect);
        }
    }
    for (int i = 0; i < int(mButtonsRight.size()); i++) {
        mButtonsRight[i].draw(window);
        if (i == mSelectedRight) {
            DarkenRect(mButtonsRight[i].rect);
        }
    }

    sf::RectangleShape gameViewportOutline(gameViewportSize);
    gameViewportOutline.setOrigin(gameViewportSize / 2.f);
    gameViewportOutline.setPosition(screenSize / 2.f);
    gameViewportOutline.setOutlineThickness(20.f);
    gameViewportOutline.setOutlineColor(sf::Color::White);
    gameViewportOutline.setFillColor(sf::Color::Transparent);
    window.draw(gameViewportOutline);

    mGameTitle.draw(window);
    mPlayerOneText.draw(window);
    mPlayerTwoText.draw(window);

    mPlayButton.draw(window);

    mPlayerOneName.draw(window);
    mPlayerOneTime.draw(window);
    mPlayerTwoName.draw(window);
    mPlayerTwoTime.draw(window);
}

int Ui::getPlayerOne() const {
    return mSelectedLeft;
}

int Ui::getPlayerTwo() const {
    return mSelectedRight;
}

sf::Font& Ui::getFont() {
    return mFont;
}

void Ui::setPlayerOneName(std::string name) {
    mPlayerOneName.text.setString(name);
    mPlayerOneName.text.setOrigin(-5.f, 80.f);
}
void Ui::setPlayerTwoName(std::string name) {
    mPlayerTwoName.text.setString(name);
    mPlayerTwoName.text.setOrigin(-5.f, 10.f);
}
void Ui::setPlayerOneTime(std::string time) {
    mPlayerOneTime.text.setString(time);
    mPlayerOneTime.text.setOrigin(mPlayerOneTime.text.getGlobalBounds().width+20.f, 80.f);
}
void Ui::setPlayerTwoTime(std::string time) {
    mPlayerTwoTime.text.setString(time);
    mPlayerTwoTime.text.setOrigin(mPlayerTwoTime.text.getGlobalBounds().width+20.f, 10.f);
}
