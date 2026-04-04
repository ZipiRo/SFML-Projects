#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
#include <cmath>
#include <iostream>

#include "include/Timer.h"

template<typename T>
using vector2 = std::vector<std::vector<T>>;

using namespace sf;

RenderWindow window;
const int window_width = 950;
const int window_height = 700;
const char *window_title = "Pathfinding Vizualisation";
const int FPS = 120;
const float PI = 3.1415926354f;

Color BackgroundColor = Color::Black;
Font font_jetBrains;

View camera;
View canvas;

Vector2f canvasMousePosition;
Vector2f worldMousePosition;

float fps_timer = 0;
int current_FPS;

#include "include/Input.h"

const Vector2i INFO_W_SIZE = Vector2i(250, window_height);
const Vector2i INFO_W_POS = Vector2i(window_width - INFO_W_SIZE.x, 0);

const int START_GRID_SIZE = 20;
const int MAX_GRID_LENGTH = window_width - INFO_W_SIZE.x - 150;

const Color CELL_ROOM_COLOR = Color(40, 40, 40);
const Color CELL_WALL_COLOR = Color(20, 20, 20);
const Color CELL_FINISH_COLOR = Color(200, 0, 0);
const Color CELL_START_COLOR = Color(0, 200, 0);
const Color CELL_VISITED_COLOR = Color(30, 184, 231);
const Color CELL_FRONTIER_COLOR = Color(237, 241, 14);
const Color CELL_BACKTRACK_COLOR = Color(216, 37, 37);
const Color CELL_PATH_COLOR = Color(30, 231, 64);

const Color HOVER_CURSOR_COLOR = Color(10, 10, 10);
const Color GRID_OUTLINE_COLOR = CELL_WALL_COLOR;

enum CellType
{
    CELL_ROOM,
    CELL_WALL,
    CELL_START,
    CELL_FINISH,
    CELL_VISITED,
    CELL_BACKTRACK,
    CELL_FRONTIER,
    CELL_PATH,
};

enum PlaceingType
{
    PLACE_WALL,
    PLACE_START,
    PLACE_FINISH
};

enum SimState
{
    SIM_STATE_SETUP,
    SIM_STATE_SIMULATEING,
    SIM_STATE_DONE
};

enum AlgoState
{
    ALGO_STATE_WAITING,
    ALGO_STATE_INIT,
    ALGO_STATE_STEP,
    ALGO_STATE_DONE
};

struct Cell
{
    int type = CELL_ROOM;
};

#include "include/Pathfinding.h"

Color grid_colors[10] = { CELL_ROOM_COLOR, CELL_WALL_COLOR, CELL_START_COLOR, CELL_FINISH_COLOR, CELL_VISITED_COLOR, CELL_BACKTRACK_COLOR, CELL_FRONTIER_COLOR, CELL_PATH_COLOR};
std::string placeing_type_strings[3] = {"Walls", "Start", "Finish"};
std::string simulation_state_strings[3] = {"Setup", "Simulateing", "Done"};
std::string algo_state_strings[5] = {"Waiting", "Initialization", "Steping", "Done"};

int grid_size = 0;
int cell_size = 0;
vector2<Cell> grid;
Vector2f grid_position;
Vector2i hovered_cell = Vector2i(-1, -1);
Vector2i start_point = Vector2i(-1, -1);
Vector2i finish_point = Vector2i(-1, -1);
bool start_placed = false;
bool finish_placed = false;

int placeing = PLACE_WALL;
float simulation_step_timer = 0;
float simulation_step_delay = 0.05f;
float path_step_delay = 0.05f;
float algo_timer = 0.0f;

int sim_state = SIM_STATE_SETUP; 
int algo_state = ALGO_STATE_WAITING;

PathfindAlgorithm *algorithm = nullptr;

void InitGrid(int size, vector2<Cell> &grid)
{
    grid_size = size;
    cell_size = MAX_GRID_LENGTH / grid_size;

    grid.resize(grid_size);
    for(auto &row : grid)
        row.resize(grid_size);
}

bool CheckPointOverlapBox(const Vector2f &point, const Vector2f &bmin, const Vector2f &bmax)
{
    if(point.x >= bmin.x && point.x <= bmax.x && point.y >= bmin.y && point.y <= bmax.y)
        return true;

    return false;
}

Vector2i CheckMouseHoverOnCell(const Vector2f &mouse_position)
{
    hovered_cell = Vector2i(-1, -1);
    for(int i = 0; i < grid_size; i++)
    {
        for (int j = 0; j < grid_size; j++)
        {
            int left = grid_position.x + j * cell_size;
            int top = grid_position.y + i * cell_size;
            int right = left + cell_size;
            int bottom = top + cell_size;
            
            if(CheckPointOverlapBox(mouse_position, Vector2f(left, top), Vector2f(right, bottom)))
                hovered_cell = Vector2i(j, i);
        }
    }

    return hovered_cell;
}

