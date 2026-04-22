#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <stack>
#include <queue>
#include <algorithm>

#include "ApplicationBase.h"
#include "Utils.h"

#include "Grid/Grid.h"
#include "Grid/GridColorTheme.h"
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
};

#include "Algorithms/PathAlgorithms/PathAlgorithm.h"
#include "Algorithms/MazeAlgorithms/MazeAlgorithm.h"
#include "Modules/Module.h"

#include "UI/InterfaceManager.h"

class Application : public ApplicationBase
{
public:
    Application()
        : ApplicationBase("MPGrid", Vector2u(1600, 900)) {}

    Grid grid;
    GridRenderer grid_render;
    GridCursor grid_cursor;
    Interface interface;

    ApplicationContext context
    { 
        .window = window, 
        .grid = grid, 
        .grid_render = grid_render, 
        .grid_cursor = grid_cursor, 
        .interface = interface
    };

    Module *active_module;
    Topo topo;
    Pathfinder pathfinder;
    Mazer mazer;

    InterfaceManager UIManager;

    void Start() override;
    void Update(float delta_time) override;
    void Draw() override;
};

void Application::Start()
{
    srand(time(0));
    window.setFramerateLimit(99999);
    background = Color::Black;

    grid.Create(100, 100);

    Vector2f cell_size(float(window.getSize().y - 50) / grid.GetSize().x, float(window.getSize().y - 50) / grid.GetSize().y);

    grid_render.Build(cell_size, grid.GetSize()); 
    grid_render.SetPosition(Vector2f(window.getSize().x / 2 - grid_render.GetLength().x / 2, window.getSize().y / 2 - grid_render.GetLength().y / 2));

    grid_cursor.Init(cell_size);

    active_module = &topo;
    active_module->Start(context);

    interface.SetSettingsWindow(Vector2f(100, 100), Vector2f(window.getSize().x * 0.4f, window.getSize().y * 0.5f));
    interface.SetSidebarWindow(window.getSize().x * 0.2f);
}

void Application::Update(float delta_time)
{
    context.delta_time = delta_time;

    if(Input::IsKeyDown(Keyboard::Key::Num1))
    {
        active_module = &topo;
        active_module->Init(context);
    }
    else if(Input::IsKeyDown(Keyboard::Key::Num2))
    {
        active_module = &pathfinder;
        active_module->Init(context);
    }
    else if(Input::IsKeyDown(Keyboard::Key::Num3))
    {
        active_module = &mazer;
        active_module->Init(context);
    }
    
    active_module->Update(context);
    grid_render.Update(grid);
    UIManager.Update(context, *active_module);
}

void Application::Draw()
{
    grid_render.Draw(window);
    grid_cursor.Draw(window, grid_render);
    active_module->Draw(context);
}