#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
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

CircleShape circle;
int circle_segments = 30;

void Start()
{
    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    circle = CircleShape(100, circle_segments); 
    circle.setFillColor(Color::Red);
}

void Update()
{
    ImGui::Begin("WOW");
    ImGui::Text("THIS IS AMAZING!");
    ImGui::SliderInt("Segments:", &circle_segments, 3, 10);
    ImGui::End();

    circle.setPointCount(circle_segments);
}

void Draw()
{
    window.setView(camera);
    window.draw(circle);
    window.setView(canvas);
}

int main()
{
    window = RenderWindow(VideoMode({window_width, window_height}), window_title);
    window.setFramerateLimit(FPS);
    
    if (!ImGui::SFML::Init(window)) { return -1; }

    Timer::GetInstance();
    Start();
    
    Clock deltaClock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);
            
            if (event->is<Event::Closed>())
                window.close();
        }
        
        Timer::Tick();
        if(Timer::deltaTime >= 1.0f / FPS)
        {
            Timer::Reset();

            canvasMousePosition = window.mapPixelToCoords(Mouse::getPosition(window), canvas);
            worldMousePosition = window.mapPixelToCoords(Mouse::getPosition(window), camera);

            ImGui::SFML::Update(window, deltaClock.restart());
            Update();

            window.clear(BackgroundColor);
            Draw();
            ImGui::SFML::Render(window);
            window.display();
        }
    }

    ImGui::SFML::Shutdown();
}