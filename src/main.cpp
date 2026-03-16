#include <SFML/Graphics.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include "Game/GameState.hpp"

// Forward declarations of state handling
void handleNameEntry(sf::RenderWindow& window, GameState& state, std::string& playerName);
void handleWebcamCapture(sf::RenderWindow& window, GameState& state, cv::Mat& playerImage);
void handlePlaying(sf::RenderWindow& window, GameState& state, const std::string& playerName, const cv::Mat& playerImage, int& score);
void handleLeaderboard(sf::RenderWindow& window, GameState& state);

int main() {
    sf::RenderWindow window(sf::VideoMode({640, 480}), "OpenShot");
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
                if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
                    state = GameState::Exiting;
                    return;
                } else if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
                    if (!playerName.empty()) {
                        done = true;
                    }
                } else if (eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Backspace) {
                    if (!playerName.empty()) playerName.pop_back();
                }
            } else if (eventOpt->is<sf::Event::TextEntered>()) {
                char c = static_cast<char>(eventOpt->getIf<sf::Event::TextEntered>()->unicode);
                // Accept only printable ascii, ignore control chars
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
    // TODO: implement webcam image capture with round overlay
    state = GameState::Playing; // advance for initial scaffolding
}

void handlePlaying(sf::RenderWindow& window, GameState& state, const std::string& playerName, const cv::Mat& playerImage, int& score) {
    // TODO: implement main game UI, throwing and obstacles
    state = GameState::Leaderboard; // advance for initial scaffolding
}

void handleLeaderboard(sf::RenderWindow& window, GameState& state) {
    // TODO: implement leaderboard display and CSV I/O
    state = GameState::Exiting;
}