void PlaceCell(int place, const Vector2i &hovered_cell, vector2<Cell> &grid)
{
    if(hovered_cell == Vector2i(-1, -1)) return;

    int type;
    
    if(place == PLACE_WALL) type = CELL_WALL;
    else if(place == PLACE_START) type = CELL_START;
    else if(place == PLACE_FINISH) type = CELL_FINISH;

    if((type == CELL_START && start_placed) || 
        (type == CELL_FINISH && finish_placed)) return;

    Cell &cell = grid[hovered_cell.y][hovered_cell.x];

    if(cell.type != CELL_ROOM) return;

    if(type == CELL_START) 
    {
        start_placed = true;
        start_point = hovered_cell;
    }
    else if(type == CELL_FINISH) 
    {
        finish_placed = true;
        finish_point = hovered_cell;
    }

    cell.type = type;
}

void ClearGrid(vector2<Cell> &grid)
{
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            grid[i][j].type = CELL_ROOM;

    start_placed = finish_placed = false;
    start_point = finish_point = Vector2i(-1, -1);
}

void ResetGrid(vector2<Cell> &grid)
{
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
        {
            int &type = grid[i][j].type; 
            if(!(type == CELL_WALL || type == CELL_START || type == CELL_FINISH))
                type = CELL_ROOM;
        }
}

void ClearCell(const Vector2i &hovered_cell, vector2<Cell> &grid)
{
    if(hovered_cell == Vector2i(-1, -1)) return;

    Cell &cell = grid[hovered_cell.y][hovered_cell.x];

    if(cell.type == CELL_START && start_placed) 
    {
        start_point = Vector2i(-1, -1);
        start_placed = false;
    }
    else if(cell.type == CELL_FINISH && finish_placed) 
    {
        finish_point = Vector2i(-1, -1);
        finish_placed = false;
    }

    cell.type = CELL_ROOM;
}

void DrawGrid(const vector2<Cell> &grid)
{
    RectangleShape rectangle(Vector2f(cell_size, cell_size));
    rectangle.setOutlineColor(GRID_OUTLINE_COLOR);
    rectangle.setOutlineThickness(1);

    for(int i = 0; i < grid_size; i++)
    {
        for(int j = 0; j < grid_size; j++)
        {
            Vector2f position = Vector2f(i * cell_size, j * cell_size) + grid_position;
            Cell cell = grid[j][i];

            rectangle.setFillColor(grid_colors[cell.type]);
            rectangle.setPosition(position);
            window.draw(rectangle);
        }
    }
}

void DrawCellCursor(const Vector2i &hovered_cell)
{
    if(hovered_cell == Vector2i(-1, -1)) return;

    RectangleShape rectangle(Vector2f(cell_size - 2, cell_size - 2));
    rectangle.setFillColor(Color::Transparent);
    rectangle.setOutlineColor(HOVER_CURSOR_COLOR);
    rectangle.setOutlineThickness(2);
    rectangle.setPosition(Vector2f(hovered_cell.x * cell_size, hovered_cell.y * cell_size) + grid_position);

    window.draw(rectangle);
}

void IGInfoWindow()
{
    std::string text;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(ImVec2(INFO_W_POS.x, INFO_W_POS.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(INFO_W_SIZE.x, INFO_W_SIZE.y), ImGuiCond_Always);

    ImGui::Begin("Information", nullptr, flags);

    text = "FPS: " + std::to_string(current_FPS);
    ImGui::Text(text.c_str());

    text = "Placeing: " + placeing_type_strings[placeing];
    ImGui::Text(text.c_str());

    text = "Start placed: "; start_placed ? text += "Yes" : text += "No";
    ImGui::Text(text.c_str());
    
    text = "Finish placed: "; finish_placed ? text += "Yes" : text += "No";
    ImGui::Text(text.c_str());

    text = "Simulation state: " + simulation_state_strings[sim_state];
    ImGui::Text(text.c_str());

    text = "Algorithm state: " + algo_state_strings[algo_state];
    ImGui::Text(text.c_str());

    text = "Time taken: " + std::to_string(algo_timer) + "seconds";
    ImGui::Text(text.c_str());

    text = "Using: " + algorithm->name;
    ImGui::Text(text.c_str());

    if(sim_state != SIM_STATE_SETUP) ImGui::BeginDisabled();

    ImGui::NewLine();

    text = "Choose algorithm: ";
    ImGui::Text(text.c_str());
    
    if(ImGui::Button(algorithms_strings[ALG_DFS].c_str()))
        SetAlgorithm(ALG_DFS, algorithm);
    
    ImGui::SameLine();

    if(ImGui::Button(algorithms_strings[ALG_BFS].c_str()))
        SetAlgorithm(ALG_BFS, algorithm);

    if(sim_state != SIM_STATE_SETUP) ImGui::EndDisabled();

    ImGui::Text("Algorithm step delay");
    ImGui::SliderFloat("ASD", &simulation_step_delay, 0.0f, 0.25f);
    
    ImGui::Text("Path step delay");
    ImGui::SliderFloat("PSD", &path_step_delay, 0.0f, 0.25f);

    ImGui::End();
}

bool IG_MENU_custom_grid_size_window = false;
void IGMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if(sim_state == SIM_STATE_SIMULATEING) ImGui::BeginDisabled();

            if (ImGui::BeginMenu("GridSize"))
            {
                if (ImGui::MenuItem("5x5"))   { InitGrid(5, grid); }
                if (ImGui::MenuItem("10x10")) { InitGrid(10, grid); }
                if (ImGui::MenuItem("20x20")) { InitGrid(20, grid); }
                if (ImGui::MenuItem("40x40")) { InitGrid(40, grid); }
                if (ImGui::MenuItem("Custom")){ IG_MENU_custom_grid_size_window = true; }
                ImGui::EndMenu();
            }

            if(sim_state == SIM_STATE_SIMULATEING) ImGui::EndDisabled();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

int IG_CGS_grid_size = 10; 
void IGCustomGridSizeWindow()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(200, 100));
    ImGui::Begin("Custom Grid Size", nullptr, flags);
    ImGui::InputInt("Size", &IG_CGS_grid_size);
    
    if(ImGui::Button("Apply"))
    {
        InitGrid(IG_CGS_grid_size, grid);
        IG_MENU_custom_grid_size_window = false;    
    }
    
    ImGui::SameLine();

    if(ImGui::Button("Close"))
        IG_MENU_custom_grid_size_window = false;
    
    ImGui::End();
}

