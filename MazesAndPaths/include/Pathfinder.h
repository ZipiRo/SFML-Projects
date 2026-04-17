const Color DEFAULT_VISTED_COLOR = Color(37, 132, 180); 
const Color DEFAULT_FRONTIER_COLOR = Color(64, 179, 237); 
const Color DEFAULT_BACKTRACK_COLOR = Color(10, 49, 69); 
const Color DEFAULT_PATH_COLOR = Color::Green; 
const Color DEFAULT_PATH_START_COLOR = Color(0, 41, 255);
const Color DEFAULT_PATH_END_COLOR = Color(228, 36, 26);
const float DEFAULT_PATH_STEP_DELAY = 0.05f;

class Pathfinder
{
public:
    struct Algorithm
    {
        bool path_found;
        bool done;

        Color *visited_color;
        Color *frontier_color;
        Color *backtrack_color;

        Algorithm()
        {
            path_found = false;
            done = false;
        }

        virtual void Init(const Grid &grid, const Vector2i &start, const Vector2i &end) = 0;
        virtual void Step(Grid &grid) = 0;
        virtual std::vector<Vector2i> ConstructPath() = 0;
    };

private:
    enum AlgorithmState
    {
        ALGO_STAL,
        ALGO_INIT,
        ALGO_RUN,
        ALGO_DONE
    };

    enum PlaceType
    {
        PLACE_START,
        PLACE_END
    };

    struct AlgorithmEntry
    {
        std::string abbr;
        std::string name;
        std::string description;
        std::function<std::unique_ptr<Algorithm>()> create;

        AlgorithmEntry(std::string abbr, std::string name, std::string description, std::function<std::unique_ptr<Algorithm>()> create)
        {
            this->abbr = abbr;
            this->name = name;
            this->description = description;
            this->create = create;
        }
    };

    int path_index;

    int algorithm_state;
    int using_algorithm; 
    
    int algo_start_delay;
    float algo_step_delay;
    float path_step_delay;
    
    float step_timer;
    float start_timer;

    bool algo_pause;
    bool show_sidebar;
    bool show_settings;

    std::vector<Vector2i> path;

    Point start, end;
    
    Grid *grid = nullptr;
    std::unique_ptr<Algorithm> algorithm;
    std::vector<AlgorithmEntry> algorithm_entry;

    void RegisterAlgorithms();

    void SetDefaultColors()
    {
        visited_color = DEFAULT_VISTED_COLOR;
        frontier_color = DEFAULT_FRONTIER_COLOR;
        backtrack_color = DEFAULT_BACKTRACK_COLOR;
        path_color = DEFAULT_PATH_COLOR;
        start_color = DEFAULT_PATH_START_COLOR;
        end_color = DEFAULT_PATH_END_COLOR;
    }
    
    void Run()
    {
        if(!start.valid || !end.valid) return;

        algorithm = algorithm_entry[using_algorithm].create();
        algorithm->visited_color = &visited_color;
        algorithm->frontier_color = &frontier_color;
        algorithm->backtrack_color = &backtrack_color;
        
        algorithm_state = ALGO_INIT;
    }

    void PlacePoint(const Vector2i &where)
    {
        if(grid->graph[where.y][where.x].type != CELL_ROOM) return;

        if(!start.valid)
        {
            start.position = where;
            start.valid = true;

            grid->graph[start.position.y][start.position.x] = Cell(start_color);
        }
        else if(!end.valid)
        {
            end.position = where;
            end.valid = true;

            grid->graph[end.position.y][end.position.x] = Cell(end_color);
        }
    }

    void RemovePoint(const Vector2i &where)
    {
        bool removed = false;
        if(where == start.position) 
        {
            start.valid = false;
            removed = true;
        }
        else if(where == end.position) 
        {
            end.valid = false;
            removed = true;
        }

        if(removed)
            grid->graph[where.y][where.x] = Cell(CELL_ROOM);
    }

    void StepPath()
    {
        if(path_index >= path.size())
            return;

        float t = float(path_index) / path.size();

        Vector2i point = path[path_index++];
        grid->graph[point.y][point.x].color = &path_color;
    }

    void RandomStartEnd()
    {
        Reset();
        Vector2i random_position = Vector2i(rand() % grid->size.x, rand() % grid->size.y);
        while (!start.valid || !end.valid)
        {
            random_position = Vector2i(rand() % grid->size.x, rand() % grid->size.y);
            PlacePoint(random_position);
        }
    }

    void InitSettingsWindow()
    {
        
    }
    
