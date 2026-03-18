#pragma once

#include "constants.hpp"
#include "Game/gamestate.hpp"
#include <SFML/Graphics.hpp>

void play(sf::RenderWindow& window, GameState& state, const std::string& playerName, int& score);