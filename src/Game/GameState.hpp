#pragma once

// High-level game states for Open Shot
// Extend as needed for future features

enum class GameState {
    NameEntry,
    WebcamCapture,
    Playing,
    Leaderboard,
    Exiting
};
