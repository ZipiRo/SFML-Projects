namespace MenubarInterface
{
    void GridMenu(ApplicationContext &context)
    {
        if(ImGui::BeginMenu("Grid"))
        {   
            if(ImGui::MenuItem("Resize"))
                context.interface.resize_grid_popup = true;

            if (ImGui::MenuItem("Save"))
                context.interface.save_grid_popup = true;

            if (ImGui::MenuItem("Load"))
                context.interface.load_grid_popup = true;
            
            ImGui::EndMenu();
        }
    }

};

void InterfaceManager::Menubar(ApplicationContext &context)
{
    ImGui::BeginMainMenuBar();

    MenubarInterface::GridMenu(context);

    ImGui::EndMainMenuBar();
}