#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Ui {
public:
    Ui(std::string gameTitle);
    void draw(sf::RenderWindow &window);

public:
    static const sf::Vector2f screenSize;
    static const sf::Vector2f gameViewportSize;

private:
    struct Button {
        sf::RectangleShape rect;
        std::string title;
    };

private:
    sf::Font mFont;
    std::vector<Button> mButtons;

    sf::Text mGameTitle;
};
