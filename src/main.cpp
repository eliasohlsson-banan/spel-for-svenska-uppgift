#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>

// class for checking if the game is on the launch screen or not
enum class GameState
{
    LaunchScreen,
    Playing
};

// function for reading the text file and parsing the important text
std::string getText(std::ifstream &textFile, const std::string &targetSection) 
{
    bool insideSection = false;
    std::string line;
    std::string result;

    while (getline(textFile, line))
    {   
        // checks for the brackets / headers in the .txt file
        if (!line.empty() && line[0] == '[' && line.back() == ']')
        {
            std::string sectionName = line.substr(1, line.size() - 2);
            insideSection = (sectionName == targetSection);
            continue; // skip printing the header itself
        }

        // only print lines if inside the section we want
        if (insideSection)
        {
            result += line + "\n";
        }
    }
    return result;
}

// function for poll events (required by sfml to run every frame)
void pollEvents(sf::RenderWindow &window, GameState &state)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        // if key pressed during launch screen then switch to playing state
        if (state == GameState::LaunchScreen && event->is<sf::Event::KeyPressed>())
        {
            state = GameState::Playing;
        }
    }
}

int main()
{   
    // get text file
    std::ifstream textFile("texts/main.txt");

    // sections of the game
    std::vector<std::string> sections =
    {"intro", "left-right", "dead end"};

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
    // bold font
    sf::Font fontBold;
    if (!fontBold.openFromFile("fonts/pixeloid-font/bold.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    sf::Text launchText(font, "Press any key to start...", 30);
    launchText.setPosition({150, 250});
    std::string introStr = getText(textFile, "intro");
    sf::String sfmlIntroStr = sf::String::fromUtf8(introStr.begin(), introStr.end());
    sf::Text introText(font, introStr, 30);

    // main loop
    while (window.isOpen())
    {
        pollEvents(window, state);
        
        // if not on the launch screen
        if (state == GameState::Playing)
        {
            // if windowed then fullscreen
            if (window.getSize().x != sf::VideoMode::getDesktopMode().size.x)
            {
                window.create(sf::VideoMode::getDesktopMode(), "Game", sf::State::Fullscreen);
                window.setVerticalSyncEnabled(true);
            }

            window.clear();
            window.draw(introText);
        }

        if (state == GameState::LaunchScreen)
        {
            window.clear();
            window.draw(launchText);
        }

        window.display();
    }
}