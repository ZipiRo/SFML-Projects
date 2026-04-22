namespace PopupInterface
{
    bool lock_size_input = false;
    int size_input[2] = {};

    void SaveGrid(ApplicationContext &context)
    {
        ImGui::SetWindowSize(ImVec2(200, 100), ImGuiCond_Always);
    }   

    void LoadGrid(ApplicationContext &context)
    {
        ImGui::SetWindowSize(ImVec2(200, 100), ImGuiCond_Always);
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
            Vector2f cell_size(float(context.window.getSize().y - 50) / context.grid.GetSize().x, float(context.window.getSize().y - 50) / context.grid.GetSize().y);
            context.grid_render.Build(cell_size, context.grid.GetSize());
            context.grid_cursor.Init(cell_size);
            context.interface.resize_grid_popup = false;
        }
    }  
}