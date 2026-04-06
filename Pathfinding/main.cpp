#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>

#include "include/Timer.h"

template<typename T>
using vector2 = std::vector<std::vector<T>>;

using namespace sf;

RenderWindow window;
const int window_width = 1000;
const int window_height = 700;
const char *window_title = "Pathfinding Vizualisation";
const float PI = 3.1415926354f;
const int MAX_FPS = 999999;

int FPS = 60 + 1;
Color BackgroundColor = Color::Black;
Font font_jetBrains;

View camera;
View canvas;

Vector2f canvasMousePosition;
Vector2f worldMousePosition;

float fps_timer = 0;
int current_FPS;
bool program_open = true;

#include "include/Input.h"

const Vector2i INFO_W_SIZE = Vector2i(250, window_height);
const Vector2i INFO_W_POS = Vector2i(window_width - INFO_W_SIZE.x, 0);

const int START_GRID_SIZE = 20;
const int MAX_GRID_LENGTH = window_width - INFO_W_SIZE.x - 150;

const float MAX_SIM_STEP_DELAY = 0.25f;
const float MAX_PATH_STEP_DELAY = 0.2f;

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

SoundBuffer pop_sound;
SoundBuffer done_sound;
SoundBuffer place_sound;
SoundBuffer remove_sound;
 
std::map<std::string, Texture> key_textures;

Sound sound_player{pop_sound};

Color grid_colors[10] = { CELL_ROOM_COLOR, CELL_WALL_COLOR, CELL_START_COLOR, CELL_FINISH_COLOR, CELL_VISITED_COLOR, CELL_BACKTRACK_COLOR, CELL_FRONTIER_COLOR, CELL_PATH_COLOR};
std::string placeing_type_strings[3] = {"Walls", "Start", "Finish"};
std::string simulation_state_strings[3] = {"Setup", "Simulateing", "Done"};
std::string algo_state_strings[5] = {"Waiting", "Initialization", "Steping", "Done"};

int grid_size;
float cell_size;
float grid_length;
vector2<Cell> grid;
Vector2f grid_position;
Vector2i hovered_cell;
Vector2i start_point;
Vector2i finish_point;
bool start_placed;
bool finish_placed;

int placeing;
float step_timer = 0;

float simulation_step_delay = 0.05f;
float path_step_delay = 0.05f;

int sim_state; 
int algo_state;

PathfindAlgorithm *algorithm = nullptr;
int using_algorithm = algo["DFS"]; 
float algo_timer = 0;
bool algorithm_paused = false;

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

    sound_player.setBuffer(place_sound);
    sound_player.setPitch(1);
    sound_player.play();
}

