#pragma once

#include <SFML/Graphics.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include "Registration/register.hpp"

class Game {
    private:

    public:
        Game() =delete;
        Game(const Register& r);

        void run();
        
        ~Game();
};