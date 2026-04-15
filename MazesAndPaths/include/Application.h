#include <vector>
#include <iostream>
#include <stack>
#include <queue>
#include <algorithm>
#include <cmath>
#include <functional>

#include "Keybinds.h"

#include "Utils.h"
#include "Input.h"

const float MENU_BAR_HEIGHT = 18;
const float SIDEBAR_WIDTH = 300;

const float DEFAULT_ALGO_STEP_DELAY = 0.05f;
const float DEFAULT_ALGO_START_DELAY = 3.0f;

#include "Grid.h"
#include "Pathfinder.h"
#include "Mazer.h"

enum State
{
    GRID_SETUP_STATE,
    PATH_SETUP_STATE,
    MAZE_SETUP_STATE
};

int state;
bool show_interface = true;

Grid grid;
Pathfinder pathfinder;
Mazer mazer;

void MenuInterface()
{
    if(ImGui::BeginMainMenuBar())
    {
        ImGui::EndMainMenuBar();
    }
}

void Start()
{
    grid = Grid(Vector2i(20, 20), Vector2f(window_height - 50, window_height - 50));
    pathfinder = Pathfinder(&grid);
    mazer = Mazer(&grid);

    grid.Fill(PLACE_ROOM);

    pathfinder.using_algorithm = 1; 

    state = GRID_SETUP_STATE;
}

void Update()
{
    if(Keyboard::isKeyPressed(KEY_GRID_SETUP)) 
    {
        grid.show_sidebar = true;
        grid.show_settings = false;

        pathfinder.Reset();
        mazer.Reset();

        state = GRID_SETUP_STATE;
    }
    else if(Keyboard::isKeyPressed(KEY_PATH_SETUP)) 
    {
        pathfinder.show_sidebar = true;
        pathfinder.show_settings = false;

        mazer.Reset();

        state = PATH_SETUP_STATE;
    }
    else if(Keyboard::isKeyPressed(KEY_MAZE_SETUP)) 
    {
        mazer.show_sidebar = true;
        mazer.show_settings = false;

        pathfinder.Reset();

        state = MAZE_SETUP_STATE;
    }       

    if(IsKeyboardButtonDown(KEY_HIDE_INTERFACE))
        show_interface = !show_interface;
    
    if(show_interface) grid.offset = Vector2f(0, MENU_BAR_HEIGHT);
    else grid.offset = Vector2f(0, 0);
    
    if(show_interface) MenuInterface();

    switch (state)
    {
    case GRID_SETUP_STATE:
        if(show_interface) 
            grid.UserInterface();

        grid.Update();
        break;
    case PATH_SETUP_STATE:
        if(show_interface) 
            pathfinder.UserInterface();

        pathfinder.Update();
        break;
    case MAZE_SETUP_STATE:
        if(show_interface) 
            mazer.UserInterface();

        mazer.Update();
        break;
    default:
        break;
    }
}

void Draw()
{
    window.setView(camera);
    window.setView(canvas);
    grid.Draw();
}