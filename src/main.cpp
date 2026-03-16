#include <SFML/Graphics.hpp>
#include <opencv4/opencv2/opencv.hpp>

int main() {

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) return -1;
    
    sf::Texture texture({640, 480});
    sf::Sprite sprite(texture);

    // 3. Create SFML Window
    sf::RenderWindow window(sf::VideoMode({640, 480}), "OpenCV + SFML Project");

    cv::Mat frame, frameRGB;

    while (window.isOpen()) {
        
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, frameRGB, cv::ColorConversionCodes::COLOR_BGR2RGBA);

        texture.update(frameRGB.data);

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
