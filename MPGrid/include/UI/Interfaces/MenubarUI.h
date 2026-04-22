namespace MenubarInterface
{
    void GridMenu(ApplicationContext &context)
    {
        if(ImGui::BeginMenu("Grid"))
        {   
            if(ImGui::MenuItem("Themes"))
            {
                context.interface.themes_grid_popup = true;
            }

            if(ImGui::MenuItem("Resize"))
            {
                PopupInterface::size_input[0] = context.grid.GetSize().x;
                PopupInterface::size_input[1] = context.grid.GetSize().y;
                
                context.interface.resize_grid_popup = true;
            }

            if (ImGui::MenuItem("Save"))
            {
                PopupInterface::savefile_input[0] = '\0';

                context.interface.save_grid_popup = true;
            }

            if (ImGui::MenuItem("Load"))
            {
                PopupInterface::savefile_input[0] = '\0';
                
                context.interface.load_grid_popup = true;
            }

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