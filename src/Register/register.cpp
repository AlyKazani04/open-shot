#include "register.hpp"

void nameEntry(sf::RenderWindow& window, GameState& state, std::string& playerName)
{
    playerName.clear();
    sf::Font font;
    if(!font.openFromFile(FONT_PATH))
    {
        std::cerr << "[ERROR] Missing font: " << FONT_PATH << std::endl;
        state = GameState::Exiting;
        return;
    }
    bool done = false;
    bool showCursor = true;
    auto lastBlink = std::chrono::steady_clock::now();

    sf::Text nameDisplay(font, "", 36);
    nameDisplay.setFillColor(sf::Color::White);
    nameDisplay.setPosition({(SCREEN_WIDTH / 2) - nameDisplay.getLocalBounds().position.x / 2, SCREEN_HEIGHT / 2 - nameDisplay.getLocalBounds().size.y / 2});
    
    sf::Text prompt(font, "Enter your name (max 40 chars):", 26);
    prompt.setFillColor(sf::Color(220,220,255));
    prompt.setPosition({(SCREEN_WIDTH / 2) - prompt.getLocalBounds().size.x / 2, (SCREEN_HEIGHT / 2) - nameDisplay.getGlobalBounds().position.y - 40});

    sf::Text info(font, "Press ENTER to confirm, ESC to quit.", 18);
    info.setFillColor(sf::Color(180,180,180));
    info.setPosition({(SCREEN_WIDTH / 2) - info.getLocalBounds().size.x / 2, (SCREEN_HEIGHT / 2) + nameDisplay.getGlobalBounds().size.y + 40});

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

        window.clear(sf::Color(31, 31, 31));
        window.draw(prompt);
        window.draw(nameDisplay);
        window.draw(info);
        window.display();
    }
    state = GameState::Playing;
}

void showLeaderboard(sf::RenderWindow& window, GameState& state)
{
    // TODO: implement leaderboard display and CSV I/O
    state = GameState::Exiting;
}