void InitGrid(int size, vector2<Cell> &grid)
{
    grid_size = size;
    cell_size = MAX_GRID_LENGTH / float(grid_size);
    grid_length = grid_size * cell_size;

    grid_position = Vector2f(
        (window_width - INFO_W_SIZE.x) * 0.5 - grid_length * 0.5,
        (window_height + 20) * 0.5 - grid_length * 0.5
    );

    grid.resize(grid_size);
    for(auto &row : grid)
        row.resize(grid_size);

    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            grid[i][j].type = CELL_ROOM;

    start_placed = finish_placed = false;
    start_point = finish_point = Vector2i(-1, -1);
    sim_state = SIM_STATE_SETUP;
    algo_state = ALGO_STATE_WAITING;
    placeing = PLACE_WALL;
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

    if(cell.type == CELL_ROOM) return;

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

    sound_player.setBuffer(remove_sound);
    sound_player.setPitch(1);
    sound_player.play();
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

    ImGui::Begin("Information", &program_open, flags);

    text = "FPS: " + std::to_string(current_FPS);
    ImGui::Text(text.c_str());

    text = "Grid size: " + std::to_string(grid_size) + 'x' + std::to_string(grid_size);
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
    
    text = "Algorithm Paused: "; algorithm_paused ? text += "Yes" : text += "No";
    ImGui::Text(text.c_str());

    text = "Time taken: " + std::to_string(algo_timer) + "seconds";
    ImGui::Text(text.c_str());

    ImGui::NewLine();

    ImGui::Text("Algorithm step delay");
    ImGui::SliderFloat("ASD", &simulation_step_delay, 0.0f, MAX_SIM_STEP_DELAY);
    
    ImGui::Text("Path step delay");
    ImGui::SliderFloat("PSD", &path_step_delay, 0.0f, MAX_PATH_STEP_DELAY);

    ImGui::NewLine();

    ImGui::BeginDisabled(sim_state != SIM_STATE_SETUP);

    ImGui::Text("Choose algorithm:");

    float max_x = ImGui::GetContentRegionAvail().x;
    float x = 0;

    for (int i = 0; i < algorithms.size(); i++)
    {
        ImVec2 size = ImGui::CalcTextSize(algorithms[i].name.c_str());
        float btn_width = size.x + 20.0f;

        if (x + btn_width > max_x) x = 0;
        else if (x > 0) ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, 
            (i == using_algorithm) ? ImVec4(0.2f,0.7f,0.2f,1.0f)
                                    : ImGui::GetStyleColorVec4(ImGuiCol_Button));
        
        if (ImGui::Button(algorithms[i].name.c_str(), ImVec2(btn_width, 0)))
        {
            delete algorithm;
            algorithm = algorithms[i].create();
            using_algorithm = i;
        }

        ImGui::PopStyleColor();

        x += btn_width + ImGui::GetStyle().ItemSpacing.x;
    }

    text = "Name: " + algorithm->name;
    ImGui::Text(text.c_str());

    text = "Description: " + algorithm->description;
    ImGui::TextWrapped(text.c_str());

    ImGui::EndDisabled();

    ImGui::End();
}

bool IG_MENU_custom_grid_size_window = false;
bool IG_MENU_keybinds_window = false;
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
            
            if (ImGui::BeginMenu("MaxFPS"))
            {
                if (ImGui::MenuItem("60"))  { FPS = 60 + 1; window.setFramerateLimit(FPS); }
                if (ImGui::MenuItem("120")) { FPS = 120 + 1; window.setFramerateLimit(FPS); }
                if (ImGui::MenuItem("240")) { FPS = 240 + 1; window.setFramerateLimit(FPS); }
                if (ImGui::MenuItem("MAX")) { FPS = MAX_FPS + 1; window.setFramerateLimit(FPS); }
                ImGui::EndMenu();
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::Button("Keybinds"))
            IG_MENU_keybinds_window = true;

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