    void SidebarWindow()
    {
        if(!show_sidebar) return;
        grid->offset.x = -SIDEBAR_WIDTH;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;

        ImGui::SetNextWindowPos(ImVec2(window_width - SIDEBAR_WIDTH, MENU_BAR_HEIGHT), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(SIDEBAR_WIDTH, window_height), ImGuiCond_Always);

        ImGui::Begin("Pathfinder", nullptr, flags);

        ImGui::BeginDisabled(show_settings);

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Algorithms"))
        {
            float offset = 0;
            for(int i = 0; i < algorithm_entry.size(); i++)
            {
                ImVec2 text_size = ImGui::CalcTextSize(algorithm_entry[i].abbr.c_str());
                float button_width = text_size.x + 20.0f;

                if(offset + button_width > SIDEBAR_WIDTH) offset = 0;
                else if(offset > 0) ImGui::SameLine();


                ImGui::PushStyleColor(ImGuiCol_Button, 
                    (using_algorithm == i) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f)
                                        : ImGui::GetStyleColorVec4(ImGuiCol_Button));

                if(ImGui::Button(algorithm_entry[i].abbr.c_str(), ImVec2(button_width, 0)))
                    using_algorithm = i;
                
                ImGui::PopStyleColor();
                
                offset += button_width + ImGui::GetStyle().ItemSpacing.x; 
            }
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Algorithm Settings"))
        {
            ImGui::Text("Start Delay");
            if(ImGui::Button("|##1"))
                algo_start_delay = 0;

            ImGui::SameLine();
            ImGui::SliderInt("##AlgoStartDelay", &algo_start_delay, 0, 5);
            
            ImGui::SameLine();
            if(ImGui::Button("Default##1"))
                algo_start_delay = DEFAULT_ALGO_START_DELAY;
            ImGui::Text("Step Delay");
            if(ImGui::Button("|##2"))
                algo_step_delay = 0.0f;
            
            ImGui::SameLine();
            ImGui::SliderFloat("##AlgoStepDelay", &algo_step_delay, 0.0f, 0.2f);
            
            ImGui::SameLine();
            if(ImGui::Button("Default##2"))
                algo_step_delay = DEFAULT_ALGO_STEP_DELAY;
            ImGui::Text("Path Step Delay");
            if(ImGui::Button("|##3"))
                path_step_delay = 0.0f;
            
            ImGui::SameLine();
            ImGui::SliderFloat("##PathStepDelay", &path_step_delay, 0.0f, 0.2f);
            
            ImGui::SameLine();
            if(ImGui::Button("Default##3"))
                path_step_delay = DEFAULT_PATH_STEP_DELAY;
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Actions"))
        {
            bool running_algorithm = 
            ( 
                algorithm_state == ALGO_INIT || 
                algorithm_state == ALGO_RUN || 
                algorithm_state == ALGO_DONE
            );

            ImGui::BeginDisabled(running_algorithm);
            ImGui::PushStyleColor(ImGuiCol_Button, 
                    (running_algorithm) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
            if(ImGui::Button("Start (Space)")) 
                Run();
            ImGui::PopStyleColor();
            ImGui::EndDisabled();

            ImGui::SameLine();
            if(ImGui::Button("Reset (R)")) 
                Reset();

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (algo_pause) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImVec4(0.7f, 0.2f, 0.0f, 1.0f));
            if(ImGui::Button(algo_pause ? "Play (P)" : "Pause (P)"))
                algo_pause = !algo_pause;
            ImGui::PopStyleColor();

            ImGui::BeginDisabled(running_algorithm);
            if(ImGui::Button("Random Start & End"))
                RandomStartEnd();
            ImGui::EndDisabled();
            
            if(ImGui::Button("Settings (LShift)"))
            {
                show_settings = true;
                InitSettingsWindow();
            }
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Algorithm Info"))
        {
            ImGui::Text(std::string("Name:\n" + algorithm_entry[using_algorithm].name).c_str());
            ImGui::TextWrapped(std::string("Description:\n" + algorithm_entry[using_algorithm].description).c_str());
        }

        ImGui::EndDisabled();
        ImGui::End();
    }

    void SettingsWindow()
    {
        if(!show_settings) return;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;
                                
        ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Always);
        ImGui::Begin("Pathfinder Settings", &show_settings, flags);

        if(ImGui::IsKeyDown(ImGuiKey_Escape))
            show_settings = false;
     
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Colors"))
        {
            ImVec4 imgui_color;

            imgui_color = SFMLToImColor(visited_color);
            if(ImGui::ColorEdit4("Visited Color", (float*)&imgui_color))
                visited_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(backtrack_color);
            if(ImGui::ColorEdit4("Backtrack Color", (float*)&imgui_color))
                backtrack_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(frontier_color);
            if(ImGui::ColorEdit4("Frontier Color", (float*)&imgui_color))
                frontier_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(path_color);
            if(ImGui::ColorEdit4("Path Color", (float*)&imgui_color))
                path_color = ImColorToSFML(imgui_color);

            imgui_color = SFMLToImColor(start_color);
            if(ImGui::ColorEdit4("Start Color", (float*)&imgui_color))
                start_color = ImColorToSFML(imgui_color);

            imgui_color = SFMLToImColor(end_color);
            if(ImGui::ColorEdit4("End Color", (float*)&imgui_color))
                end_color = ImColorToSFML(imgui_color);

            if(ImGui::Button("Set Defaults"))
                SetDefaultColors();
        }

        ImGui::End();
    }

public: 
    Color visited_color;
    Color frontier_color;
    Color backtrack_color;
    Color path_color;
    Color start_color;
    Color end_color;

    Pathfinder() {}

    Pathfinder(Grid *grid)
    {
        this->grid = grid;
        algorithm_state = ALGO_STAL;

        algo_step_delay = DEFAULT_ALGO_STEP_DELAY;
        path_step_delay = DEFAULT_PATH_STEP_DELAY;
        algo_start_delay = DEFAULT_ALGO_START_DELAY;
        
        algo_pause = false;

        using_algorithm = 0;

        show_sidebar = true;
        show_settings = false;

        SetDefaultColors();
        RegisterAlgorithms(); 
    }

    void UserInterface()
    {
        SidebarWindow();
        SettingsWindow();
    }

    void Start()
    {
        show_sidebar = true;
        show_settings = false;
    }

    void Update()
    {
        if(IsKeyboardButtonDown(KEY_SHOW_STTINGS))
        {
            show_settings = !show_settings;
            if(show_settings)
                InitSettingsWindow();
        }
            
        if(show_settings) return;
        
        if(IsKeyboardButtonDown(KEY_HIDE_SIDEBAR))
            show_sidebar = !show_sidebar;
        
        if(IsKeyboardButtonDown(KEY_PAUSE))
            algo_pause = !algo_pause;

        if(IsKeyboardButtonDown(KEY_RESET))
            Reset();

        switch (algorithm_state)
        {
        case ALGO_STAL:
            grid->CheckGridCursor(canvas_mouse_position);

            if(IsMouseButtonDown(BUTTON_PLACE) && grid->cursor.valid) PlacePoint(grid->cursor.position);
            else if(IsMouseButtonDown(BUTTON_REMOVE) && grid->cursor.valid) RemovePoint(grid->cursor.position);

            if(IsKeyboardButtonDown(KEY_START_ALGO)) Run();

            start_timer = 0.0f;
            break;
        case ALGO_INIT:
            if(algo_pause) return;
            start_timer += Timer::deltaTime;
            if(start_timer < algo_start_delay) return;

            step_timer = algo_step_delay;
            path_index = 0;

            algorithm->Init(*grid, start.position, end.position);

            algorithm_state = ALGO_RUN;
            break;
        case ALGO_RUN:
            if(algo_pause) return;
            step_timer += Timer::deltaTime;
            if(step_timer < algo_step_delay) return;

            algorithm->Step(*grid);

            if(algorithm->path_found)
            {
                grid->Reset();
                grid->graph[start.position.y][start.position.x].color = &start_color;
                grid->graph[end.position.y][end.position.x].color = &end_color;
                path = algorithm->ConstructPath();
                algorithm_state = ALGO_DONE;
            }

            step_timer = 0.0f;
            break;
        case ALGO_DONE:
            if(algo_pause) return;
            step_timer += Timer::deltaTime;
            if(step_timer < path_step_delay) return;

            StepPath();

            if(algorithm->done)
            {
                algorithm_state = ALGO_STAL;
                start.valid = false;
                end.valid = false;
            }

            step_timer = 0.0f;
            break;
        default:
            break;
        }
    }

    void Reset()
    {
        algo_pause = false;
        start.valid = false;
        end.valid = false;
        path_index = 0;
        grid->Reset();
        algorithm_state = ALGO_STAL;
    }
};

#include "Pathfinder/Depth-FirstSearch.h"
#include "Pathfinder/Breadth-FirstSearch.h"

void Pathfinder::RegisterAlgorithms()
{
    algorithm_entry.emplace_back(
        DFS::abbr,
        DFS::name,
        DFS::description,
        []() { return std::make_unique<DFS>(); }
    );

    algorithm_entry.emplace_back(
        BFS::abbr,
        BFS::name,
        BFS::description,
        []() { return std::make_unique<BFS>(); }
    );
}