#pragma once

#include "GamePlay/play.hpp"
#include "Register/register.hpp"
#include <SFML/Graphics.hpp>

class Game
{
    private:
        sf::RenderWindow m_window;
        GameState m_state;
        int m_score;
        std::string m_playerName;
    public:
        Game();

        void run();
        
        ~Game();
};