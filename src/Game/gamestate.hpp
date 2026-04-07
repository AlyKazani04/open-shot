#pragma once

// High-level game states for Open Shot

enum class GameState {
    NameEntry,
    LevelIntro,
    Playing,
    LevelComplete,
    Leaderboard,
    GameEnd,
    Exiting
};
