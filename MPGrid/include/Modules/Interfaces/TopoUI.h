void Topo::SidebarInterface(ApplicationContext& context)
{
    ImGui::BeginDisabled(context.interface.show_settings_window);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Tools"))
    {
        ImGui::PushStyleColor(ImGuiCol_Button, 
                (using_tool == BRUSH) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
        if(ImGui::Button("Brush (B)")) using_tool = BRUSH;
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, 
                (using_tool == BUCKET) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
        if(ImGui::Button("Bucket (G)")) using_tool = BUCKET;
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, 
                (using_tool == RECTANGLE) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
        if(ImGui::Button("Rectangle (S)")) using_tool = RECTANGLE;
        ImGui::PopStyleColor();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Actions"))
    {
        if(ImGui::Button("Reset (R)")) context.grid.ClearColors();

        ImGui::SameLine();
        if(ImGui::Button("Fill (F)")) context.grid.Fill(CELL_WALL);
        
        ImGui::SameLine();
        if(ImGui::Button("Clear (C)")) context.grid.Fill(CELL_ROOM);
        
        if(ImGui::Button("Settings (LShift)"))
            context.interface.show_settings_window = true;
    }
    
    ImGui::EndDisabled();
}

void Topo::SettingsInterface(ApplicationContext& context)
{
    
}