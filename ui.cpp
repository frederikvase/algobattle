#include "ui.h"

const sf::Vector2f Ui::screenSize = sf::Vector2f(1920.f, 1080.f);
const sf::Vector2f Ui::gameViewportSize = sf::Vector2f(750.f, 750.f);

Ui::Ui(std::string gameTitle)
    : mButtons()
    , mFont() 
    , mGameTitle()
{
    while (!mFont.loadFromFile("resources/fonts/comic-shanns.ttf")) {
        // error...
    }

    mGameTitle.setFont(mFont);
    mGameTitle.setString(gameTitle);
    mGameTitle.setCharacterSize(84);
    mGameTitle.setFillColor(sf::Color::White);
    mGameTitle.setOrigin(mGameTitle.getGlobalBounds().width / 2.f, 0.f);
    mGameTitle.setPosition(screenSize.x / 2.f, 10.f);
}

void Ui::draw(sf::RenderWindow &window) {
    for (auto button : mButtons) {
        window.draw(button.rect);
    }

    sf::RectangleShape gameViewportOutline(gameViewportSize);
    gameViewportOutline.setOrigin(gameViewportSize / 2.f);
    gameViewportOutline.setPosition(screenSize / 2.f);
    gameViewportOutline.setOutlineThickness(20.f);
    gameViewportOutline.setOutlineColor(sf::Color::White);
    gameViewportOutline.setFillColor(sf::Color::Transparent);
    window.draw(gameViewportOutline);

    window.draw(mGameTitle);
}
