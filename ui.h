#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace GUI {
struct Text {
    Text() {}
    Text(sf::Vector2f pos, sf::Font &font, std::string string, int size) {
        text.setFont(font);
        text.setString(string);
        text.setCharacterSize(size);
        text.setFillColor(sf::Color::White);
        text.setOrigin(text.getGlobalBounds().width / 2.f, 0.f);
        text.setPosition(pos);
    }

    void draw(sf::RenderWindow &window) {
        window.draw(text);
    }

    sf::Text text;
};

struct Button {
    Button() {}
    Button(sf::Vector2f pos, sf::Font &font, std::string string, int size) {
        caption.setFont(font);
        caption.setString(string);
        caption.setCharacterSize(size);
        caption.setFillColor(sf::Color::White);
        caption.setOrigin(caption.getGlobalBounds().width / 2.f, 0.f);

        rect.setSize(sf::Vector2f(500.f, 80.f));
        rect.setOrigin(rect.getSize() / 2.f);
        rect.setFillColor(sf::Color(25, 25, 25));
        rect.setOutlineThickness(5.f);
        rect.setOutlineColor(sf::Color::White);

        caption.setPosition(pos - sf::Vector2f(0.f, float(size) * 0.7f));
        rect.setPosition(pos);
    }
    void draw(sf::RenderWindow &window) {
        window.draw(rect);
        window.draw(caption);
    }

    sf::RectangleShape rect;
    sf::Text caption;
};
}

class Ui {
public:
    Ui(std::string gameTitle);
    void init(std::vector<std::string> &agents);
    bool handleInput(sf::Vector2f mousePos);
    void draw(sf::RenderWindow &window);

public:
    int getPlayerOne() const;
    int getPlayerTwo() const;

    sf::Font& getFont();

    void setPlayerOneName(std::string name);
    void setPlayerTwoName(std::string name);
    void setPlayerOneTime(std::string time);
    void setPlayerTwoTime(std::string time);

public:
    static const sf::Vector2f screenSize;
    static const sf::Vector2f gameViewportSize;

private:
    sf::Font mFont;
    std::vector<GUI::Button> mButtonsLeft;
    std::vector<GUI::Button> mButtonsRight;
    GUI::Button mPlayButton;

    int mSelectedLeft, mSelectedRight;

    GUI::Text mGameTitle, mPlayerOneText, mPlayerTwoText;
    GUI::Text mPlayerOneName, mPlayerOneTime;
    GUI::Text mPlayerTwoName, mPlayerTwoTime;
};
