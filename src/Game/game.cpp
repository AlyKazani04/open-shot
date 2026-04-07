#include "game.hpp"
#include "Game/gamestate.hpp"
#include "constants.hpp"

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
                    m_state = GameState::Leaderboard;
                }
                break;
            case GameState::Leaderboard:
                showLeaderboard(m_window, m_state, m_playerName, m_score);
                break;
            case GameState::GameEnd:
              // TODO: Add Game End Screen (prompts for game restart or exit). It is an overlay over the game setting.
              // Flickering/Blinking GameOver and Score displayed on the screen.
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
