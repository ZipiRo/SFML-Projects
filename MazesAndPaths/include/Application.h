#include <vector>
#include <iostream>
#include <stack>
#include <queue>
#include <algorithm>
#include <cmath>
#include <functional>
#include <fstream>
#include <filesystem>

#include "Keybinds.h"

#include "Utils.h"
#include "Input.h"

const float MENU_BAR_HEIGHT = 18;
const float SIDEBAR_WIDTH = 300;
const Vector2f DEFAULT_GRID_MAX_LENGHT = Vector2f(window_width - 50, window_height - 50);

const float DEFAULT_ALGO_STEP_DELAY = 0.05f;
const float DEFAULT_ALGO_START_DELAY = 3.0f;

const std::string SAVE_FILE_MARK = "@)#GRD";
const std::string SAVE_FILE_EXT = ".grd";
const std::string SAVE_FILE_DIR = "GridSaves"; 
const int SAVE_FILE_MAX_SIZE = 30;

#include "Grid.h"
#include "Pathfinder.h"
#include "Mazer.h"

bool LoadGrid(const std::string &file_name, std::vector<std::vector<int>> &graph)
{
    std::string file = SAVE_FILE_DIR + "\\" + file_name;
    if(file_name.find(SAVE_FILE_EXT) == std::string::npos)
        file += SAVE_FILE_EXT;

    std::ifstream FILE(file);

    char mark[10]; 
    FILE.getline(mark, 10);
    if(std::string(mark).find(SAVE_FILE_MARK) == std::string::npos)
        return false;

    Vector2i size;
    FILE.getline(mark, 10);
    if(std::string(mark).size() > 0) size.x = atoi(mark);
    else return false; 

    FILE.getline(mark, 10);    
    if(std::string(mark).size() > 0) size.y = atoi(mark);
    else return false; 

    graph = std::vector<std::vector<int>>(size.y, std::vector<int>(size.x));

    char buffer[1000];
    for(int i = 0; i < size.y; i++)
    {
        FILE.getline(buffer, 1000);
        for(int j = 0; j < size.x; j++)
        {
            int type = buffer[j] - '0';
            graph[i][j] = type;
        }
    }

    FILE.close();

    return true;
}

void SaveGrid(const Grid &grid, const std::string &file_name)
{    
    if(!std::filesystem::is_directory(SAVE_FILE_DIR))
        std::filesystem::create_directory(SAVE_FILE_DIR);

    std::string file = SAVE_FILE_DIR + '\\' + file_name + SAVE_FILE_EXT;
    std::ofstream FILE(file);

    if(!FILE) return;

    FILE << SAVE_FILE_MARK;
    FILE << '\n';
    FILE << std::to_string(grid.size.x);
    FILE << '\n';
    FILE << std::to_string(grid.size.y);
    FILE << '\n';
    for(int i = 0; i < grid.size.y; i++)
    {
        for(int j = 0; j < grid.size.x; j++)
            FILE << grid.graph[i][j].type;
        FILE << '\n';
    }

    FILE.close();
}

enum ProgramState
{
    GRID_SETUP_STATE,
    PATH_SETUP_STATE,
    MAZE_SETUP_STATE
};

enum ApplicationState
{
    NOTHING_STATE,
    LOAD_GRID_STATE,
    SAVE_GRID_STATE
};

int prog_state;
int app_state;
bool show_interface = true;

Grid grid;
Pathfinder pathfinder;
Mazer mazer;

void CenteredPopup(const std::string &title, const Vector2i &size, const std::function<void()> Interface)
{
    bool open = true;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;
    
    ImGui::SetNextWindowPos(ImVec2(window_width / 2, window_height / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y), ImGuiCond_Always);
    ImGui::Begin(title.c_str(), &open, flags);

    if(!open || ImGui::IsKeyDown(ImGuiKey_Escape))
        app_state = NOTHING_STATE;

    Interface();

    ImGui::End();
}

char save_file_name_input[SAVE_FILE_MAX_SIZE];
void LoadInterface()
{
    ImGui::Text(std::string("File Name (" + SAVE_FILE_EXT + ')').c_str());
    ImGui::InputText("##FileName", save_file_name_input, SAVE_FILE_MAX_SIZE);
    ImGui::SameLine();
    if((ImGui::Button("Load (Enter)") || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter)) && !std::string(save_file_name_input).empty())
    {
        std::vector<std::vector<int>> graph;
        if(!LoadGrid(save_file_name_input, graph)) 
            return;

        grid.SetGraph(graph);
        
        app_state = NOTHING_STATE;
    }
}

void SaveInterface()
{
    ImGui::Text(std::string("File Name (" + SAVE_FILE_EXT + ')').c_str());
    ImGui::InputText("##FileName ", save_file_name_input, SAVE_FILE_MAX_SIZE);
    ImGui::SameLine();
    if((ImGui::Button("Save (Enter)") || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter)) && !std::string(save_file_name_input).empty())
    {
        SaveGrid(grid, save_file_name_input);
        app_state = NOTHING_STATE;
    }
}

void MenuInterface()
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Grid"))
        {
            if(ImGui::MenuItem("Load"))
            {
                save_file_name_input[0] = '\0'; 
                app_state = LOAD_GRID_STATE;
            }

            if(ImGui::MenuItem("Save"))
            {
                save_file_name_input[0] = '\0';
                app_state = SAVE_GRID_STATE;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Start()
{
    grid = Grid(Vector2i(20, 20), DEFAULT_GRID_MAX_LENGHT);
    pathfinder = Pathfinder(&grid);
    mazer = Mazer(&grid);

    prog_state = GRID_SETUP_STATE;
}

void Update()
{
    if(show_interface) 
    {
        grid.offset = Vector2f(0, MENU_BAR_HEIGHT);
        MenuInterface();
    }    
    else grid.offset = Vector2f(0, 0);

    switch (app_state)
    {
    case LOAD_GRID_STATE:
        CenteredPopup("Load", Vector2i(400, 80), LoadInterface);
        return;
    case SAVE_GRID_STATE:
        CenteredPopup("Save", Vector2i(400, 80), SaveInterface);
        return;
    default:
        break;
    }
    
    if(IsKeyboardButtonDown(KEY_HIDE_INTERFACE))
    {
        show_interface = !show_interface;
        grid.Start();
        pathfinder.Start();
        mazer.Start();
    }

    if(Keyboard::isKeyPressed(KEY_GRID_SETUP)) 
    {
        grid.Start();
        pathfinder.Reset();
        mazer.Reset();

        prog_state = GRID_SETUP_STATE;
    }
    else if(Keyboard::isKeyPressed(KEY_PATH_SETUP)) 
    {
        pathfinder.Start();
        mazer.Reset();

        prog_state = PATH_SETUP_STATE;
    }
    else if(Keyboard::isKeyPressed(KEY_MAZE_SETUP)) 
    {
        mazer.Start();
        pathfinder.Reset();

        prog_state = MAZE_SETUP_STATE;
    }       
    switch (prog_state)
    {
    case GRID_SETUP_STATE:
        if(show_interface) grid.UserInterface();
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