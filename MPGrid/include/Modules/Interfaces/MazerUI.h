void Mazer::SidebarInterface(ApplicationContext& context)
{
    ImGui::BeginDisabled(context.interface.show_settings_window);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Algorithms"))
    {
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
            
            ImGui::PopStyleColor();
            
            offset += button_width + ImGui::GetStyle().ItemSpacing.x; 
        }
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Algorithm Settings"))
    {
        ImGui::Text("Start Delay");
        if(ImGui::Button("|##1")) start_algo_delay = 0;
        ImGui::SameLine();
        ImGui::SliderInt("##AlgoStartDelay", &start_algo_delay, 0, 5);
        ImGui::SameLine();
        if(ImGui::Button("Default##1")) start_algo_delay = 3;

        ImGui::Text("Step Delay");
        if(ImGui::Button("|##2")) algo_step_delay = 0;
        ImGui::SameLine();
        ImGui::SliderFloat("##AlgoStepDelay", &algo_step_delay, 0.0f, 0.2f);
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
        ImGui::EndDisabled();

        ImGui::SameLine();
        if(ImGui::Button("Reset (R)")) Reset();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (pause_algorithm) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImVec4(0.7f, 0.2f, 0.0f, 1.0f));
        if(ImGui::Button(pause_algorithm ? "Play (P)" : "Pause (P)"))
            pause_algorithm = !pause_algorithm;
        ImGui::PopStyleColor();

        if(ImGui::Button("Fill (F)")) 
        {
            context.grid.Fill(CELL_WALL);
            start.valid = false;
        }
        
        ImGui::SameLine();
        if(ImGui::Button("Clear (C)")) 
        {
            context.grid.Fill(CELL_ROOM);
            start.valid = false;
        }

        ImGui::BeginDisabled(running_algorithm);
        if(ImGui::Button("Random Start (T)")) RandomStart(context.grid, context.grid_render.GetColorTheme());
        ImGui::EndDisabled();
        
        if(ImGui::Button("Settings (LShift)"))
            context.interface.show_settings_window = true;
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
    
}