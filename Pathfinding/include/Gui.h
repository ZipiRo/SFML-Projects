bool IG_MENU_custom_grid_size_window = false;
bool IG_MENU_keybinds_window = false;
bool IG_MENU_paths_window = true;
bool IG_MENU_save_grid_window = false;
bool IG_MENU_load_grid_window = false;
void IGMenu()
{
    grid_offset.y = MENU_BAR_SIZE.y;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if(sim_state == SIM_STATE_SIMULATING) ImGui::BeginDisabled();

            if (ImGui::BeginMenu("GridSize"))
            {
                if (ImGui::MenuItem("5x5"))   { InitGrid(5, grid); }
                if (ImGui::MenuItem("10x10")) { InitGrid(10, grid); }
                if (ImGui::MenuItem("20x20")) { InitGrid(20, grid); }
                if (ImGui::MenuItem("40x40")) { InitGrid(40, grid); }
                if (ImGui::MenuItem("100x100")) { InitGrid(100, grid); }
                if (ImGui::MenuItem("Custom")){ IG_MENU_custom_grid_size_window = true; }
                ImGui::EndMenu();
            }

            if(sim_state == SIM_STATE_SIMULATING) ImGui::EndDisabled();
            
            if (ImGui::BeginMenu("MaxFPS"))
            {
                if (ImGui::MenuItem("60"))  { FPS = 60; window.setFramerateLimit(FPS); }
                if (ImGui::MenuItem("120")) { FPS = 120; window.setFramerateLimit(FPS); }
                if (ImGui::MenuItem("240")) { FPS = 240; window.setFramerateLimit(FPS); }
                if (ImGui::MenuItem("MAX")) { FPS = MAX_FPS; window.setFramerateLimit(FPS); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Grid"))
            {
                if (ImGui::MenuItem("Save (F1)")) IG_MENU_save_grid_window = true;
                if (ImGui::MenuItem("Load (F2)")) IG_MENU_load_grid_window = true;

                ImGui::EndMenu();
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::Button("Keybinds (K)"))
            IG_MENU_keybinds_window = !IG_MENU_keybinds_window;

        if (ImGui::Button("Paths (I)"))
            IG_MENU_paths_window = !IG_MENU_paths_window;

        if(ImGui::Button("Exit (Esc)"))
            window.close();

        ImGui::EndMainMenuBar();
    }
}

int CGS_grid_size;
void IGCustomGridSizeWindow()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(200, 100));
    ImGui::Begin("Custom Grid Size", nullptr, flags);
    ImGui::InputInt("Size", &CGS_grid_size);
    
    if(ImGui::Button("Apply"))
    {
        InitGrid(CGS_grid_size, grid);
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
    ImGui::Begin("Keybinds", nullptr, flags);
    
    ImGui::Image(key_textures["mouse_left"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place cells");

    ImGui::Image(key_textures["mouse_right"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo clear cells");

    ImGui::Image(key_textures["1"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place walls");

    ImGui::Image(key_textures["2"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place start point");
    
    ImGui::Image(key_textures["3"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo place finish point");

    ImGui::Image(key_textures["C"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo clear the grid");

    ImGui::Image(key_textures["R"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo reset to setup without clearing the grid");

    ImGui::Image(key_textures["P"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo toggle pause simulation");

    ImGui::Image(key_textures["H"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo hide all the ui on screen");

    ImGui::Image(key_textures["I"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo open the information window");

    ImGui::Image(key_textures["K"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo open this window");

    ImGui::Image(key_textures["F1"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo open save grid window");

    ImGui::Image(key_textures["F2"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo open load grid window");

    ImGui::Image(key_textures["Esc"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo close the program");

    ImGui::Image(key_textures["Space"], Vector2f(50, 50));
    ImGui::SameLine();
    ImGui::Text("\nTo start the simulation");
    
    ImGui::End();
}

void IGPathsWindow()
{
    grid_offset.x = -INFO_W_SIZE.x;
    std::string text;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(ImVec2(INFO_W_POS.x, INFO_W_POS.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(INFO_W_SIZE.x, INFO_W_SIZE.y), ImGuiCond_Always);

    ImGui::Begin("Paths", nullptr, flags);

    text = "FPS: " + std::to_string(current_FPS);
    ImGui::Text(text.c_str());

    text = "Grid size: " + std::to_string(grid_size) + 'x' + std::to_string(grid_size);
    ImGui::Text(text.c_str());

    text = "Placeing: " + place_type_string[placeing];
    ImGui::Text(text.c_str());

    text = "Start placed: "; start_placed ? text += "Yes" : text += "No";
    ImGui::Text(text.c_str());
    
    text = "Finish placed: "; finish_placed ? text += "Yes" : text += "No";
    ImGui::Text(text.c_str());

    text = "Simulation state: " + sim_state_string[sim_state];
    ImGui::Text(text.c_str());

    text = "Algorithm state: " + algo_state_strings[algo_state];
    ImGui::Text(text.c_str());
    
    text = "Algorithm Paused: "; algorithm_paused ? text += "Yes" : text += "No";
    ImGui::Text(text.c_str());

    text = "Time taken: " + std::to_string(algo_timer) + "seconds";
    ImGui::Text(text.c_str());

    text = "Path length: " + std::to_string(algorithm->path.size()) + " cells";
    ImGui::Text(text.c_str());

    ImGui::NewLine();

    ImGui::Text("Algorithm step delay");
    ImGui::SliderFloat("ASD", &simulation_step_delay, 0.0f, MAX_SIM_STEP_DELAY);
    
    ImGui::Text("Path step delay");
    ImGui::SliderFloat("PSD", &path_step_delay, 0.0f, MAX_PATH_STEP_DELAY);

    if(ImGui::Button("Reset Grid (R)"))
        ResetToSetup();
    
    ImGui::SameLine();

    ImGui::BeginDisabled(sim_state != SIM_STATE_SETUP);

    if(ImGui::Button("Clear Grid (C)"))
        ClearGrid(grid);

    ImGui::PushStyleColor(ImGuiCol_Button, 
        (sim_state == SIM_STATE_SIMULATING) ? ImVec4(0.2f,0.7f,0.2f,1.0f)
                                : ImGui::GetStyleColorVec4(ImGuiCol_Button));

    if(ImGui::Button("Start (Space)") && start_placed && finish_placed)
        sim_state = SIM_STATE_SIMULATING;

    ImGui::PopStyleColor();

    ImGui::EndDisabled();

    ImGui::SameLine();


    ImGui::PushStyleColor(ImGuiCol_Button, 
        (algorithm_paused == false) ? ImVec4(0.5f,0.0f,0.2f,1.0f)
                                : ImVec4(0.2f,0.7f,0.2f,1.0f));

    if(ImGui::Button(!algorithm_paused ? "Pause (P)" : "Play (P)"))
        algorithm_paused = !algorithm_paused;
    
        ImGui::PopStyleColor();

    ImGui::Text("Place:");

    ImGui::PushStyleColor(ImGuiCol_Button, 
        (placeing == PLACE_WALL) ? ImVec4(0.2f,0.7f,0.2f,1.0f)
                                : ImGui::GetStyleColorVec4(ImGuiCol_Button));

    if(ImGui::Button("Wall (1)"))
        placeing = PLACE_WALL;

    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, 
        (placeing == PLACE_START) ? ImVec4(0.2f,0.7f,0.2f,1.0f)
                                : ImGui::GetStyleColorVec4(ImGuiCol_Button));

    if(ImGui::Button("Start (2)"))
        placeing = PLACE_START;

    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, 
        (placeing == PLACE_FINISH) ? ImVec4(0.2f,0.7f,0.2f,1.0f)
                                : ImGui::GetStyleColorVec4(ImGuiCol_Button));
    
    if(ImGui::Button("Finish (3)"))
        placeing = PLACE_FINISH;

    ImGui::PopStyleColor();

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

char save_file[100];
void IGSaveGridWindow()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(500, 100));
    ImGui::Begin("Save Grid", nullptr, flags);

    ImGui::InputText("Save Name", save_file, 100);
    ImGui::SameLine();
    ImGui::Text("(.grid)");

    if(ImGui::Button("Save (Enter)") || ImGui::IsKeyDown(ImGuiKey_Enter))
    {
        if(strlen(save_file) > 0)
        {
            SaveGrid(grid, save_file);
            save_file[0] = '\0';
            IG_MENU_save_grid_window = false;
        }
    }

    ImGui::SameLine();

    if(ImGui::Button("Close"))
    {
        IG_MENU_save_grid_window = false;
    }

    ImGui::End();
}
 
char file_path[100];
bool file_error = false;
void IGLoadGridWindow()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(500, 100));
    ImGui::Begin("Load Grid", nullptr, flags);
    ImGui::InputText("Load File", file_path, 100);

    if(file_error) ImGui::Text("File had some errors!");
    
    if(ImGui::Button("Load (Enter)") || ImGui::IsKeyDown(ImGuiKey_Enter))
    {
        if(strlen(file_path) > 0)
        {
            if(!LoadGrid(grid, file_path)) file_error = true;
            else {
                IG_MENU_load_grid_window = false;
                file_error = false;    
            }

            file_path[0] = '\0';
        }
    }

    ImGui::SameLine();

    if(ImGui::Button("Close"))
    {
        IG_MENU_load_grid_window = false;
    }

    ImGui::End();
}