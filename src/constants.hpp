#pragma once

constexpr int SCREEN_WIDTH = 960;
constexpr int SCREEN_HEIGHT = 720;

// Design-time reference resolution for scaling
constexpr float DESIGN_WIDTH = 960.0f;
constexpr float DESIGN_HEIGHT = 720.0f;

inline float scaleX()
{
    return static_cast<float>(SCREEN_WIDTH) / DESIGN_WIDTH;
}

inline float scaleY()
{
    return static_cast<float>(SCREEN_HEIGHT) / DESIGN_HEIGHT;
}

inline float scaleUniform()
{
    float sx = scaleX();
    float sy = scaleY();
    return (sx < sy) ? sx : sy;
}

constexpr int MAX_NAME_LENGTH = 40;
constexpr char FONT_PATH[] = "fonts/RobotoSlab-Medium.ttf";

constexpr char SLIPPER_TEXTURE_PATH[] = "assets/slipper.png";
constexpr char ANT_TEXTURE_PATH[] = "assets/ant.png";

constexpr int LEVEL_COUNT = 3;

constexpr float GRAVITY = 800.0f;
constexpr float GROUND_HEIGHT = 60.0f;      // design units; scale with scaleY()
constexpr float PROJECTILE_RADIUS = 12.0f;  // design units; scale with scaleUniform()
constexpr int BASE_SHOTS_LEVEL1 = 5;
constexpr int SCORE_PER_OBSTACLE = 10;

// Leaderboard / session data configuration
constexpr char LEADERBOARD_DIR[] = "data";
constexpr int MAX_LEADERBOARD_RECORDS = 20;

// Per-session results for external leaderboard
constexpr char SESSION_RESULTS_FILE[] = "data/session_results.jsonl";
constexpr char GAME_ID[] = "open-shot";

// Launch/drag configuration
constexpr float MAX_PULL_RADIUS = 140.0f;
constexpr float MAX_LAUNCH_SPEED = 900.0f;
constexpr float MIN_PULL_DISTANCE = 10.0f;
