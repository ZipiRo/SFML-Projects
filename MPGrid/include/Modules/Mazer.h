class Mazer : public Module
{
private:
    enum AlgoState
    {
        ALGO_STAL,
        ALGO_INIT,
        ALGO_RUN,
        ALGO_DONE
    };

    Point start;

    std::unique_ptr<MazeAlgorithm> algorithm;
    const std::vector<MazeAlgorithmEntry> algorithms = GetMazeAlgorithms();

    bool pause_algorithm;
    bool running_algorithm;
    bool reset_grid;
    int using_algorithm;

    bool no_step_algorithm;

    float step_timer;
    float start_timer;

    float algo_step_delay;
    int start_algo_delay;

    AlgoState algorithm_state;

    void SidebarInterface(ApplicationContext&) override;
    void SettingsInterface(ApplicationContext&) override;

    void PlaceStart(Vector2i where, Grid &grid, GridColorTheme theme)
    {
        if(!start.valid)
        {
            start.position = where;
            start.valid = true;

            grid.SetCell(where.x, where.y, CELL_NONE, theme.colors[MazeStartColor]);
        }
    }

    void RemoveStart(Vector2i where, Grid &grid)
    {
        if(where == start.position && start.valid)
        {
            start.valid = false;
            grid.SetCell(where.x, where.y, CELL_ROOM);
        }
    }

    void RandomStart(Grid &grid, GridColorTheme theme)
    {
        Vector2i random_position = Vector2i(rand() % grid.GetSize().x, rand() % grid.GetSize().y);
        while (!start.valid)
        {
            random_position = Vector2i(rand() % grid.GetSize().x, rand() % grid.GetSize().y);
            PlaceStart(random_position, grid, theme);
        }
    }

    void Reset()
    {
        algorithm_state = ALGO_STAL;
        pause_algorithm = false;
        running_algorithm = false;
        start.valid = false;
        reset_grid = true;
    }

    void Run(GridColorTheme theme)
    {
        if(!start.valid) return;
        running_algorithm = true;
        start_timer = 0.0f;

        algorithm = algorithms[using_algorithm].Get();
        algorithm->primary_color = theme.colors[MazePrimaryColor];
        algorithm->secondary_color = theme.colors[MazeSecondaryColor];

        algorithm_state = ALGO_INIT;
    }

    std::string GetSidebarTitle() override
    {
        return "Mazer";
    }

    std::string GetSettingsTitle() override 
    {
        return "Mazer Settings";
    }

    void AlgorithmUpdate(Grid &grid, float delta_time);
public:
    Mazer() 
    {
        pause_algorithm = false;
        using_algorithm = 0;
        reset_grid = false;
        algorithm_state = ALGO_STAL;

        algo_step_delay = 0.05f;
        start_algo_delay = 0;

        no_step_algorithm = false;
    }

    void Init(ApplicationContext &context) override
    {
        Reset();
    }

    void Update(ApplicationContext &context) override
    {   
        if(Input::IsKeyDown(Keyboard::Key::R))
            Reset();

        if(Input::IsKeyDown(Keyboard::Key::P))
            pause_algorithm = !pause_algorithm;

        if(reset_grid)
        {
            context.grid.ClearColors();
            reset_grid = false;
        }

        if(running_algorithm)
        {
            AlgorithmUpdate(context.grid, context.delta_time);
            return;
        }

        Vector2i mouse_position = Mouse::getPosition(context.window);
        Point cursor = context.grid_cursor.GetCursorPoint(mouse_position, context.grid_render);

        if(Input::IsMouseButtonDown(Mouse::Button::Left) && cursor.valid)
            PlaceStart(cursor.position, context.grid, context.grid_render.GetColorTheme());
        
        if(Input::IsMouseButtonDown(Mouse::Button::Right) && cursor.valid)
            RemoveStart(cursor.position, context.grid);

        if(Input::IsKeyDown(Keyboard::Key::T))
            RandomStart(context.grid, context.grid_render.GetColorTheme());

        if(Input::IsKeyDown(Keyboard::Key::F))
        {
            context.grid.Fill(CELL_WALL);
            start.valid = false;
        }

        if(Input::IsKeyDown(Keyboard::Key::C))
        {
            context.grid.Fill(CELL_ROOM);
            start.valid = false;
        }
     
        if(Input::IsKeyDown(Keyboard::Key::Space))
            Run(context.grid_render.GetColorTheme());
        
        if(Input::IsKey(Keyboard::Key::LControl) && Input::MouseWheelDelta() > 0)
            using_algorithm = std::min((int)algorithms.size() - 1, using_algorithm + 1);

        if(Input::IsKey(Keyboard::Key::LControl) && Input::MouseWheelDelta() < 0)
            using_algorithm = std::max(0, using_algorithm - 1);
    }
};

void Mazer::AlgorithmUpdate(Grid &grid, float delta_time)
{
    if(pause_algorithm) return;

    switch (algorithm_state)
    {
    case ALGO_INIT:
        start_timer += delta_time;
        if(start_timer < start_algo_delay) return;

        algorithm->Init(grid, start.position);

        step_timer = algo_step_delay;
        algorithm_state = ALGO_RUN;
        break;
    case ALGO_RUN:
        step_timer += delta_time;
        if(step_timer < algo_step_delay) return;

        no_step_algorithm ? algorithm->Direct(grid) : algorithm->Step(grid);

        SoundPlayer::Play(ResourceManager::Sounds.Get("Remove"));

        if(algorithm->done)
            algorithm_state = ALGO_DONE;

        step_timer = 0.0f;
        break;
    case ALGO_DONE: 
        Reset();
        break;
    default:
        break;
    }
}

