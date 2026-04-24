#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

#include "Input.h"

class ApplicationBase
{
protected:
    RenderWindow window;

    std::string window_title;

    Color background = Color::White;

public:
    ApplicationBase(std::string title, Vector2u size) : window_title(title), window(VideoMode(size), title)
    {
        window.setFramerateLimit(60);
    }

    virtual ~ApplicationBase() = default;

    void Run()
    {
        if (!ImGui::SFML::Init(window))
            return;

        Start();

        Clock deltaClock;

        while(window.isOpen())
        {   
            Input::BeginFrame();
            
            while(auto event = window.pollEvent())
            {
                ImGui::SFML::ProcessEvent(window, *event);

                if(event->is<Event::Closed>())
                    window.close();
                
                Input::FetchInputData(*event);
            }

            float delta_time = deltaClock.restart().asSeconds();

            ImGui::SFML::Update(window, seconds(delta_time));

            Update(delta_time);

            window.clear(background);

            Draw();

            ImGui::SFML::Render(window);
            window.display();
        }

        ImGui::SFML::Shutdown();
    }

    virtual void Start() {}
    virtual void Update(float delta_time) = 0;
    virtual void Draw() {}
};