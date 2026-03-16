#include <SFML/Graphics.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include "Game/GameState.hpp"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
// Forward declarations of state handling
void handleNameEntry(sf::RenderWindow& window, GameState& state, std::string& playerName);
void handleWebcamCapture(sf::RenderWindow& window, GameState& state, cv::Mat& playerImage);
void handlePlaying(sf::RenderWindow& window, GameState& state, const std::string& playerName, const cv::Mat& playerImage, int& score);
void handleLeaderboard(sf::RenderWindow& window, GameState& state);

int main() {
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "OpenShot");
    GameState state = GameState::NameEntry;

    std::string playerName;
    cv::Mat playerImage;
    int score = 0;

    while (window.isOpen() && state != GameState::Exiting) {
        switch (state) {
            case GameState::NameEntry:
                handleNameEntry(window, state, playerName);
                break;
            case GameState::WebcamCapture:
                handleWebcamCapture(window, state, playerImage);
                break;
            case GameState::Playing:
                handlePlaying(window, state, playerName, playerImage, score);
                break;
            case GameState::Leaderboard:
                handleLeaderboard(window, state);
                break;
            default:
                window.close();
                break;
        }
    }
    return 0;
}

// --- State handler stubs (to be implemented step by step) ---
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <chrono>

constexpr int MAX_NAME_LENGTH = 40;
constexpr char FONT_PATH[] = "fonts/RobotoSlab-Medium.ttf";

void handleNameEntry(sf::RenderWindow& window, GameState& state, std::string& playerName) {
    playerName.clear();
    sf::Font font;
    if (!font.openFromFile(FONT_PATH)) {
        // Draw an error message in default font and wait for user quit
        while (window.isOpen()) {
            while (auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>() ||
                    (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
                    window.close();
            }
            window.clear({30,10,10});
            sf::Text errText(font, std::string("Missing font: ") + FONT_PATH, 22);
            errText.setFillColor(sf::Color::White);
            errText.setPosition(sf::Vector2f(30.f,100.f));
            window.draw(errText);
            window.display();
        }
        state = GameState::Exiting;
        return;
    }
    bool done = false;
    bool showCursor = true;
    auto lastBlink = std::chrono::steady_clock::now();
    sf::Text prompt(font, "Enter your name (max 40 chars):", 26);
    prompt.setFillColor(sf::Color(220,220,255));
    prompt.setPosition(sf::Vector2f(40.f, 120.f));

    sf::Text nameDisplay(font, "", 36);
    nameDisplay.setFillColor(sf::Color::White);
    nameDisplay.setPosition(sf::Vector2f(40.f, 200.f));

    sf::Text info(font, "Press ENTER to confirm, ESC to quit.", 18);
    info.setFillColor(sf::Color(180,180,180));
    info.setPosition(sf::Vector2f(40.f, 240.f));

    while (!done && window.isOpen()) {
        auto eventOpt = window.pollEvent();
        while (eventOpt) {
            if (eventOpt->is<sf::Event::Closed>()) {
                window.close();
                state = GameState::Exiting;
                return;
            }
            if (eventOpt->is<sf::Event::KeyPressed>()) {
                auto key = eventOpt->getIf<sf::Event::KeyPressed>()->code;
                if (key == sf::Keyboard::Key::Escape) {
                    state = GameState::Exiting;
                    return;
                } else if (key == sf::Keyboard::Key::Enter) {
                    if (!playerName.empty()) {
                        done = true;
                    }
                } else if (key == sf::Keyboard::Key::Backspace) {
                    if (!playerName.empty()) playerName.pop_back();
                }
            }
            else if (eventOpt->is<sf::Event::TextEntered>()) {
                char c = static_cast<char>(eventOpt->getIf<sf::Event::TextEntered>()->unicode);
                if (c >= 32 && c < 127 && (int)playerName.size() < MAX_NAME_LENGTH) {
                    playerName += c;
                }
            }
            eventOpt = window.pollEvent();
        }
        // blinking cursor logic (0.5s)
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlink).count() > 500) {
            showCursor = !showCursor;
            lastBlink = now;
        }
        std::string display = playerName;
        if (showCursor) display += "_";
        nameDisplay.setString(display);

        window.clear(sf::Color(20,40,70));
        window.draw(prompt);
        window.draw(nameDisplay);
        window.draw(info);
        window.display();
    }
    state = GameState::WebcamCapture;
}

