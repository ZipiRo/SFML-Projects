namespace PopupInterface
{
    bool lock_size_input = true;
    int size_input[2] = {};
    std::vector<std::string> files;
    char savefile_input[100];

    void SaveGrid(ApplicationContext &context)
    {        
        ImGui::SetWindowSize(ImVec2(335, 300), ImGuiCond_Always);

        ImGui::Text(std::string("File Name (" + GridSaveFileExt + ')').c_str());
        ImGui::InputText("##FileName ", savefile_input, 100);
        ImGui::SameLine();
        if((ImGui::Button("Save (Enter)") || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter)) && 
            !std::string(savefile_input).empty())
        {
            context.grid.Save(savefile_input);
            context.interface.save_grid_popup = false;
        }

        ImGui::Text(std::string(GridSaveFileDir).c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Refresh").x - 10);
        if(ImGui::Button("Refresh"))
            files = GetStrFilesFrom(GridSaveFileDir, GridSaveFileExt);

        ImGui::BeginChild("FileList", ImVec2(0, -1), true);
        for(auto &file : files)
        {
            if(ImGui::Selectable(file.c_str()))
                strcpy(savefile_input, file.c_str());
        }
        ImGui::EndChild();
    }   

    void LoadGrid(ApplicationContext &context)
    {
        ImGui::SetWindowSize(ImVec2(335, 300), ImGuiCond_Always);
        
        ImGui::Text(std::string("File Name (" + GridSaveFileExt + ')').c_str());
        ImGui::InputText("##FileName", savefile_input, 100);
        ImGui::SameLine();
        if((ImGui::Button("Load (Enter)") || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter)) && 
            !std::string(savefile_input).empty())
        {
            if(!context.grid.Load(savefile_input)) return;
            context.grid_render.Build(context.grid.GetSize());
            context.grid_cursor.Init(context.grid_render.GetCellSize());
            context.interface.load_grid_popup = false;       
        }

        ImGui::Text(std::string(GridSaveFileDir).c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Refresh").x - 10);
        if(ImGui::Button("Refresh"))
            files = GetStrFilesFrom(GridSaveFileDir, GridSaveFileExt);

        ImGui::BeginChild("FileList", ImVec2(0, -1), true);
        for(auto &file : files)
        {
            if(ImGui::Selectable(file.c_str()))
                strcpy(savefile_input, file.c_str());

            if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {   
                if(!context.grid.Load(savefile_input)) return;
                context.grid_render.Build(context.grid.GetSize());
                context.grid_cursor.Init(context.grid_render.GetCellSize());
                context.interface.load_grid_popup = false;     
            }
        }
        ImGui::EndChild();
    }   
    
    void ResizeGrid(ApplicationContext &context)
    {
        ImGui::SetWindowSize(ImVec2(360, 80), ImGuiCond_Always);
        ImGui::Text("Cols"); 
        ImGui::SameLine(145.0f);

        ImGui::Text("Rows");

        ImGui::SetNextItemWidth(100.0f);
        ImGui::InputInt("##Colls", &size_input[0]);
        
        ImGui::SameLine();
        if(ImGui::Checkbox("##LockSize", &lock_size_input))
        {
            if(lock_size_input)
                size_input[1] = size_input[0];
        }
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("Make the rows and cols equal");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        ImGui::InputInt("##Rows", &size_input[1]);
        
        ImGui::SameLine();
        bool changes = false;
        if(size_input[0] != context.grid.GetSize().x || size_input[1] != context.grid.GetSize().y)
        {
            changes = true;

            if(lock_size_input)
                size_input[1] = size_input[0];
        }

        if((ImGui::Button("Apply (Enter)") || ImGui::IsKeyDown(ImGuiKey_Enter)) && changes)
        {
            context.grid.Create(size_input[0], size_input[1]);
            context.grid_render.Build(context.grid.GetSize());
            context.grid_cursor.Init(context.grid_render.GetCellSize());
            context.interface.resize_grid_popup = false;
        }
    }  

    void GridColorThemes(ApplicationContext &context)
    {
        ImGui::SetWindowSize(ImVec2(300, 500));

        ImGui::SetNextItemWidth(-1);
        if(ImGui::BeginCombo("##Themes", context.grid_render.GetColorTheme().name.c_str()))
        {
            for(int i = 0; i < context.grid_render.grid_color_themes.size(); i++)
            {
                bool is_selected = false;

                if(ImGui::Selectable(context.grid_render.grid_color_themes[i].name.c_str(), is_selected))
                    context.grid_render.UsePresetTheme(i);

                if(is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        GridColorTheme grid_current_theme = context.grid_render.GetColorTheme();
        
        ImVec4 imgui_color;
        for(auto &[name, color] : grid_current_theme.colors)
        {
            imgui_color = SFMLToImColor(color);
            ImGui::ColorButton(std::string("##" + name).c_str(), imgui_color, ImGuiColorEditFlags_NoTooltip, ImVec2(30, 30));
            ImGui::SameLine();
            ImGui::Text(std::string("\n" + name).c_str());
        }
    }

    void SetBackgroundColor(ApplicationContext &context)
    {
        ImGui::SetWindowSize(ImVec2(400, 60), ImGuiCond_Always);
            
        ImVec4 imgui_color = SFMLToImColor(ApplicationBase::background_color);
        if(ImGui::ColorEdit4("Background Color", (float*)&imgui_color))
            ApplicationBase::background_color = ImColorToSFML(imgui_color);
    }  

    void ShowKeybinds(ApplicationContext &context)
    {
        ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_Always);
        
        if (ImGui::BeginTable("Keybinds", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Key");
            ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

            auto Row = [](const char* key, const char* action)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", key);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", action);
            };

            Row("LMB", "Place");
            Row("RMB", "Remove");
            Row("LShift", "Show module settings");
            Row("Esc", "Close windows/popups");
            Row("Num1", "Topografy module");
            Row("Num2", "Pathfinding module");
            Row("Num3", "Maze generator module");
            Row("Tab", "Hide sidebar");
            Row("Space", "Start algorithm");
            Row("P", "Pause algorithm");
            Row("H", "Hide interface");
            Row("T", "Random algorithm positions");
            Row("F", "Fill grid");
            Row("C", "Clear grid");
            Row("R", "Reset algorithm/grid");
            Row("Ctrl + Wheel", "Switch algorithm");

            ImGui::EndTable();
        }
    }  
}