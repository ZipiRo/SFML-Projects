#include "Input.h"

template <typename T>
std::vector<T> Shuffle(std::vector<T> v) 
{
    for (int i = v.size() - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        std::swap(v[i], v[j]);
    }

    return v;
}

Color LerpColor(const Color& colora, const Color& colorb, float t)
{
    return Color(
        colora.r + (colorb.r - colora.r) * t,
        colora.g + (colorb.g - colora.g) * t,
        colora.b + (colorb.b - colora.b) * t,
        colora.a + (colorb.a - colora.a) * t
    );
}

Image gridimage;

const Vector2i INFO_W_SIZE = Vector2i(250, window_height);
const Vector2i INFO_W_POS = Vector2i(window_width - INFO_W_SIZE.x, 18);
const Vector2i MENU_BAR_SIZE = Vector2i(window_width, 18);

const int START_GRID_SIZE = 20;

const float MAX_SIM_STEP_DELAY = 0.25f;
const float MAX_PATH_STEP_DELAY = 0.2f;

const Color CELL_ROOM_COLOR = Color(40, 40, 40);
const Color CELL_TRACE_COLOR = Color(30, 30, 30);
const Color CELL_WALL_COLOR = Color(20, 20, 20);
const Color CELL_START_COLOR = Color(38, 135, 200);
const Color CELL_FINISH_COLOR = Color(255, 0, 157);

const Color HOVER_CURSOR_COLOR = Color(10, 10, 10);
const Color GRID_OUTLINE_COLOR = CELL_WALL_COLOR;

enum CellType
{
    CELL_ROOM,
    CELL_WALL,
    CELL_START,
    CELL_FINISH,
    CELL_TRACE,
    CELL_NONE
};

enum PlaceingType
{
    PLACE_WALL,
    PLACE_START,
    PLACE_FINISH
};

enum SimState    
{
    SIM_STATE_WAITING,
    SIM_STATE_SETUP,
    SIM_SATE_CREATE_MAZE,
    SIM_STATE_SIMULATING,
    SIM_STATE_DONE
};

enum AlgoState
{
    ALGO_STATE_WAITING,
    ALGO_STATE_INIT,
    ALGO_STATE_STEP,
    ALGO_STATE_DONE
};

enum MazeState
{
    MAZE_STATE_INIT,
    MAZE_STATE_CREATE,
    MAZE_STATE_DONE
};

struct Cell
{
    int type;
    Color color;
};

#include "Pathfinding.h"
 
std::map<std::string, Texture> key_textures;
std::map<std::string, SoundBuffer> soundbuffers;

Sound sound_player(soundbuffers["nan"]);

Color cell_colors[6] = { CELL_ROOM_COLOR, CELL_WALL_COLOR, CELL_START_COLOR, CELL_FINISH_COLOR, CELL_TRACE_COLOR};
std::string place_type_string[3] = {"Walls", "Start", "Finish"};
std::string sim_state_string[3] = {"Setup", "Simulating", "Done"};
std::string algo_state_strings[4] = {"Waiting", "Initialization", "Steping", "Done"};

int grid_size;
float cell_size;
float grid_length;
int max_grid_length;
vector2<Cell> grid;
Vector2f grid_position;
Vector2f grid_offset;
Vector2i hovered_cell;
Vector2i start_point;
Vector2i finish_point;
bool start_placed;
bool finish_placed;

int placeing;
float step_timer = 0;

float simulation_step_delay = 0.05f;
float path_step_delay = 0.05f;
float maze_step_delay = 0.05f;

int sim_state; 
int algo_state;
int maze_state;

PathfindAlgorithm *algorithm = nullptr;
int using_algorithm = algo[first_algorithm]; 
float algo_timer = 0;
bool algorithm_paused = false;

bool show_gui = true;

bool CheckPointOverlapBox(const Vector2f &point, const Vector2f &bmin, const Vector2f &bmax)
{
    if(point.x >= bmin.x && point.x <= bmax.x && point.y >= bmin.y && point.y <= bmax.y)
        return true;

    return false;
}
#include "Grid.h"

void CreateGridFromImage()
{
    gridimage.loadFromFile("resources/Gridimage.png");
    for(int i = 0; i < gridimage.getSize().y; i++)
    {
        for(int j = 0; j < gridimage.getSize().x; j++)
        {
            Color pixel = gridimage.getPixel({j, i});

            if(pixel.r > 128) grid[i][j].type = CELL_ROOM;
                else grid[i][j].type = CELL_WALL;
        }
    }
}

void InverseCreateGridFromImage()
{
    gridimage.loadFromFile("resources/gridimage.png");

    for(int i = 0; i < gridimage.getSize().y; i++)
    {
        for(int j = 0; j < gridimage.getSize().x; j++)
        {
            Color pixel = gridimage.getPixel({j, i});

            if(pixel.r > 128) grid[i][j].type = CELL_WALL;
        }
    }
}

