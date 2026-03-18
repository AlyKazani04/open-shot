#include "play.hpp"

#include <SFML/Graphics/Color.hpp>
#include <optional>

namespace
{
    struct ObstacleConfig
    {
        sf::Vector2f position;
        sf::Vector2f size;
        bool destructible;
        int scoreOnHit;
    };

    struct HillConfig
    {
        sf::Vector2f position;
        sf::Vector2f size;
    };

    struct LevelConfig
    {
        int shots;
        std::vector<ObstacleConfig> obstacles;
        std::vector<HillConfig> hills;
    };

    struct Obstacle
    {
        sf::RectangleShape shape{sf::Vector2f{}};
        std::optional<sf::Sprite> sprite;
        bool alive{};
        bool destructible{};
        int scoreOnHit{};
    };

    struct Projectile
    {
        Projectile() = delete;
        explicit Projectile(const sf::Texture& texture) : sprite(texture) {}

        sf::Sprite sprite;
        sf::Vector2f velocity{};
        bool inFlight{false};
        float rotationSpeed{0.0f};
    };

    LevelConfig getLevelConfig(int levelNumber)
    {
        LevelConfig cfg{};

        switch(levelNumber)
        {
            case 1:
            default:
            {
                cfg.shots = BASE_SHOTS_LEVEL1;

                // Simple hill on the right side to support the targets
                float hillWidth = 160.0f;
                float hillHeight = 80.0f;
                float hillX = 440.0f;
                float hillY = static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT - hillHeight;
                cfg.hills.push_back({{hillX, hillY}, {hillWidth, hillHeight}});

                // Targets resting on top of the hill
                float blockW = 36.0f;
                float blockH = 60.0f;
                float hillTopY = hillY;
                float baseY = hillTopY - blockH / 2.0f;

                cfg.obstacles = {
                    {{hillX + 20.0f, baseY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillX + 60.0f, baseY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillX + 100.0f, baseY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillX + 140.0f, baseY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                };
                break;
            }
            case 2:
            {
                cfg.shots = 5;

                // Same hill as level 1 for targets
                float hillWidth = 160.0f;
                float hillHeight = 80.0f;
                float hillX = 440.0f;
                float hillY = static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT - hillHeight;
                cfg.hills.push_back({{hillX, hillY}, {hillWidth, hillHeight}});

                float blockW = 30.0f;
                float blockH = 60.0f;
                float hillTopY = hillY;
                float baseY = hillTopY - blockH / 2.0f;

                // Destructible targets behind a blocking column
                cfg.obstacles = {
                    // blocking column (non-destructible)
                    {{260.0f, static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT - 50.0f},
                     {40.0f, 100.0f},
                     false,
                     0},
                    // targets on hill behind column
                    {{hillX + 40.0f, baseY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillX + 80.0f, baseY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillX + 120.0f, baseY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                };
                break;
            }
            case 3:
            {
                cfg.shots = 4;

                // Two hills at different distances and heights
                float hillAWidth = 120.0f;
                float hillAHeight = 70.0f;
                float hillAX = 380.0f;
                float hillAY = static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT - hillAHeight;
                cfg.hills.push_back({{hillAX, hillAY}, {hillAWidth, hillAHeight}});

                float hillBWidth = 140.0f;
                float hillBHeight = 110.0f;
                float hillBX = 500.0f;
                float hillBY = static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT - hillBHeight;
                cfg.hills.push_back({{hillBX, hillBY}, {hillBWidth, hillBHeight}});

                float blockW = 26.0f;
                float blockH = 50.0f;

                float hillATopY = hillAY;
                float hillBTopY = hillBY;

                float baseAY = hillATopY - blockH / 2.0f;
                float baseBY = hillBTopY - blockH / 2.0f;

                // Two blocking columns creating a narrow window
                cfg.obstacles = {
                    // front shorter column
                    {{220.0f, static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT - 40.0f},
                     {30.0f, 80.0f},
                     false,
                     0},
                    // second taller column closer to hills
                    {{320.0f, static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT - 70.0f},
                     {30.0f, 140.0f},
                     false,
                     0},

                    // targets on hill A
                    {{hillAX + 30.0f, baseAY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillAX + 70.0f, baseAY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},

                    // targets on hill B, slightly higher and further
                    {{hillBX + 20.0f, baseBY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillBX + 60.0f, baseBY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                    {{hillBX + 100.0f, baseBY}, {blockW, blockH}, true, SCORE_PER_OBSTACLE},
                };
                break;
            }
        }

        return cfg;
    }

    bool allObstaclesCleared(const std::vector<Obstacle>& obstacles)
    {
        for(const auto& o : obstacles)
        {
            if(o.alive && o.destructible)
            {
                return false;
            }
        }
        return true;
    }
}

void play(sf::RenderWindow& window,
          GameState& state,
          const std::string& playerName,
          int& score,
          int levelNumber,
          int maxLevels)
{
    (void)maxLevels;

    sf::Font font;
    if(!font.openFromFile(FONT_PATH))
    {
        std::cerr << "[ERROR] Missing font: " << FONT_PATH << std::endl;
        state = GameState::Exiting;
        return;
    }

    const float groundY = static_cast<float>(SCREEN_HEIGHT) - GROUND_HEIGHT;

    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(SCREEN_WIDTH), GROUND_HEIGHT));
    ground.setFillColor(sf::Color(50, 50, 50));
    ground.setPosition(sf::Vector2f(0.0f, groundY));

    sf::RectangleShape sky(
        sf::Vector2f(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)));
    sky.setFillColor(sf::Color(255, 222, 179));
    sky.setPosition(sf::Vector2f(0.0f, 0.0f));

    sf::Texture slipperTexture;
    if(!slipperTexture.loadFromFile(SLIPPER_TEXTURE_PATH))
    {
        std::cerr << "[ERROR] Missing texture: " << SLIPPER_TEXTURE_PATH << std::endl;
        state = GameState::Exiting;
        return;
    }

    sf::Texture antTexture;
    if(!antTexture.loadFromFile(ANT_TEXTURE_PATH))
    {
        std::cerr << "[ERROR] Missing texture: " << ANT_TEXTURE_PATH << std::endl;
        state = GameState::Exiting;
        return;
    }

    // Launcher base and ball placement
    const sf::Vector2f launcherBaseSize(40.0f, 60.0f);
    const sf::Vector2f launcherBasePos(60.0f,
                                       groundY - launcherBaseSize.y);
    sf::Vector2f launcherCenter(
        launcherBasePos.x + launcherBaseSize.x / 2.0f,
        launcherBasePos.y + launcherBaseSize.y / 2.0f);

    sf::Vector2f ballStart(
        launcherCenter.x - launcherBaseSize.x / 2.0f + PROJECTILE_RADIUS,
        launcherCenter.y - launcherBaseSize.y / 2.0f);

    Projectile projectile(slipperTexture);
    auto slipperSize = slipperTexture.getSize();
    float targetHeight = PROJECTILE_RADIUS * 2.0f;
    float slipperScale = slipperSize.y > 0
                              ? targetHeight / static_cast<float>(slipperSize.y)
                              : 1.0f;
    projectile.sprite.setScale(sf::Vector2f(slipperScale, slipperScale));
    projectile.sprite.setOrigin(sf::Vector2f(static_cast<float>(slipperSize.x) / 2.0f,
                                             static_cast<float>(slipperSize.y) / 2.0f));
    projectile.sprite.setPosition(ballStart);
    projectile.velocity = {0.0f, 0.0f};
    projectile.inFlight = false;
    projectile.rotationSpeed = 180.0f;

    sf::RectangleShape launcherBase(launcherBaseSize);
    launcherBase.setFillColor(sf::Color(120, 80, 40));
    launcherBase.setPosition(launcherBasePos);

    LevelConfig levelCfg = getLevelConfig(levelNumber);
    int remainingShots = levelCfg.shots;

    // Build hills/terrain
    std::vector<sf::RectangleShape> hills;
    hills.reserve(levelCfg.hills.size());
    for(const auto& h : levelCfg.hills)
    {
        sf::RectangleShape hill(h.size);
        hill.setPosition(h.position);
        hill.setFillColor(sf::Color(70, 90, 60));
        hills.push_back(hill);
    }

    std::vector<Obstacle> obstacles;
    obstacles.reserve(levelCfg.obstacles.size());
    for(const auto& cfg : levelCfg.obstacles)
    {
        Obstacle o{};
        o.shape.setSize(cfg.size);
        o.shape.setOrigin(sf::Vector2f(cfg.size.x / 2.0f, cfg.size.y / 2.0f));
        o.shape.setPosition(cfg.position);
        if(cfg.destructible)
        {
            o.shape.setFillColor(sf::Color(120, 170, 210));
            o.sprite.emplace(antTexture);
            auto antSize = antTexture.getSize();
            float targetWidth = cfg.size.x;
            float antScale = antSize.x > 0
                                 ? targetWidth / static_cast<float>(antSize.x)
                                 : 1.0f;
            o.sprite->setScale(sf::Vector2f(antScale, antScale));
            o.sprite->setOrigin(sf::Vector2f(static_cast<float>(antSize.x) / 2.0f,
                                             static_cast<float>(antSize.y) / 2.0f));
            o.sprite->setPosition(cfg.position);
        }
        else
        {
            o.shape.setFillColor(sf::Color(90, 100, 120));
        }
        o.alive = true;
        o.destructible = cfg.destructible;
        o.scoreOnHit = cfg.scoreOnHit;
        obstacles.push_back(o);
    }

    sf::Text hudName(font, "Player: " + playerName, 18);
    hudName.setFillColor(sf::Color::Black);
    hudName.setPosition(sf::Vector2f(10.0f, 10.0f));

    sf::Text hudScore(font, "Score: " + std::to_string(score), 18);
    hudScore.setFillColor(sf::Color::Black);
    hudScore.setPosition(sf::Vector2f(10.0f, 34.0f));

    sf::Text hudShots(font, "Shots: " + std::to_string(remainingShots), 18);
    hudShots.setFillColor(sf::Color::Black);
    hudShots.setPosition(sf::Vector2f(10.0f, 58.0f));

    sf::Text hudLevel(font,
                      "Level: " + std::to_string(levelNumber) + "/" + std::to_string(maxLevels),
                      18);
    hudLevel.setFillColor(sf::Color::Black);
    hudLevel.setPosition(sf::Vector2f(static_cast<float>(SCREEN_WIDTH) -
                                          hudLevel.getLocalBounds().size.x - 10.0f,
                                      10.0f));

    sf::Text hudHelp(font, "Drag and release to shoot. ESC to quit.", 16);
    hudHelp.setFillColor(sf::Color(180, 180, 180));
    hudHelp.setPosition(sf::Vector2f(10.0f, static_cast<float>(SCREEN_HEIGHT) - 30.0f));

    bool dragging = false;
    sf::Vector2f dragStart = ballStart;
    sf::Vector2f dragCurrent = ballStart;

    sf::Clock clock;

    bool running = true;
    while(running && window.isOpen())
    {
        auto eventOpt = window.pollEvent();
        while(eventOpt)
        {
            if(eventOpt->is<sf::Event::Closed>())
            {
                state = GameState::Exiting;
                return;
            }

            if(eventOpt->is<sf::Event::KeyPressed>())
            {
                auto key = eventOpt->getIf<sf::Event::KeyPressed>()->code;
                if(key == sf::Keyboard::Key::Escape)
                {
                    state = GameState::Exiting;
                    return;
                }
            }

            if(eventOpt->is<sf::Event::MouseButtonPressed>())
            {
                auto button = eventOpt->getIf<sf::Event::MouseButtonPressed>()->button;
                if(button == sf::Mouse::Button::Left && !projectile.inFlight && remainingShots > 0)
                {
                    sf::Vector2i mp = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos(static_cast<float>(mp.x), static_cast<float>(mp.y));
                    if(projectile.sprite.getGlobalBounds().contains(mousePos))
                    {
                        dragging = true;
                        dragStart = projectile.sprite.getPosition();
                        dragCurrent = mousePos;
                    }
                }
            }
            else if(eventOpt->is<sf::Event::MouseButtonReleased>())
            {
                auto button = eventOpt->getIf<sf::Event::MouseButtonReleased>()->button;
                if(button == sf::Mouse::Button::Left && dragging)
                {
                    sf::Vector2f dragVec = dragStart - dragCurrent;
                    float length = std::sqrt(dragVec.x * dragVec.x + dragVec.y * dragVec.y);
                    float pull = std::min(length, MAX_PULL_RADIUS);
                    if(pull >= MIN_PULL_DISTANCE)
                    {
                        float t = (MAX_PULL_RADIUS > 0.0f) ? (pull / MAX_PULL_RADIUS) : 0.0f;
                        float speed = t * MAX_LAUNCH_SPEED;
                        sf::Vector2f dir = {dragVec.x / (length > 0.0f ? length : 1.0f),
                                            dragVec.y / (length > 0.0f ? length : 1.0f)};
                        projectile.velocity = {dir.x * speed, dir.y * speed};
                        projectile.inFlight = true;
                        --remainingShots;
                        hudShots.setString("Shots: " + std::to_string(remainingShots));
                    }

                    dragging = false;
                }
            }
            else if(eventOpt->is<sf::Event::MouseMoved>() && dragging)
            {
                sf::Vector2i mp = sf::Mouse::getPosition(window);
                dragCurrent = sf::Vector2f(static_cast<float>(mp.x), static_cast<float>(mp.y));
            }

            eventOpt = window.pollEvent();
        }

        float dt = clock.restart().asSeconds();

        if(projectile.inFlight)
        {
            projectile.velocity.y += GRAVITY * dt;

            sf::Vector2f pos = projectile.sprite.getPosition();
            pos += projectile.velocity * dt;

            if(pos.y + PROJECTILE_RADIUS >= groundY)
            {
                pos.y = groundY - PROJECTILE_RADIUS;
                projectile.inFlight = false;
                projectile.velocity = {0.0f, 0.0f};
                projectile.sprite.setPosition(ballStart);
                projectile.sprite.setRotation(sf::degrees(0.0f));
            }
            else if(pos.x < -50.0f || pos.x > static_cast<float>(SCREEN_WIDTH) + 50.0f ||
                    pos.y < -50.0f)
            {
                projectile.inFlight = false;
                projectile.velocity = {0.0f, 0.0f};
                projectile.sprite.setPosition(ballStart);
                projectile.sprite.setRotation(sf::degrees(0.0f));
            }
            else
            {
                projectile.sprite.setPosition(pos);
                projectile.sprite.rotate(sf::degrees(projectile.rotationSpeed * dt));
            }
        }

        if(projectile.inFlight)
        {
            sf::FloatRect projBounds = projectile.sprite.getGlobalBounds();

            // Collide with hills (treat like extra ground)
            for(const auto& hill : hills)
            {
                if(projBounds.findIntersection(hill.getGlobalBounds()).has_value())
                {
                    projectile.inFlight = false;
                    projectile.velocity = {0.0f, 0.0f};
                    projectile.sprite.setPosition(ballStart);
                    projectile.sprite.setRotation(sf::degrees(0.0f));
                    break;
                }
            }

            if(projectile.inFlight)
            {
                for(auto& o : obstacles)
                {
                    if(!o.alive)
                    {
                        continue;
                    }
                    if(projBounds.findIntersection(o.shape.getGlobalBounds()).has_value())
                    {
                        if(o.destructible)
                        {
                            o.alive = false;
                            score += o.scoreOnHit;
                            hudScore.setString("Score: " + std::to_string(score));
                        }
                        else
                        {
                            // Blocking column: stop and reset ball
                            projectile.inFlight = false;
                            projectile.velocity = {0.0f, 0.0f};
                            projectile.sprite.setPosition(ballStart);
                            projectile.sprite.setRotation(sf::degrees(0.0f));
                        }
                    }
                }
            }
        }

        bool cleared = allObstaclesCleared(obstacles);
        if(cleared || (remainingShots == 0 && !projectile.inFlight))
        {
            state = GameState::LevelComplete;
            running = false;
        }

        window.clear();
        window.draw(sky);
        window.draw(ground);

        for(const auto& hill : hills)
        {
            window.draw(hill);
        }

        for(const auto& o : obstacles)
        {
            if(o.alive)
            {
                if(o.sprite)
                {
                    window.draw(*o.sprite);
                }
                else
                {
                    window.draw(o.shape);
                }
            }
        }

        window.draw(launcherBase);
        window.draw(projectile.sprite);

        if(dragging)
        {
            sf::Vector2f dragVec = dragStart - dragCurrent;
            float length = std::sqrt(dragVec.x * dragVec.x + dragVec.y * dragVec.y);
            float pull = std::min(length, MAX_PULL_RADIUS);

            float t = (MAX_PULL_RADIUS > 0.0f) ? (pull / MAX_PULL_RADIUS) : 0.0f;
            unsigned char alpha = 100;

            sf::Color blue(0, 160, 255, alpha);
            sf::Color yellow(255, 220, 0, alpha);
            sf::Color red(255, 40, 0, alpha);

            sf::Color dragColor;
            if(t <= 0.5f)
            {
                float u = (t / 0.5f);
                dragColor.r = static_cast<unsigned char>(blue.r + (yellow.r - blue.r) * u);
                dragColor.g = static_cast<unsigned char>(blue.g + (yellow.g - blue.g) * u);
                dragColor.b = static_cast<unsigned char>(blue.b + (yellow.b - blue.b) * u);
                dragColor.a = alpha;
            }
            else
            {
                float u = (t - 0.5f) / 0.5f;
                dragColor.r = static_cast<unsigned char>(yellow.r + (red.r - yellow.r) * u);
                dragColor.g = static_cast<unsigned char>(yellow.g + (red.g - yellow.g) * u);
                dragColor.b = static_cast<unsigned char>(yellow.b + (red.b - yellow.b) * u);
                dragColor.a = alpha;
            }

            sf::CircleShape dragCircle(pull);
            dragCircle.setOrigin(sf::Vector2f(pull, pull));
            dragCircle.setPosition(projectile.sprite.getPosition());
            dragCircle.setFillColor(dragColor);
            dragCircle.setOutlineThickness(0.0f);

            window.draw(dragCircle);

            sf::Vertex line[] = {
                {{dragStart.x, dragStart.y}, sf::Color::White},
                {{dragCurrent.x, dragCurrent.y}, sf::Color(200, 200, 255)}};
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        window.draw(hudName);
        window.draw(hudScore);
        window.draw(hudShots);
        window.draw(hudLevel);
        window.draw(hudHelp);

        window.display();
    }
}
