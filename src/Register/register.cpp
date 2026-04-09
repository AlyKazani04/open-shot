#include "register.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "constants.hpp"

namespace
{
    void ensureLeaderboardDir()
    {
        std::error_code ec;
        std::filesystem::create_directories(LEADERBOARD_DIR, ec);
    }

}

void nameEntry(sf::RenderWindow& window, GameState& state, std::string& playerName)
{
    playerName.clear();
    const float su = scaleUniform();

    sf::Font font;
    if(!font.openFromFile(FONT_PATH))
    {
        std::cerr << "[ERROR] Missing font: " << FONT_PATH << std::endl;
        state = GameState::Exiting;
        return;
    }
    font.setSmooth(false);

    bool done = false;
    bool showCursor = true;
    auto lastBlink = std::chrono::steady_clock::now();

    unsigned int titleSize = static_cast<unsigned int>(42.0f * su);
    if(titleSize < 24u)
    {
        titleSize = 24u;
    }

    unsigned int promptSize = static_cast<unsigned int>(28.0f * su);
    if(promptSize < 18u)
    {
        promptSize = 18u;
    }

    unsigned int nameSize = static_cast<unsigned int>(36.0f * su);
    if(nameSize < 20u)
    {
        nameSize = 20u;
    }

    unsigned int infoSize = static_cast<unsigned int>(18.0f * su);
    if(infoSize < 14u)
    {
        infoSize = 14u;
    }

    sf::Text nameDisplay(font, "", nameSize);
    nameDisplay.setFillColor(sf::Color::White);
    nameDisplay.setPosition({(SCREEN_WIDTH / 2) - nameDisplay.getLocalBounds().position.x / 2, SCREEN_HEIGHT / 2 - nameDisplay.getLocalBounds().size.y / 2});


    sf::Text prompt(font, "Enter your name (max 40 chars):", promptSize);
    prompt.setFillColor(sf::Color(220,220,255));
    prompt.setPosition({(SCREEN_WIDTH / 2) - prompt.getLocalBounds().size.x / 2,  80});

    sf::Text info(font, "ENTER: confirm  ESC: quit.", infoSize);
    info.setFillColor(sf::Color(180,180,180));
    info.setPosition({(SCREEN_WIDTH / 2) - info.getLocalBounds().size.x / 2, (SCREEN_HEIGHT - 60)});

    sf::Text title(font, "Open Shot", titleSize);
    title.setFillColor(sf::Color(230, 230, 250));
    title.setPosition({(SCREEN_WIDTH / 2.0f) - title.getLocalBounds().size.x / 2.0f, 20.0f});

    while(!done && window.isOpen())
    {
        auto eventOpt = window.pollEvent();
        while(eventOpt)
        {
            if(eventOpt->is<sf::Event::Closed>())
            {
                window.close();
                state = GameState::Exiting;
                return;
            }

            if(eventOpt->is<sf::Event::KeyPressed>())
            {
                auto key = eventOpt->getIf<sf::Event::KeyPressed>()->code;
                if(key == sf::Keyboard::Key::Escape)
                {
                    state = GameState::Exiting;
                    return;
                } 

                if(key == sf::Keyboard::Key::Enter)
                {
                    if(!playerName.empty())
                        done = true;
                } 
                else if(key == sf::Keyboard::Key::Backspace)
                {
                    if(!playerName.empty()) 
                        playerName.pop_back();
                }
            }
            else if(eventOpt->is<sf::Event::TextEntered>())
            {
                char c = static_cast<char>(eventOpt->getIf<sf::Event::TextEntered>()->unicode);
                if(c >= 32 && c < 127 && (int)playerName.size() < MAX_NAME_LENGTH)
                {
                    playerName += c;
                }
            }
            eventOpt = window.pollEvent();
        }
        
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlink).count() > 500) 
        {
            showCursor = !showCursor;
            lastBlink = now;
        }
        
        std::string display = playerName;
        if(showCursor) 
        {
            display += "_";
        }
        nameDisplay.setString(display);

        sf::FloatRect textRect = nameDisplay.getLocalBounds();   
        nameDisplay.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
        });

        nameDisplay.setPosition({SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});

        window.clear(sf::Color(31, 31, 31));
        window.draw(title);
        window.draw(prompt);
        window.draw(nameDisplay);
        window.draw(info);
        window.display();
    }
    state = GameState::Playing;
}

void saveSessionResult(const std::string& playerName, int score)
{
    ensureLeaderboardDir();

    std::ofstream out(SESSION_RESULTS_FILE, std::ios::app);
    if(!out.is_open())
    {
        std::cerr << "[ERROR] Could not append session results: " << SESSION_RESULTS_FILE
                  << std::endl;
        return;
    }

    std::string safeName = playerName;
    for(char& c : safeName)
    {
        if(c == '"' || c == '\\')
        {
            c = '_';
        }
    }

    out << "{\"game\":\"" << GAME_ID
        << "\",\"player\":\"" << safeName
        << "\",\"score\":" << score
        << "}" << '\n';
}