void ResetToSetup()
{
    ResetGrid(grid);
    delete algorithm;
    algorithm = algorithms[using_algorithm].create();
    sim_state = SIM_STATE_SETUP;
    algo_state = ALGO_STATE_WAITING;
    maze_state = MAZE_STATE_INIT;
    placeing = PLACE_WALL;
    
    if(start_point != Vector2i(-1, -1))
        grid[start_point.y][start_point.x].type = CELL_START;
    
    if(finish_point != Vector2i(-1, -1))
        grid[finish_point.y][finish_point.x].type = CELL_FINISH;
        
    algo_timer = 0;
}

#include "Gui.h"

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
        sim_state = SIM_STATE_SIMULATING;
    }
}

vector2<bool> maze_visited;
std::stack<Vector2i> maze_stack;
Vector2i maze_start_point;
bool maze_finished;
void GenerateMazeStep(vector2<Cell> &grid)
{
    if(maze_stack.empty())
    {
        maze_finished = true;
        return;
    }

    Vector2i current = maze_stack.top();

    std::vector<Vector2i> neighbors;
    for(const auto &direction : directions)
    {
        Vector2i neighbor = current + direction * 2;
        
        if(neighbor.x < 0 || neighbor.y < 0 || neighbor.x >= grid_size || neighbor.y >= grid_size) continue;
        if(grid[neighbor.y][neighbor.x].type != CELL_WALL) continue;

        if(!maze_visited[neighbor.y][neighbor.x])
            neighbors.push_back(neighbor);
    }

    if(!neighbors.empty()) 
    {
        Vector2i next = neighbors[rand() % neighbors.size()];
        Vector2i direction = (next - current) / 2;

        Vector2i wall = current + direction;

        grid[next.y][next.x].type = CELL_NONE; 
        grid[next.y][next.x].color = Color(172, 53, 53);

        grid[wall.y][wall.x].type = CELL_NONE;
        grid[wall.y][wall.x].color = Color(172, 53, 53);
        
        sound_player.setBuffer(soundbuffers["remove"]);
        sound_player.setPitch(1);
        sound_player.play();

        maze_visited[next.y][next.x] = true;
        maze_stack.push(next);
    }
    else 
    {
        maze_stack.pop();
        grid[current.y][current.x].type = CELL_NONE;
        grid[current.y][current.x].color = Color(105, 55, 55);
        
        sound_player.setBuffer(soundbuffers["pop"]);
        sound_player.setPitch(1);
        sound_player.play();
    }
}

void CreateMazeState()
{
    if(maze_state == MAZE_STATE_INIT)
    {    
        maze_stack = std::stack<Vector2i>();
        maze_visited = vector2<bool>();
        maze_finished = false;

        maze_visited.resize(grid_size);
        for(int i = 0; i < grid_size; i++)
        {
            maze_visited[i].resize(grid_size);
            for(int j = 0; j < grid_size; j++)
                grid[i][j].type = CELL_WALL;
        }

        maze_start_point = Vector2i(rand() % grid_size, rand() % grid_size);

        maze_stack.push(maze_start_point);
        maze_visited[maze_start_point.y][maze_start_point.x] = true;
        grid[maze_start_point.y][maze_start_point.x].type = CELL_ROOM;
        maze_finished = false;

        maze_state = MAZE_STATE_CREATE;
    }
    else if(maze_state == MAZE_STATE_CREATE)
    {   
        step_timer += Timer::deltaTime;
        if(step_timer < maze_step_delay) return;

        GenerateMazeStep(grid);

        if(maze_finished)
        {
            maze_state = MAZE_STATE_DONE;
            
            sound_player.setBuffer(soundbuffers["done"]);
            sound_player.setPitch(1);
            sound_player.play();
        }

        step_timer = 0;
    }
    else if(maze_state == MAZE_STATE_DONE)
    {
        sim_state = SIM_STATE_SETUP;
        maze_state = MAZE_STATE_INIT;
        ResetToSetup();
    }
}

void SimulationState()
{
    if(algorithm_paused) return;

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

        sound_player.setBuffer(soundbuffers["pop"]);
        sound_player.setPitch(1.5f - (simulation_step_delay / MAX_SIM_STEP_DELAY));
        sound_player.play();

        if(algorithm->finished)
        {
            ResetGrid(grid);
            algorithm->Trace(grid);
            grid[start_point.y][start_point.x].type = CELL_START;
            grid[finish_point.y][finish_point.x].type = CELL_FINISH;

            algo_state = ALGO_STATE_DONE;
        }

        step_timer = 0;
    }
    else if(algo_state == ALGO_STATE_DONE)
    {
        step_timer += Timer::deltaTime;
        if(step_timer < path_step_delay) return;
        
        algorithm->StepPath(grid);

        float t = (float)algorithm->path_index / (float)algorithm->path.size();

        float pitch = 2.0f - t * 1.5f;
        pitch = std::clamp(pitch, 0.1f, 3.0f);

        sound_player.setBuffer(soundbuffers["find"]);
        sound_player.setPitch(pitch);
        sound_player.play();   

        if(algorithm->done)
        {
            sound_player.setBuffer(soundbuffers["done"]);
            sound_player.setPitch(1);
            sound_player.play();   
            sim_state = SIM_STATE_DONE;
        }

        step_timer = 0;
    }
}

