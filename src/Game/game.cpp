#include "game.hpp"

Game::Game() : 
    m_window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "OpenShot"), m_state(GameState::NameEntry), m_score(0)
{ 

}

void Game::run()
{
    while(m_window.isOpen() && m_state != GameState::Exiting)
    {
        switch(m_state) 
        {
            case GameState::NameEntry:
                nameEntry(m_window, m_state, m_playerName);
                break;
            case GameState::Playing:
                play(m_window, m_state, m_playerName, m_score);
                break;
            case GameState::Leaderboard:
                showLeaderboard(m_window, m_state);
                break;
            default: break;
        }
    }
}

Game::~Game()
{
    m_window.close();
}