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
sf::Text getText(std::ifstream &textFile, const std::string &targetSection, sf::Font &font) 
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
    //converts the string to an sf::Text because otherwise it will fuck itself anally
    sf::String sfmlResult = sf::String::fromUtf8(result.begin(), result.end());
    sf::Text text(font, sfmlResult, 40);
    text.setPosition({20 * (1920 / 320), 20 * (1080 / 180)});
    return text;
}

// function for poll events (required by sfml to run every frame)
void pollEvents(sf::RenderWindow &window, GameState &state, sf::String &userInput, sf::Text &inputText)
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

        // text input box
        if (event->is<sf::Event::TextEntered>())
        {
            auto unicode = event->getIf<sf::Event::TextEntered>()->unicode;

            // check for backspace
            if (unicode == 8)
            {
                if (!userInput.isEmpty())
                    userInput.erase(userInput.getSize() - 1, 1);
            }
            // >= 32 are printable characters
            else if (unicode >= 32) 
            {
                userInput += unicode;
            }

            inputText.setString(sf::String::fromUtf8(userInput.begin(), userInput.end()));
        }
    }
}



int main()
{   
    // my sprites are in 320 / 180 so divide screen size by the sprite size to scale them properly
    float scaleX = 1920 / 320.f;
    float scaleY = 1200 / 180.f;

    // get text file
    std::ifstream textFile("texts/main.txt");

    // sections of the game
    std::vector<std::string> sections =
    {"intro", "fackla-väggen", "dead end"};

    // sets gamestate to the launch screen
    GameState state = GameState::LaunchScreen;

    // renders window
    sf::RenderWindow window(sf::VideoMode({720, 540}), "game");
    
    // ui + background sprite
    sf::Texture ui("sprites/ui.png");
    sf::Sprite uiSprite(ui);

    // cursor sprite
    sf::RectangleShape cursor;
    cursor.setSize({4 * scaleX, 14 * scaleY});

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

    // user input string and text
    sf::String userInput;
    sf::Text inputText(font, "", 40);
    inputText;

    sf::Clock cursorTimer;

    bool cursorVisible = true;
    int currentSection = 0;

    // main loop
    while (window.isOpen())
    {
        pollEvents(window, state, userInput, inputText);
        
        // if windowed then fullscreen
        if (window.getSize().x != sf::VideoMode::getDesktopMode().size.x)
        {
            window.create(sf::VideoMode::getDesktopMode(), "Game", sf::State::Fullscreen);
            window.setVerticalSyncEnabled(true);
            uiSprite.setScale({scaleX, scaleY});
        }

        // main game logic
        
        cursor.setPosition({inputText.getPosition().x + inputText.getLocalBounds().size.x, inputText.getPosition().y});

        if (cursorTimer.getElapsedTime().asSeconds() >= 0.5)
        {
            cursorVisible = !cursorVisible;
            cursorTimer.restart();
        }

        static sf::Text sectionText = getText(textFile, sections[currentSection], font);
        window.clear();
        window.draw(uiSprite);
        window.draw(sectionText);
        window.draw(inputText);
        if (cursorVisible) window.draw(cursor);
        

        window.display();
    }
}