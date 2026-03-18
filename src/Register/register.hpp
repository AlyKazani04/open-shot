#pragma once

#include "constants.hpp"
#include <iostream>
#include "Game/gamestate.hpp"
#include <SFML/Graphics.hpp>

void nameEntry(sf::RenderWindow& window, GameState& state, std::string& playerName);

void showLeaderboard(sf::RenderWindow& window, GameState& state);