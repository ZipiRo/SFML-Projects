#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <functional>
#include <stack>
#include <queue>
#include <algorithm>
#include <map>
#include <filesystem>
#include <array>

#include "ApplicationBase.h"
#include "Utils.h"

#include "ResourceManager.h" 

#include "SoundPlayer.h"

#include "Grid/Grid.h"
#include "Grid/GridTheme.h"
#include "Grid/GridRenderer.h"
#include "Grid/GridCursor.h"

#include "UI/Interface.h"

struct ApplicationContext
{
    RenderWindow &window;
    Grid &grid;
    GridRenderer &grid_render;
    GridCursor &grid_cursor;
    Interface &interface;
    float delta_time;
    int using_module;
    std::function<void(int)> SetModule;
};

#include "Algorithms/PathAlgorithms/PathAlgorithm.h"
#include "Algorithms/MazeAlgorithms/MazeAlgorithm.h"
#include "Modules/Module.h"

#include "UI/InterfaceManager.h"

class Application : public ApplicationBase
{
private:
    void SetModule(int module)
    {
        using_module = module; 
        init_module = true;
    }

public:
    Application()
        : ApplicationBase("MPGrid", Vector2u(1600, 900)) {}

    Grid grid;
    GridRenderer grid_render;
    GridCursor grid_cursor;
    Interface interface;

    std::vector<Module*> modules;
    int using_module = 0;
    bool init_module = false;

    ApplicationContext context { 
        .window = window, 
        .grid = grid, 
        .grid_render = grid_render, 
        .grid_cursor = grid_cursor, 
        .interface = interface,
        .SetModule = [this](int module)
        {
            using_module = module;
            init_module = true;
        }
    };

    InterfaceManager UIManager;

    void Start() override;
    void Update(float delta_time) override;
    void Draw() override;
};

void Application::Start()
{
    if(!LoadResources()) window.close();

    srand(time(0));
    window.setFramerateLimit(99999);
    background_color = Color::Black;

    grid.Create(20, 20);
    grid_render.SetMaxLength(Vector2f(window.getSize().y - 70, window.getSize().y - 70));
    grid_render.Build(grid.GetSize()); 
    grid_render.SetPosition(Vector2f(window.getSize().x / 2 - grid_render.GetLength().x / 2, window.getSize().y / 2 - grid_render.GetLength().y / 2));
    grid_cursor.Init(grid_render.GetCellSize());

    modules.push_back(new Topo());
    modules.push_back(new Pathfinder());
    modules.push_back(new Mazer());

    modules[using_module]->Start(context);

    interface.SetSettingsWindow(Vector2f(100, 100), Vector2f(window.getSize().x * 0.4f, window.getSize().y * 0.5f));
    interface.SetModulesBar(Vector2f(10, 30), Vector2f(57.5f, 145));
    interface.SetSidebarWindow(window.getSize().x * 0.2f);
}

void Application::Update(float delta_time)
{
    context.delta_time = delta_time;
    context.using_module = using_module;

    UIManager.Update(context, *modules[using_module]);

    if(!(interface.show_settings_window || interface.show_popup))
    {
        if(Input::IsKeyDown(Keyboard::Key::Num1)) SetModule(0);
        else if(Input::IsKeyDown(Keyboard::Key::Num2)) SetModule(1);
        else if(Input::IsKeyDown(Keyboard::Key::Num3)) SetModule(2);

        if(init_module)
        {
            modules[using_module]->Init(context);
            init_module = false;
        }

        modules[using_module]->Update(context);
    }

    grid_render.Update(grid);
}

void Application::Draw()
{
    grid_render.Draw(window);
    grid_cursor.Draw(window, grid_render);
    modules[using_module]->Draw(context);
}