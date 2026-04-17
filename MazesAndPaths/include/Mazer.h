const Color DEFAULT_MAZE_START_COLOR = Color(221, 86, 23);
const Color DEFAULT_PRIMARY_COLOR = Color(190, 28, 28);
const Color DEFAULT_SECONDARY_COLOR = Color(106, 19, 19);

class Mazer
{
public:
    struct Algorithm
    {
        bool done;

        Color *primary_color;
        Color *secondary_color;
        Vector2i start;

        Algorithm()
        {
            done = false;
        }

        virtual void Init(const Grid& grid, Vector2i &start) = 0;
        virtual void Step(Grid& grid) = 0;
    };

private:
    enum AlgorithmState
    {
        ALGO_STAL,
        ALGO_INIT,
        ALGO_RUN,
        ALGO_DONE
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

    int using_algorithm;
    int algorithm_state;

    float algo_step_delay;
    int algo_start_delay;
    
    float step_timer;
    float start_timer;
    
    bool algo_pause;
    bool show_sidebar;
    bool show_settings;

    Point start;

    Grid *grid;
    std::unique_ptr<Algorithm> algorithm;
    std::vector<AlgorithmEntry> algorithm_entry;

    void RegisterAlgorithms();

    void SetDefaultColors()
    {
        primary_color = DEFAULT_PRIMARY_COLOR;
        secondary_color = DEFAULT_SECONDARY_COLOR;
        start_color = DEFAULT_MAZE_START_COLOR;
    }

    void Run()
    {
        if(!start.valid) return;

        algorithm = algorithm_entry[using_algorithm].create();
        algorithm->primary_color = &primary_color;
        algorithm->secondary_color = &secondary_color;

        algorithm_state = ALGO_INIT;
    }

    void PlaceStart(const Vector2i &where)
    {
        if(!start.valid)
        {
            start.position = where;
            start.valid = true;

            grid->graph[start.position.y][start.position.x] = Cell(start_color);
        }
    }

    void RemoveStart(const Vector2i &where)
    {
        if(where == start.position) 
        {
            start.valid = false;
            grid->graph[where.y][where.x] = Cell(CELL_ROOM);
        }
    }

    void RandomStart()
    {
        Reset();
        Vector2i random_position = Vector2i(rand() % grid->size.x, rand() % grid->size.y);
        while (!start.valid)
        {
            random_position = Vector2i(rand() % grid->size.x, rand() % grid->size.y);
            PlaceStart(random_position);
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

        ImGui::Begin("Mazer", nullptr, flags);

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
                algo_step_delay = 0;
            
            ImGui::SameLine();
            ImGui::SliderFloat("##AlgoStepDelay", &algo_step_delay, 0.0f, 0.2f);
            
            ImGui::SameLine();
            if(ImGui::Button("Default##2"))
                algo_step_delay = DEFAULT_ALGO_STEP_DELAY;
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

            if(ImGui::Button("Fill (F)")) 
            {
                grid->Fill(PLACE_WALL);
                start.valid = false;
            }
            
            ImGui::SameLine();
            if(ImGui::Button("Clear (C)")) 
            {
                grid->Fill(PLACE_ROOM);
                start.valid = false;
            }

            ImGui::BeginDisabled(running_algorithm);
            if(ImGui::Button("Random Start"))
                RandomStart();
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
        ImGui::Begin("Mazer Settings", &show_settings, flags);

        if(ImGui::IsKeyDown(ImGuiKey_Escape))
            show_settings = false;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Colors"))
        {
            ImVec4 imgui_color;

            imgui_color = SFMLToImColor(start_color);
            if(ImGui::ColorEdit4("Start Color", (float*)&imgui_color))
                start_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(primary_color);
            if(ImGui::ColorEdit4("Primary Color", (float*)&imgui_color))
                primary_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(secondary_color);
            if(ImGui::ColorEdit4("Secondary Color", (float*)&imgui_color))
                secondary_color = ImColorToSFML(imgui_color);

            if(ImGui::Button("Set Defaults"))
                SetDefaultColors();
        }

        ImGui::End();
    }

public:
    Color primary_color;
    Color secondary_color;
    Color start_color;

    Mazer() {}

    Mazer(Grid *grid)
    {
        this->grid = grid;
        algorithm_state = ALGO_STAL;

        algo_step_delay = DEFAULT_ALGO_STEP_DELAY;
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

            if(IsMouseButtonDown(BUTTON_PLACE) && grid->cursor.valid) PlaceStart(grid->cursor.position);
            else if(IsMouseButtonDown(BUTTON_REMOVE) && grid->cursor.valid) RemoveStart(grid->cursor.position);
            
            if(IsKeyboardButtonDown(KEY_FILL_GRID))
            {
                grid->Fill(PLACE_WALL);
                start.valid = false;
            }

            if(IsKeyboardButtonDown(KEY_CLEAR_GRID))
            {
                grid->Fill(PLACE_ROOM);
                start.valid = false;
            }

            if(IsKeyboardButtonDown(KEY_START_ALGO)) Run();

            start_timer = 0.0f;
            break;
        case ALGO_INIT:
            if(algo_pause) return;
            if(!start.valid) return;

            start_timer += Timer::deltaTime;
            if(start_timer < algo_start_delay) return;

            step_timer = algo_step_delay;

            algorithm->Init(*grid, start.position);

            algorithm_state = ALGO_RUN;
            break;
        case ALGO_RUN:
            if(algo_pause) return;
            step_timer += Timer::deltaTime;
            if(step_timer < algo_step_delay) return;

            algorithm->Step(*grid);

            if(algorithm->done)
                algorithm_state = ALGO_DONE;                

            step_timer = 0.0f;
            break;
        case ALGO_DONE:
            if(algo_pause) return;
            start.valid = false;
            grid->Reset();
            algorithm_state = ALGO_STAL;
            break;
        default:
            break;
        }
    }

    void Reset()
    {
        algo_pause = false;
        start.valid = false;
        grid->Reset();
        algorithm_state = ALGO_STAL;
    }
};

#include "Mazer/RecursiveBacktracking.h"
#include "Mazer/RecursiveDivision.h"

void Mazer::RegisterAlgorithms()
{
    algorithm_entry.emplace_back(
        RB::abbr,
        RB::name,
        RB::description,
        []() { return std::make_unique<RB>(); }
    );
    
    algorithm_entry.emplace_back(
        RD::abbr,
        RD::name,
        RD::description,
        []() { return std::make_unique<RD>(); }
    );
}