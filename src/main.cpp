#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <experimental/random>

// structure for commands and values
struct command 
{
    sf::String text;
    int SectionNumber;
};

// function for reading the text file and parsing the important text
sf::Text getText(std::ifstream &textFile, const std::string &targetSection, sf::Font &font) 
{
    bool insideSection = false;
    std::string line;
    std::string result;

    // resets the file so it can search from the beginning again
    textFile.clear();                  // clear end of file tag
    textFile.seekg(0, std::ios::beg);  // restart


    while (getline(textFile, line))
    {   
        // checks for the brackets / headers in the .txt file
        if (!line.empty() && line[0] == '[' && line.back() == ']')
        {
            std::string sectionName = line.substr(1, line.size() - 2);
            insideSection = (sectionName == targetSection);
            continue; // skip printing the header
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

// fix for the stupid swedish letter bullshit
sf::String UTF8(const char* str)
{
    return sf::String::fromUtf8(str, str + std::strlen(str));
}

// function for poll events (required by sfml to run every frame)
void pollEvents(sf::RenderWindow &window, sf::String &userInput, sf::Text &inputText, std::vector<std::vector<command>> &commands, int &currentSection, sf::String &list, sf::Text &tutorialText, sf::Clock &globalTimer)
{
    while (const std::optional event = window.pollEvent())
    {
        // if window is closed then close the window? bro fuck sfml this shit sucks
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        // close window if escape is pressed
        if (event->is<sf::Event::KeyPressed>())
        {
            if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
            {
                window.close();
            }
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

            inputText.setString(userInput);
        }

        if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Enter && globalTimer.getElapsedTime().asSeconds() >= 0.1)
        {
            globalTimer.restart();
            for (const auto& cmd : commands[currentSection])
            {
                if (userInput == cmd.text)
                {
                    currentSection = cmd.SectionNumber;
                    break;
                }
            }   

            list = UTF8("Kommandon: ");
            for (const auto& cmd : commands[currentSection])
                list += cmd.text + UTF8(". ");

            tutorialText.setString(list);
            userInput.clear();
            inputText.setString("");
        }
    }
}



int main()
{   
    // my sprites are in 320 / 180 so divide screen size by the sprite size to scale them properly
    float scaleX = 1920 / 320.f;
    float scaleY = 1200 / 180.f;

    // get text file
    std::ifstream textFile("assets/texts/main.txt");

    // sections of the game
    std::vector<std::string> sections =
    {"intro", "fackla-väggen", "stup", "dark-hall", "stup-hoppa"};

    std::vector<std::vector<command>> commands =
    {
        // section 0, start
        {
            {UTF8("gå framåt"), 1}
        },
        // section 1, höger/vänster
        {
            {UTF8("gå åt höger"), 2},
            {UTF8("gå åt vänster"), 3}
        },
        // section 2, stup
        {
            {UTF8("hoppa"), 4},
            {UTF8("gå tillbaks"), 1}
        },
        // section 3, mörk korridor
        {
            {UTF8("bajsa ner dig"), 6},
            {UTF8("bajsa ner dig2"), 7}
        }
    };

    // renders window
    sf::RenderWindow window(sf::VideoMode({720, 540}), "game");
    
    // ui background sprite
    sf::Texture ui("assets/sprites/ui.png");
    sf::Sprite uiSprite(ui);

    sf::Texture jumpscare("assets/sprites/face.png");
    sf::Sprite face(jumpscare);
    face.setScale({scaleX, scaleY});

    // cursor sprite
    sf::RectangleShape cursor;
    cursor.setSize({4 * scaleX, 6 * scaleY});

    // self explanitory text stuff
    sf::Font font;
    if (!font.openFromFile("assets/fonts/pixeloid-font/normal.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }
    // bold font
    sf::Font fontBold;
    if (!fontBold.openFromFile("assets/fonts/pixeloid-font/bold.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    // user input string and text
    sf::String userInput;
    sf::Text inputText(font, "", 40);
    inputText.setPosition({175, 1035});

    sf::String list(UTF8("Kommandon: gå framåt."));
    sf::Text tutorialText(font, list, 40);
    tutorialText.setPosition({20 * scaleX, 800});

    sf::Text sectionText(font, "", 40);

    sf::Clock cursorTimer;
    sf::Clock textCheckTimer;
    sf::Clock faceTimer;
    sf::Clock globalTimer;

    bool cursorVisible = true;
    int currentSection = 0;
    bool faceVisible = false;
    int faceRNG = 67;
    int randomValue = 0;


    // main loop
    while (window.isOpen())
    {
        pollEvents(window, userInput, inputText, commands, currentSection, list, tutorialText, globalTimer);
        
        // if windowed then fullscreen
        if (window.getSize().x != sf::VideoMode::getDesktopMode().size.x)
        {
            window.create(sf::VideoMode::getDesktopMode(), "Game", sf::State::Fullscreen);
            window.setVerticalSyncEnabled(true);
            uiSprite.setScale({scaleX, scaleY});
        }

        // main game logic
        cursor.setPosition({inputText.getPosition().x + inputText.getLocalBounds().size.x, inputText.getPosition().y + 5});

        // jumpscare random logic
        if (!faceVisible && faceTimer.getElapsedTime().asSeconds() >= 1)
        {
            randomValue = std::experimental::randint(1, 800);
            if (randomValue == faceRNG) { faceVisible = true; }
            std::cout << "random value: " << randomValue << "\n";
            faceTimer.restart();
        }

        // cursor blonk
        if (cursorTimer.getElapsedTime().asSeconds() >= 0.5)
        {
            cursorVisible = !cursorVisible;
            cursorTimer.restart();
        }

        // delay for text so it wont check every frame or hang itself completely (you little fucker fuck you) 
        if (textCheckTimer.getElapsedTime().asSeconds() >= 0.05)
        {
            sectionText = getText(textFile, sections[currentSection], font);
            textCheckTimer.restart();
        }

        // screen text
        window.clear();
        window.draw(uiSprite);
        window.draw(sectionText);
        window.draw(inputText);
        window.draw(tutorialText);
        if (cursorVisible) window.draw(cursor);
        // face jumpscare :)
        if (faceVisible)
        {
            window.draw(face);
            if (faceTimer.getElapsedTime().asSeconds() >= 0.15)
            {
                faceVisible = false;
                faceTimer.restart();
            }
        }

        window.display();
    }
}