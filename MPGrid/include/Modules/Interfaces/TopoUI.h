void Topo::SidebarInterface(ApplicationContext& context)
{
    ImGui::BeginDisabled(context.interface.show_settings_window);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Actions"))
    {
        if(ImGui::Button("Reset (R)")) 
            context.grid.ClearColors();

        ImGui::SameLine();
        if(ImGui::Button("Fill (F)")) 
            context.grid.Fill(CELL_WALL);
        
        ImGui::SameLine();
        if(ImGui::Button("Clear (C)")) 
            context.grid.Fill(CELL_ROOM);
        
        if(ImGui::Button("Settings (LShift)"))
        {
            context.interface.show_settings_window = true;
        }
    }
    
    ImGui::EndDisabled();
}

void Topo::SettingsInterface(ApplicationContext& context)
{
    
}