void Mazer::SidebarInterface(ApplicationContext& context)
{
    ImGui::BeginDisabled(context.interface.show_settings_window);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Algorithms (Ctrl + Wheel)"))
    {
        ImGui::BeginDisabled(running_algorithm);
        float offset = 0;
        for(int i = 0; i < algorithms.size(); i++)
        {
            ImVec2 text_size = ImGui::CalcTextSize(algorithms[i].abbr.c_str());
            float button_width = text_size.x + 20.0f;

            if(offset + button_width > context.interface.GetSidebarWidth()) offset = 0;
            else if(offset > 0) ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, 
            (using_algorithm == i) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f)
                                    : ImGui::GetStyleColorVec4(ImGuiCol_Button));

            if(ImGui::Button(algorithms[i].abbr.c_str(), ImVec2(button_width, 0)))
                using_algorithm = i;
            if(ImGui::IsItemHovered())
                ImGui::SetTooltip("Maze generator algorithm");
            
            ImGui::PopStyleColor();
            
            offset += button_width + ImGui::GetStyle().ItemSpacing.x; 
        }
        ImGui::EndDisabled();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Algorithm Settings"))
    {
        ImGui::Text("Start Delay");
        if(ImGui::Button("|##1")) start_algo_delay = 0;
        ImGui::SameLine();
        ImGui::SliderInt("##AlgoStartDelay", &start_algo_delay, 0, 5);
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Set the dealy to start the algorithm");
        ImGui::SameLine();
        if(ImGui::Button("Default##1")) start_algo_delay = 3;

        ImGui::Text("Step Delay");
        if(ImGui::Button("|##2")) algo_step_delay = 0;
        ImGui::SameLine();
        ImGui::SliderFloat("##AlgoStepDelay", &algo_step_delay, 0.0f, 0.2f);
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("How fast the algorithm moves");
        ImGui::SameLine();
        if(ImGui::Button("Default##2")) algo_step_delay = 0.05f;
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Actions"))
    {
        ImGui::BeginDisabled(running_algorithm);
        ImGui::PushStyleColor(ImGuiCol_Button, 
        (running_algorithm) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
        if(ImGui::Button("Start (Space)")) 
            Run(context.grid_render.GetColorTheme());
        ImGui::PopStyleColor();
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Start the algorithm");
        ImGui::EndDisabled();

        ImGui::SameLine();
        if(ImGui::Button("Reset (R)")) Reset();
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Reset the algorithm and clear colors");

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (pause_algorithm) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImVec4(0.7f, 0.2f, 0.0f, 1.0f));
        if(ImGui::Button(pause_algorithm ? "Play (P)" : "Pause (P)"))
            pause_algorithm = !pause_algorithm;
        ImGui::PopStyleColor();
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Pause the algorithm");

        if(ImGui::Button("Fill (F)")) 
        {
            context.grid.Fill(CELL_WALL);
            start.valid = false;
        }
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Fill grid with walls");
        
        ImGui::SameLine();
        if(ImGui::Button("Clear (C)")) 
        {
            context.grid.Fill(CELL_ROOM);
            start.valid = false;
        }
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Clear walls from the grid");

        ImGui::BeginDisabled(running_algorithm);
        if(ImGui::Button("Random Start (T)")) RandomStart(context.grid, context.grid_render.GetColorTheme());
        ImGui::EndDisabled();
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Set random start positition");
        
        if(ImGui::Button("Settings (LShift)"))
            context.interface.show_settings_window = true;
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Show settings");
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Algorithm Info"))
    {
        ImGui::Text(std::string("Name:\n" + algorithms[using_algorithm].name).c_str());
        ImGui::TextWrapped(std::string("Description:\n" + algorithms[using_algorithm].desc).c_str());
    }

    ImGui::EndDisabled();
}

void Mazer::SettingsInterface(ApplicationContext& context)
{
    bool use_custom_theme = context.grid_render.IsCustomTheme();
    if(ImGui::Checkbox("Custom colors", &use_custom_theme))
    {
        if(use_custom_theme) context.grid_render.UseCustomTheme();
        else context.grid_render.UsePresetTheme();
    }

    ImGui::SetNextItemOpen(use_custom_theme, ImGuiCond_Always);
    if(ImGui::CollapsingHeader("Custom Theme"))
    {
        GridColorTheme custom_theme = context.grid_render.GetColorTheme();
        ImVec4 imgui_color;

        bool changed_colors = false;
            
        imgui_color = SFMLToImColor(custom_theme.colors[MazeStartColor]);
        if(ImGui::ColorEdit4("Start Color", (float*)&imgui_color))
        {
            custom_theme.colors[MazeStartColor] = ImColorToSFML(imgui_color);
            changed_colors = true;
        }
            
        imgui_color = SFMLToImColor(custom_theme.colors[MazePrimaryColor]);
        if(ImGui::ColorEdit4("Primary Color", (float*)&imgui_color))
        {
            custom_theme.colors[MazePrimaryColor] = ImColorToSFML(imgui_color);
            changed_colors = true;
        }
            
        imgui_color = SFMLToImColor(custom_theme.colors[MazeSecondaryColor]);
        if(ImGui::ColorEdit4("Secondary Color", (float*)&imgui_color))
        {
            custom_theme.colors[MazeSecondaryColor] = ImColorToSFML(imgui_color);
            changed_colors = true;
        }

        if(changed_colors)
        {
            context.grid_render.SetColorTheme(custom_theme);
        }
    }
}