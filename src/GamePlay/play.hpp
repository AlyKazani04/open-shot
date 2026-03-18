#pragma once

#include "constants.hpp"
#include "Game/gamestate.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>

void play(sf::RenderWindow& window,
          GameState& state,
          const std::string& playerName,
          int& score,
          int levelNumber,
          int maxLevels);
