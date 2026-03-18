#pragma once

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

constexpr int MAX_NAME_LENGTH = 40;
constexpr char FONT_PATH[] = "fonts/RobotoSlab-Medium.ttf";

constexpr int LEVEL_COUNT = 3;

constexpr float GRAVITY = 800.0f;
constexpr float GROUND_HEIGHT = 60.0f;
constexpr float PROJECTILE_RADIUS = 12.0f;
constexpr int BASE_SHOTS_LEVEL1 = 5;
constexpr int SCORE_PER_OBSTACLE = 10;

// Leaderboard configuration
constexpr char LEADERBOARD_DIR[] = "data";
constexpr char LEADERBOARD_FILE[] = "data/leaderboard.csv";
constexpr int MAX_LEADERBOARD_RECORDS = 20;
constexpr int MAX_LEADERBOARD_DISPLAY = 10;

// Launch/drag configuration
constexpr float MAX_PULL_RADIUS = 140.0f;
constexpr float MAX_LAUNCH_SPEED = 900.0f;
constexpr float MIN_PULL_DISTANCE = 10.0f;
