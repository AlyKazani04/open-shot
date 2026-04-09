#pragma once

#include "constants.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include "Game/gamestate.hpp"
#include <SFML/Graphics.hpp>

void nameEntry(sf::RenderWindow& window, GameState& state, std::string& playerName);

// Persist a finished game session locally for later upload
void saveSessionResult(const std::string& playerName, int score);
