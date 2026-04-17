#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "include/Timer.h"

using namespace sf;

RenderWindow window;
const int window_width = 1600;
const int window_height = 900;
const char *window_title = "Pathfinder";

const int MAX_FPS = 99999;

const float PI = 3.1415926354f;

View camera, canvas;

Vector2f canvas_mouse_position, world_mouse_position;
Color background_color = Color::Black;

float FPS_timer = 1.0f;
int current_FPS;
int FPS = 60;

#include "include/Application.h"

int main()
{
    srand(time(0));
    
    window = RenderWindow(VideoMode({window_width, window_height}), window_title, Style::Close);
    window.setFramerateLimit(FPS);
    
    canvas = window.getView();
    camera = View(Vector2f(0, 0), Vector2f(window_width, window_height));
    
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
        if(Timer::deltaTime >= 1.0f / (FPS + 1))
        {
            Timer::Reset();

            FPS_timer += Timer::deltaTime;
            if(FPS_timer >= 1.0f)
            {
                current_FPS = 1.0f / Timer::deltaTime;
                FPS_timer = 0.0f; 
            }

            canvas_mouse_position = window.mapPixelToCoords(Mouse::getPosition(window), canvas);
            world_mouse_position = window.mapPixelToCoords(Mouse::getPosition(window), camera);

            ImGui::SFML::Update(window, deltaClock.restart());
            Update();

            window.clear(background_color);
            Draw();
            ImGui::SFML::Render(window);
            window.display();
        }
    }

    ImGui::SFML::Shutdown();
}