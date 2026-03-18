#include "register.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

namespace
{
    struct LeaderboardEntry
    {
        std::string name;
        int score{};
    };

    void ensureLeaderboardDir()
    {
        std::error_code ec;
        std::filesystem::create_directories(LEADERBOARD_DIR, ec);
    }

    std::vector<LeaderboardEntry> loadLeaderboard()
    {
        std::vector<LeaderboardEntry> entries;

        std::ifstream in(LEADERBOARD_FILE);
        if(!in.is_open())
        {
            return entries;
        }

        std::string line;
        while(std::getline(in, line))
        {
            if(line.empty())
            {
                continue;
            }

            auto commaPos = line.find_last_of(',');
            if(commaPos == std::string::npos)
            {
                continue;
            }

            std::string name = line.substr(0, commaPos);
            std::string scoreStr = line.substr(commaPos + 1);
            std::istringstream iss(scoreStr);
            int score = 0;
            if(!(iss >> score))
            {
                continue;
            }

            if(name.empty())
            {
                continue;
            }

            entries.push_back({name, score});
        }

        return entries;
    }

    void saveLeaderboard(const std::vector<LeaderboardEntry>& entries)
    {
        ensureLeaderboardDir();

        std::ofstream out(LEADERBOARD_FILE, std::ios::trunc);
        if(!out.is_open())
        {
            std::cerr << "[ERROR] Could not write leaderboard file: " << LEADERBOARD_FILE
                      << std::endl;
            return;
        }

        const std::size_t count = std::min(entries.size(), static_cast<std::size_t>(MAX_LEADERBOARD_RECORDS));
        for(std::size_t i = 0; i < count; ++i)
        {
            out << entries[i].name << "," << entries[i].score << '\n';
        }
    }

    void upsertLeaderboardEntry(std::vector<LeaderboardEntry>& entries,
                                const std::string& playerName,
                                int score)
    {
        entries.push_back({playerName, score});

        std::sort(entries.begin(), entries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
            if(a.score != b.score)
            {
                return a.score > b.score;
            }
            return a.name < b.name;
        });

        if(entries.size() > static_cast<std::size_t>(MAX_LEADERBOARD_RECORDS))
        {
            entries.resize(MAX_LEADERBOARD_RECORDS);
        }
    }
}

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


    sf::Text prompt(font, "Enter your name (max 40 chars):", 28);
    prompt.setFillColor(sf::Color(220,220,255));
    prompt.setPosition({(SCREEN_WIDTH / 2) - prompt.getLocalBounds().size.x / 2,  60});

    sf::Text info(font, "ENTER: confirm  ESC: quit.", 18);
    info.setFillColor(sf::Color(180,180,180));
    info.setPosition({(SCREEN_WIDTH / 2) - info.getLocalBounds().size.x / 2, (SCREEN_HEIGHT - 60)});

    sf::Text title(font, "Open Shot", 42);
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

void showLeaderboard(sf::RenderWindow& window,
                     GameState& state,
                     const std::string& playerName,
                     int score)
{
    auto entries = loadLeaderboard();
    upsertLeaderboardEntry(entries, playerName, score);
    saveLeaderboard(entries);

    sf::Font font;
    if(!font.openFromFile(FONT_PATH))
    {
        std::cerr << "[ERROR] Missing font: " << FONT_PATH << std::endl;
        state = GameState::Exiting;
        return;
    }

    sf::Text title(font, "Leaderboard (Top 10)", 28);
    title.setFillColor(sf::Color::White);
    title.setPosition({(SCREEN_WIDTH / 2) - title.getLocalBounds().size.x / 2, 40.0f});

    std::vector<sf::Text> rows;
    rows.reserve(std::min(entries.size(), static_cast<std::size_t>(MAX_LEADERBOARD_DISPLAY)));

    float startY = 100.0f;
    float lineHeight = 26.0f;

    for(std::size_t i = 0;
        i < entries.size() && i < static_cast<std::size_t>(MAX_LEADERBOARD_DISPLAY);
        ++i)
    {
        const auto& e = entries[i];
        std::string line = std::to_string(i + 1) + ". " + e.name + " - " + std::to_string(e.score);

        sf::Text text(font, line, 20);

        bool isCurrent = (e.name == playerName && e.score == score);
        if(isCurrent)
        {
            text.setFillColor(sf::Color(255, 230, 120));
        }
        else
        {
            text.setFillColor(sf::Color(210, 210, 210));
        }

        text.setPosition({40.0f, startY + static_cast<float>(rows.size()) * lineHeight});
        rows.push_back(text);
    }

    sf::Text footer(font, "ENTER: play again   ESC: quit", 18);
    footer.setFillColor(sf::Color(180, 180, 180));
    footer.setPosition({(SCREEN_WIDTH / 2) - footer.getLocalBounds().size.x / 2,
                        static_cast<float>(SCREEN_HEIGHT) - 40.0f});

    bool done = false;
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
                    done = true;
                }
            }

            eventOpt = window.pollEvent();
        }

        window.clear(sf::Color(31, 31, 31));
        window.draw(title);
        for(const auto& t : rows)
        {
            window.draw(t);
        }
        window.draw(footer);
        window.display();
    }

    // Start a new run
    state = GameState::NameEntry;
}