void IGKeybindsWindow()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(500, 500));
    ImGui::Begin("Keybinds", &IG_MENU_keybinds_window, flags);
    
    ImGui::Image(key_textures["mouse_left_key"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place cells");

    ImGui::Image(key_textures["mouse_right_key"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo clear cells");

    ImGui::Image(key_textures["key_1"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place walls");

    ImGui::Image(key_textures["key_2"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place start point");
    
    ImGui::Image(key_textures["key_3"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place finish point");

    ImGui::Image(key_textures["key_C"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo clear the grid");

    ImGui::Image(key_textures["key_R"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo reset to setup without clearing the grid");

    ImGui::Image(key_textures["key_P"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo toggle pause simulation");

    ImGui::Image(key_textures["key_Space"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo start the simulation");
    
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
    else if(algo_state == ALGO_STATE_INIT)
    {
        algorithm->Init(start_point, finish_point, grid_size);
        algo_state = ALGO_STATE_STEP;
    }
    else if(algo_state == ALGO_STATE_STEP)
    {
        step_timer += Timer::deltaTime;
        if(step_timer < simulation_step_delay) return;

        algo_timer += Timer::deltaTime;
        
        algorithm->StepAlgorithm(grid);

        sound_player.setBuffer(pop_sound);
        sound_player.setPitch(1.5f - (simulation_step_delay / MAX_SIM_STEP_DELAY));
        sound_player.play();

        if(algorithm->finished)
        {
            algo_state = ALGO_STATE_DONE;
            grid[start_point.y][start_point.x].type = CELL_START;
            grid[finish_point.y][finish_point.x].type = CELL_FINISH;
         
            sound_player.setBuffer(done_sound);
            sound_player.setPitch(1);
            sound_player.play();   
        }

        step_timer = 0;
    }
    else if(algo_state == ALGO_STATE_DONE)
    {
        step_timer += Timer::deltaTime;
        if(step_timer < path_step_delay) return;
        
        algorithm->StepPath(grid);

        if(algorithm->done)
            sim_state = SIM_STATE_DONE;

        step_timer = 0;
    }
}

void ResetToSetup()
{
    ResetGrid(grid);
    delete algorithm;
    algorithm = algorithms[using_algorithm].create();
    sim_state = SIM_STATE_SETUP;
    algo_state = ALGO_STATE_WAITING;
    placeing = PLACE_WALL;
    
    if(start_point != Vector2i(-1, -1))
        grid[start_point.y][start_point.x].type = CELL_START;
    
    if(finish_point != Vector2i(-1, -1))
        grid[finish_point.y][finish_point.x].type = CELL_FINISH;
        
    algo_timer = 0;
}

void Start()
{
    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    RegisterAlgorithms();

    InitGrid(START_GRID_SIZE, grid);

    delete algorithm;
    algorithm = algorithms[using_algorithm].create();
}

void Update()
{
    if(IsKeyboardButtonDown(Keyboard::Key::R))
        ResetToSetup();
    
    if(IsKeyboardButtonDown(Keyboard::Key::P))
        algorithm_paused = !algorithm_paused;

    IGInfoWindow();
    IGMenu();

    if(IG_MENU_custom_grid_size_window) { IGCustomGridSizeWindow(); return; }
    if(IG_MENU_keybinds_window) IGKeybindsWindow(); 

    if(sim_state == SIM_STATE_SETUP) SetupState();
    else if(sim_state == SIM_STATE_SIMULATEING && !algorithm_paused) SimulationState();
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
    srand(time(0));
    window = RenderWindow(VideoMode({window_width, window_height}), window_title, sf::Style::None);
    window.setFramerateLimit(FPS);

    if(!font_jetBrains.openFromFile("resources/JetBrainsMono-Regular.ttf"))
        return 1;

    if(!pop_sound.loadFromFile("resources/pop.wav"))
        return 1;

    if(!done_sound.loadFromFile("resources/done.wav"))
        return 1;

    if(!place_sound.loadFromFile("resources/place.wav"))
        return 1;

    if(!remove_sound.loadFromFile("resources/remove.wav"))
        return 1;

    if(!key_textures["mouse_left_key"].loadFromFile("resources/Mouse_Left_Key_Dark.png"))
        return 1;
    
    if(!key_textures["mouse_right_key"].loadFromFile("resources/Mouse_Right_Key_Dark.png"))
        return 1;

    if(!key_textures["key_C"].loadFromFile("resources/C_Key_Dark.png"))
        return 1;

    if(!key_textures["key_R"].loadFromFile("resources/R_Key_Dark.png"))
        return 1;

    if(!key_textures["key_P"].loadFromFile("resources/P_Key_Dark.png"))
        return 1;

    if(!key_textures["key_Space"].loadFromFile("resources/Space_Key_Dark.png"))
        return 1;
        
    if(!key_textures["key_1"].loadFromFile("resources/1_Key_Dark.png"))
        return 1;

    if(!key_textures["key_2"].loadFromFile("resources/2_Key_Dark.png"))
        return 1;
        
    if(!key_textures["key_3"].loadFromFile("resources/3_Key_Dark.png"))
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
        
        if(!program_open) window.close();

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