void Start()
{
    canvas = window.getView();
    camera = View(Vector2f(0, 0), Vector2f(window_width, window_height));

    RegisterAlgorithms();

    InitGrid(START_GRID_SIZE, grid);

    delete algorithm;
    algorithm = algorithms[using_algorithm].create();

    grid_offset = Vector2f(0, MENU_BAR_SIZE.y);
}

void Update()
{
    if(IsKeyboardButtonDown(Keyboard::Key::Escape))
        window.close();

    grid_offset = Vector2f(0, 0);
    max_grid_length = window_width - 350;

    if(IG_MENU_save_grid_window) { IGSaveGridWindow(); return; }
    if(IG_MENU_load_grid_window) { IGLoadGridWindow();  return; }

    if(IsKeyboardButtonDown(Keyboard::Key::F1))
        IG_MENU_save_grid_window = true;

    if(IsKeyboardButtonDown(Keyboard::Key::F2))
        IG_MENU_load_grid_window = true;

    if(IsKeyboardButtonDown(Keyboard::Key::R))
        ResetToSetup();
    
    if(IsKeyboardButtonDown(Keyboard::Key::P))
        algorithm_paused = !algorithm_paused;

    if(IsKeyboardButtonDown(Keyboard::Key::H))
        show_gui = !show_gui;

    if(IsKeyboardButtonDown(Keyboard::Key::I))
        IG_MENU_paths_window = !IG_MENU_paths_window;

    if(IsKeyboardButtonDown(Keyboard::Key::K))
        IG_MENU_keybinds_window = !IG_MENU_keybinds_window;

    if(IsKeyboardButtonDown(Keyboard::Key::M))
        sim_state = SIM_SATE_CREATE_MAZE;

    if(IsKeyboardButtonDown(Keyboard::Key::L))
        CreateGridFromImage();

    if(IsKeyboardButtonDown(Keyboard::Key::O))
        InverseCreateGridFromImage();

    if(show_gui)
    {
        IGMenu();

        if(IG_MENU_custom_grid_size_window) { IGCustomGridSizeWindow(); return; }
        if(IG_MENU_keybinds_window) IGKeybindsWindow(); 
        if(IG_MENU_paths_window) IGPathsWindow();
    }

    if(sim_state == SIM_STATE_SETUP) SetupState();
    else if(sim_state == SIM_SATE_CREATE_MAZE) CreateMazeState();
    else if(sim_state == SIM_STATE_SIMULATING) SimulationState();
}

void Draw()
{
    window.setView(camera);
    window.setView(canvas);
    DrawGrid(grid);

    if(sim_state == SIM_STATE_SETUP)
        DrawCellCursor(hovered_cell);
}

bool LoadResources()
{
    if(!soundbuffers["pop"].loadFromFile("resources/pop.wav")) return false;
    if(!soundbuffers["done"].loadFromFile("resources/done.wav")) return false;
    if(!soundbuffers["place"].loadFromFile("resources/place.wav")) return false;
    if(!soundbuffers["remove"].loadFromFile("resources/remove.wav")) return false;
    if(!soundbuffers["find"].loadFromFile("resources/find.wav")) return false;

    if(!key_textures["mouse_left"].loadFromFile("resources/Mouse_Left_Key_Dark.png")) return false;
    if(!key_textures["mouse_right"].loadFromFile("resources/Mouse_Right_Key_Dark.png")) return false;
    if(!key_textures["C"].loadFromFile("resources/C_Key_Dark.png")) return false;
    if(!key_textures["R"].loadFromFile("resources/R_Key_Dark.png")) return false;
    if(!key_textures["P"].loadFromFile("resources/P_Key_Dark.png")) return false;
    if(!key_textures["H"].loadFromFile("resources/H_Key_Dark.png")) return false;
    if(!key_textures["I"].loadFromFile("resources/I_Key_Dark.png")) return false;
    if(!key_textures["K"].loadFromFile("resources/K_Key_Dark.png")) return false;
    if(!key_textures["Space"].loadFromFile("resources/Space_Key_Dark.png")) return false;
    if(!key_textures["1"].loadFromFile("resources/1_Key_Dark.png")) return false;
    if(!key_textures["2"].loadFromFile("resources/2_Key_Dark.png")) return false;
    if(!key_textures["3"].loadFromFile("resources/3_Key_Dark.png")) return false;
    if(!key_textures["F1"].loadFromFile("resources/F1_Key_Dark.png")) return false;
    if(!key_textures["F2"].loadFromFile("resources/F2_Key_Dark.png")) return false;
    if(!key_textures["Esc"].loadFromFile("resources/Esc_Key_Dark.png")) return false;

    return true;
}