#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include <fstream>

#include "include/Timer.h"

template<typename T>
using vector2 = std::vector<std::vector<T>>;

using namespace sf;

RenderWindow window;
const int window_width = 1000;
const int window_height = 800;
const char *window_title = "Pathfinding Vizualisation";
const float PI = 3.1415926354f;
const int MAX_FPS = 999999;

int FPS = 60;
Color BackgroundColor = Color::Black;
Font font_jetBrains;

View camera;
View canvas;

Vector2f canvasMousePosition;
Vector2f worldMousePosition;

float fps_timer = 0;
int current_FPS;
bool dragging = false;
Vector2i mouse_start_drag;
Vector2i window_start_drag;

#include "include/Core.h"

int main() 
{
    srand(time(0));
    window = RenderWindow(VideoMode({window_width, window_height}), window_title, Style::None);
    window.setFramerateLimit(FPS);

    if(!font_jetBrains.openFromFile("resources/JetBrainsMono-Regular.ttf"))
        return 1;

    if(!LoadResources())
        return 1;

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

            if(const auto mouse_pressed = event->getIf<Event::MouseButtonReleased>())
            {
                if(mouse_pressed->button == Mouse::Button::Left)
                    dragging = false;
            }
        }

        if(dragging)
        {
            Vector2i delta = Mouse::getPosition() - mouse_start_drag;
            window.setPosition(window_start_drag + delta);
        }

        Timer::Tick();
        if(Timer::deltaTime >= 1.0f / (FPS + 1))
        {
            Timer::Reset();

            fps_timer += Timer::deltaTime;

            if(fps_timer >= 1.0f)
            {
                current_FPS = 1.0f / Timer::deltaTime;
                fps_timer = 0;
            }

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
    delete algorithm;
}