void handleWebcamCapture(sf::RenderWindow& window, GameState& state, cv::Mat& capturedImage) {
    sf::Font font;
    if (!font.openFromFile(FONT_PATH)) {
        // Fatal error: Font not found close window and log to console
        while (window.isOpen()) {
            while (auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>() || (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
                    std::cerr << "[Error] Font not Found! " << FONT_PATH << std::endl;
                    window.close();
            }
        }
        state = GameState::Exiting;
        return;
    }

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        // Camera error: Display overlay error window
        bool quit = false;
        while (window.isOpen() && !quit) {
            while (auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>() || (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                    window.close(); quit = true; break;
                }
            }
            window.clear(sf::Color(30,30,50));
            sf::Text errorMsg(font, "Error: No webcam detected! Press Escape to exit.", 22);
            errorMsg.setPosition({24, window.getSize().y/2 - 24});
            errorMsg.setFillColor(sf::Color::Red);
            window.draw(errorMsg);
            window.display();
        }
        state = GameState::Exiting;
        return;
    }

    // Webcam streaming loop
    sf::Texture camTexture;
    sf::Sprite camSprite(camTexture);
    bool captured = false;
    std::string instructions = "Center your face and press Enter to capture. Esc: Quit";
    
    while (window.isOpen() && !captured) {
        // Poll events
        while (auto eventOpt = window.pollEvent()) {
            if (!eventOpt) continue;
            if (eventOpt->is<sf::Event::Closed>()) { window.close(); return; }
            if (eventOpt->is<sf::Event::KeyPressed>()) {
                auto key = eventOpt->getIf<sf::Event::KeyPressed>()->code;
                if (key == sf::Keyboard::Key::Escape) {
                    state = GameState::Exiting; return;
                }
                if (key == sf::Keyboard::Key::Enter) {
                    // Confirm capture
                    captured = true;
                }
            }
        }
        
        // Read webcam frame
        cv::Mat frame;
        if (!cap.read(frame)) {
            // Camera died mid-way
            sf::Text err(font, "Camera error! Press Escape to exit.", 20);
            err.setFillColor(sf::Color::Red);
            err.setPosition({30, window.getSize().y/2});
            window.clear(sf::Color(30,20,20));
            window.draw(err);
            window.display();
            sf::sleep(sf::milliseconds(800));
            continue;
        }

        // Convert BGR OpenCV -> RGBA SFML
        cv::Mat rgba;
        cv::cvtColor(frame, rgba, cv::COLOR_BGR2RGBA);
        // Flip row order (OpenCV has 0,0 at top-left, matches SFML)
        // Resize to fit view if needed
        int targetW = window.getSize().x; int targetH = window.getSize().y - 80;
        cv::resize(rgba, rgba, cv::Size(targetW, targetH));
        camTexture.resize({rgba.cols, rgba.rows});
        camTexture.update(rgba.data);
        camSprite.setTexture(camTexture, true);
        camSprite.setPosition({0, 40}); // Leave space for UI

        // Begin drawing
        window.clear(sf::Color(20,40,70));
        window.draw(camSprite);
        // Draw overlay circle
        float radius = std::min(targetW, targetH) * 0.33f;
        sf::CircleShape overlay(radius);
        overlay.setFillColor(sf::Color(0,0,0,0));
        overlay.setOutlineThickness(6);
        overlay.setOutlineColor(sf::Color(224,240,255,130));
        overlay.setPosition({targetW/2-radius, 40+targetH/2-radius});
        window.draw(overlay);
        // Draw instructions
        sf::Text info(font, instructions, 22);
        info.setPosition({32, 2});
        info.setFillColor(sf::Color(245,245,240));
        window.draw(info);
        window.display();
    }

    // After capture, store image for later avatar use
    cap.release(); // stop camera
    // Crop/mask in next step
    state = GameState::Playing; // for now continue as stub
}


void handlePlaying(sf::RenderWindow& window, GameState& state, const std::string& playerName, const cv::Mat& playerImage, int& score) {
    // TODO: implement main game UI, throwing and obstacles
    state = GameState::Leaderboard; // advance for initial scaffolding
}

void handleLeaderboard(sf::RenderWindow& window, GameState& state) {
    // TODO: implement leaderboard display and CSV I/O
    state = GameState::Exiting;
}

