namespace BottombarInterface
{
    int current_fps = 0;
    float fps_timer = 1.0f;

    void FPS(ApplicationContext &context)
    {
        fps_timer += context.delta_time;
        if(fps_timer >= 1.0f) 
        {
            current_fps = 1.0f / context.delta_time;
            fps_timer = 0.0f;
        }

        ImGui::Text(std::string("FPS: " + std::to_string(current_fps)).c_str());
    }
};

void InterfaceManager::Bottombar(ApplicationContext &context)
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoMove | 
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoTitleBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
    ImGui::SetNextWindowPos(ImVec2(0, context.window.getSize().y - context.interface.GetMenubarHeight()), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(context.window.getSize().x, context.interface.GetMenubarHeight()), ImGuiCond_Always);
    ImGui::Begin("##Bottombar", nullptr, flags);

    BottombarInterface::FPS(context);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}