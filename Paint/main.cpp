#include <SFML/Graphics.hpp>
#include <cmath>
#include "include/Timer.h"

using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "Template";

const int FPS = 60;

const float PI = 3.1415926354f;

const sf::Color BackgroundColor = Color(123, 123, 123);

View camera;
View canvas;

Vector2f canvasMousePosition;
Vector2f worldMousePosition;

#include "include/Scene.h"

int main()
{
    window = RenderWindow(VideoMode({window_width, window_height}), window_title);
    
    Timer::GetInstance();

    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));
    
    Start();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }
        
        Timer::Tick();
        if(Timer::deltaTime >= 1.0f / FPS)
        {
            Timer::Reset();

            canvasMousePosition = window.mapPixelToCoords(Mouse::getPosition(window), canvas);
            worldMousePosition = window.mapPixelToCoords(Mouse::getPosition(window), camera);

            Update();
            window.clear(BackgroundColor);
            Draw();
            window.display();
        }
    }
}