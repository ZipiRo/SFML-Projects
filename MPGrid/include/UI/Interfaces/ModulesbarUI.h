void InterfaceManager::Modulesbar(ApplicationContext &context)
{
    Vector2f position = context.interface.GetModulesbarPosition();
    Vector2f size = context.interface.GetModulesbarSize();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(ImVec2(position.x, position.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y), ImGuiCond_Always);
    ImGui::Begin("##Modulesbar", nullptr, flags);

    ImGui::PushStyleColor(ImGuiCol_Button, 
            (0 == context.using_module) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    if(ImGui::ImageButton("Topo", ResourceManager::Textures.Get("Map"), Vector2f(20, 20))) context.SetModule(0);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::SameLine(-1, 11.5f);
    ImGui::Text("1");
    
    ImGui::PushStyleColor(ImGuiCol_Button, 
            (1 == context.using_module) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    if(ImGui::ImageButton("Path", ResourceManager::Textures.Get("Path"), Vector2f(20, 20))) context.SetModule(1);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::SameLine(-1, 11.5f);
    ImGui::Text("2");

    ImGui::PushStyleColor(ImGuiCol_Button, 
            (2 == context.using_module) ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    if(ImGui::ImageButton("Maze", ResourceManager::Textures.Get("Maze"), Vector2f(20, 20))) context.SetModule(2);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::SameLine(-1, 11.5f);
    ImGui::Text("3");

    ImGui::End();
}