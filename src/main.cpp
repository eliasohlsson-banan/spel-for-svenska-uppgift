#include <iostream>
#include <SFML/Graphics.hpp>

// class for checking if the game is on the launch screen or not
enum class GameState
{
    LaunchScreen,
    Playing
};

// function for poll events (required by sfml to run every frame)
void pollEvents(sf::RenderWindow &window, GameState &state)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        if (event->is<sf::Event::KeyPressed>())
        {
            state = GameState::Playing;
        }
    }
}

int main()
{
    // sets gamestate to the launch screen
    GameState state = GameState::LaunchScreen;

    // renders window
    sf::RenderWindow window(sf::VideoMode({720, 540}), "game");


    // self explanitory text stuff
    sf::Font font;
    if (!font.openFromFile("fonts/pixeloid-font/normal.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    sf::Font fontBold;
    if (!fontBold.openFromFile("fonts/pixeloid-font/bold.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    sf::Text launchText(font, "Press any key to start...", 30);
    launchText.setPosition({150, 250});

    // main loop
    while (window.isOpen())
    {
        pollEvents(window, state);
        
        if (state == GameState::Playing)
        {
            if (window.getSize().x != sf::VideoMode::getDesktopMode().size.x)
            {
                window.create(sf::VideoMode::getDesktopMode(), "Game", sf::State::Fullscreen);
                window.setVerticalSyncEnabled(true);
            }
        }
        window.clear();
        window.draw(launchText);
        window.display();
    }
}