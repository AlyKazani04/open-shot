#include "game.hpp"
#include "Game/gamestate.hpp"
#include "constants.hpp"

namespace
{
    void showGameEndScreen(sf::RenderWindow& window,
                           GameState& state,
                           const std::string& playerName,
                           int score)
    {
        (void)playerName; // currently unused, kept for potential future use

        const float su = scaleUniform();

        sf::Font font;
        if(!font.openFromFile(FONT_PATH))
        {
            std::cerr << "[ERROR] Missing font: " << FONT_PATH << std::endl;
            state = GameState::Exiting;
            return;
        }
        font.setSmooth(false);

        // Title (game name)
        unsigned int titleSize = static_cast<unsigned int>(42.0f * su);
        if(titleSize < 24u)
        {
            titleSize = 24u;
        }
        sf::Text title(font, "Open Shot", titleSize);
        title.setFillColor(sf::Color(230, 230, 250));
        title.setPosition({
            static_cast<float>(SCREEN_WIDTH) / 2.0f - title.getLocalBounds().size.x / 2.0f,
            40.0f});

        // Blinking GAME OVER text
        unsigned int goSize = static_cast<unsigned int>(36.0f * su);
        if(goSize < 22u)
        {
            goSize = 22u;
        }
        sf::Text gameOver(font, "GAME OVER", goSize);
        gameOver.setFillColor(sf::Color::White);
        gameOver.setPosition({
            static_cast<float>(SCREEN_WIDTH) / 2.0f - gameOver.getLocalBounds().size.x / 2.0f,
            static_cast<float>(SCREEN_HEIGHT) / 2.0f - 60.0f});

        // Score text
        unsigned int scoreSize = static_cast<unsigned int>(28.0f * su);
        if(scoreSize < 18u)
        {
            scoreSize = 18u;
        }
        sf::Text scoreText(font, "Score: " + std::to_string(score), scoreSize);
        scoreText.setFillColor(sf::Color(230, 230, 230));
        scoreText.setPosition({
            static_cast<float>(SCREEN_WIDTH) / 2.0f - scoreText.getLocalBounds().size.x / 2.0f,
            static_cast<float>(SCREEN_HEIGHT) / 2.0f});

        // Buttons
        unsigned int buttonSize = static_cast<unsigned int>(26.0f * su);
        if(buttonSize < 18u)
        {
            buttonSize = 18u;
        }

        sf::Text playAgain(font, "Play Again", buttonSize);
        sf::Text exitText(font, "Exit", buttonSize);

        float buttonY = static_cast<float>(SCREEN_HEIGHT) - 140.0f;

        playAgain.setPosition({
            static_cast<float>(SCREEN_WIDTH) / 2.0f - playAgain.getLocalBounds().size.x - 40.0f,
            buttonY});
        exitText.setPosition({
            static_cast<float>(SCREEN_WIDTH) / 2.0f + 40.0f,
            buttonY});

        bool hoverPlay = false;
        bool hoverExit = false;
        bool showGameOver = true;

        sf::Clock blinkClock;
        const sf::Time blinkInterval = sf::milliseconds(500);

        bool running = true;
        while(running && window.isOpen() && state == GameState::GameEnd)
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
                    if(key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Space)
                    {
                        state = GameState::NameEntry;
                        running = false;
                    }
                }
                else if(eventOpt->is<sf::Event::MouseMoved>())
                {
                    sf::Vector2i mp = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos(static_cast<float>(mp.x), static_cast<float>(mp.y));
                    hoverPlay = playAgain.getGlobalBounds().contains(mousePos);
                    hoverExit = exitText.getGlobalBounds().contains(mousePos);
                }
                else if(eventOpt->is<sf::Event::MouseButtonPressed>())
                {
                    auto button = eventOpt->getIf<sf::Event::MouseButtonPressed>()->button;
                    if(button == sf::Mouse::Button::Left)
                    {
                        sf::Vector2i mp = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePos(static_cast<float>(mp.x), static_cast<float>(mp.y));
                        if(playAgain.getGlobalBounds().contains(mousePos))
                        {
                            state = GameState::NameEntry;
                            running = false;
                        }
                        else if(exitText.getGlobalBounds().contains(mousePos))
                        {
                            state = GameState::Exiting;
                            return;
                        }
                    }
                }

                eventOpt = window.pollEvent();
            }

            // Blink GAME OVER text
            if(blinkClock.getElapsedTime() > blinkInterval)
            {
                showGameOver = !showGameOver;
                blinkClock.restart();
            }

            playAgain.setFillColor(hoverPlay ? sf::Color(255, 230, 120) : sf::Color::White);
            exitText.setFillColor(hoverExit ? sf::Color(255, 230, 120) : sf::Color::White);

            window.clear(sf::Color(20, 20, 20));
            window.draw(title);
            if(showGameOver)
            {
                window.draw(gameOver);
            }
            window.draw(scoreText);
            window.draw(playAgain);
            window.draw(exitText);
            window.display();
        }
    }
}

Game::Game() :
    m_window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "OpenShot"),
    m_state(GameState::NameEntry),
    m_score(0),
    m_currentLevel(1),
    m_maxLevels(LEVEL_COUNT)
{
}

void Game::run()
{
    while(m_window.isOpen() && m_state != GameState::Exiting)
    {
        switch(m_state)
        {
            case GameState::NameEntry:
                m_score = 0;
                nameEntry(m_window, m_state, m_playerName);
                if(m_state == GameState::Playing)
                {
                    m_currentLevel = 1;
                    m_state = GameState::LevelIntro;
                }
                break;
            case GameState::LevelIntro:
                // For now, immediately start playing the current level.
                m_state = GameState::Playing;
                break;
            case GameState::Playing:
                play(m_window, m_state, m_playerName, m_score, m_currentLevel, m_maxLevels);
                break;
            case GameState::LevelComplete:
                if(m_currentLevel < m_maxLevels)
                {
                    ++m_currentLevel;
                    m_state = GameState::LevelIntro;
                }
                else
                {
                    saveSessionResult(m_playerName, m_score);
                    m_state = GameState::GameEnd;
                }
                break;
            case GameState::GameEnd:
                showGameEndScreen(m_window, m_state, m_playerName, m_score);
                break;
            default:
                break;
        }
    }
}

Game::~Game()
{
    m_window.close();
}