void SetupState()
{
    hovered_cell = CheckMouseHoverOnCell(canvasMousePosition);

    if(IsKeyboardButtonDown(Keyboard::Key::Num1)) placeing = PLACE_WALL;
    else if(IsKeyboardButtonDown(Keyboard::Key::Num2)) placeing = PLACE_START;
    else if(IsKeyboardButtonDown(Keyboard::Key::Num3)) placeing = PLACE_FINISH;

    if(IsMouseButtonPressed(Mouse::Button::Left))
        PlaceCell(placeing, hovered_cell, grid);

    if(IsMouseButtonPressed(Mouse::Button::Right))
        ClearCell(hovered_cell, grid);

    if(IsKeyboardButtonDown(Keyboard::Key::C))
        ClearGrid(grid);

    if(IsKeyboardButtonDown(Keyboard::Key::Space) && start_placed && finish_placed)
    {
        sim_state = SIM_STATE_SIMULATEING;
    }
}

void SimulationState()
{
    if(algo_state == ALGO_STATE_WAITING)
    {
        algo_state = ALGO_STATE_INIT;
    }
    if(algo_state == ALGO_STATE_INIT)
    {
        algorithm->Init(start_point, finish_point, grid_size);
        algo_state = ALGO_STATE_STEP;
    }
    else if(algo_state == ALGO_STATE_STEP)
    {
        simulation_step_timer += Timer::deltaTime;
        if(simulation_step_timer < simulation_step_delay) return;

        algo_timer += Timer::deltaTime;
        
        algorithm->StepAlgorithm(grid);

        if(algorithm->finished)
        {
            algo_state = ALGO_STATE_DONE;
            grid[start_point.y][start_point.x].type = CELL_START;
            grid[finish_point.y][finish_point.x].type = CELL_FINISH;
        }

        simulation_step_timer = 0;
    }
    else if(algo_state == ALGO_STATE_DONE)
    {
        simulation_step_timer += Timer::deltaTime;
        if(simulation_step_timer < path_step_delay) return;
        
        algorithm->StepPath(grid);

        if(algorithm->done)
            sim_state = SIM_STATE_DONE;

        simulation_step_timer = 0;
    }
}

void ResetToSetup()
{
    ResetGrid(grid);
    SetAlgorithm(ALG_DFS, algorithm);
    sim_state = SIM_STATE_SETUP;
    algo_state = ALGO_STATE_WAITING;
    placeing = PLACE_WALL;
    algo_timer = 0;
}

void Start()
{
    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    grid_position = Vector2f(
        (window_width - INFO_W_SIZE.x) * 0.5 - MAX_GRID_LENGTH * 0.5,
        window_height * 0.5 - MAX_GRID_LENGTH * 0.5
    );

    InitGrid(START_GRID_SIZE, grid);
    SetAlgorithm(ALG_DFS, algorithm);
}

void Update()
{
    if(IsKeyboardButtonDown(Keyboard::Key::R))
        ResetToSetup();

    IGInfoWindow();
    IGMenu();

    if(IG_MENU_custom_grid_size_window) { IGCustomGridSizeWindow(); return; }

    if(sim_state == SIM_STATE_SETUP) SetupState();
    else if(sim_state == SIM_STATE_SIMULATEING) SimulationState();
}

void Draw()
{
    window.setView(camera);
    window.setView(canvas);
    DrawGrid(grid);

    if(sim_state == SIM_STATE_SETUP)
        DrawCellCursor(hovered_cell);
}

int main()
{
    window = RenderWindow(VideoMode({window_width, window_height}), window_title, sf::Style::Close);
    window.setFramerateLimit(FPS);

    if(!font_jetBrains.openFromFile("resources/JetBrainsMono-Regular.ttf"))
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
            {
                window.close();
            }
        }
        
        Timer::Tick();
        if(Timer::deltaTime >= 1.0f / FPS)
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