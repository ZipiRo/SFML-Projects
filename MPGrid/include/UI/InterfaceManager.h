#include "Interfaces/PopupUI.h"

class InterfaceManager
{
private:
    Vector2f grid_offset;

    void Menubar(ApplicationContext&);
    void Bottombar(ApplicationContext&);
    void Modulesbar(ApplicationContext&);

    void SidebarWindowBase(std::string title, ApplicationContext &context, std::function<void(ApplicationContext&)> Interface)
    {
        Vector2u window_size = context.window.getSize();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;
        

        ImGui::SetNextWindowPos(ImVec2(window_size.x - context.interface.GetSidebarWidth(), context.interface.GetMenubarHeight()), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(context.interface.GetSidebarWidth(), window_size.y - context.interface.GetMenubarHeight() * 2), ImGuiCond_Always);

        ImGui::Begin(title.c_str(), nullptr, flags);

        Interface(context);

        ImGui::End();
    }
    
    void SettingsWindowBase(std::string title, ApplicationContext &context, std::function<void(ApplicationContext&)> Interface)
    {
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
        {
            context.interface.show_settings_window = false;
            return;
        }

        Vector2u window_size = context.window.getSize();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;

        ImGui::SetNextWindowPos(ImVec2(context.interface.GetSettingsWindowPosition().x, context.interface.GetSettingsWindowPosition().y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(context.interface.GetSettingsWindowSize().x, context.interface.GetSettingsWindowSize().y), ImGuiCond_Always);

        ImGui::Begin(title.c_str(), &context.interface.show_settings_window, flags);

        Interface(context);

        ImGui::End();
    }

    void CenteredPopUpBase(std::string title, bool &open, ApplicationContext &context, std::function<void(ApplicationContext&)> Interface)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;
        

        if(ImGui::IsKeyDown(ImGuiKey_Escape))
        {
            open = false;
            return;
        }

        ImGui::SetNextWindowPos(ImVec2(context.window.getSize().x / 2, context.window.getSize().y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin(title.c_str(), &open, flags);

        Interface(context);

        ImGui::End();
    }
    
    bool UpdatePopUpUI(ApplicationContext &context)
    {
        if(context.interface.save_grid_popup)
        {
            CenteredPopUpBase
            (
                "Save Grid",
                context.interface.save_grid_popup, 
                context, 
                [&](ApplicationContext&) {return PopupInterface::SaveGrid(context);}
            );

            return true;
        }

        if(context.interface.load_grid_popup)
        {
            CenteredPopUpBase
            (
                "Load Grid",
                context.interface.load_grid_popup, 
                context, 
                [&](ApplicationContext&) {return PopupInterface::LoadGrid(context);}
            );

            return true;
        }
        
        if(context.interface.resize_grid_popup)
        {
            CenteredPopUpBase
            (
                "Resize Grid",
                context.interface.resize_grid_popup, 
                context, 
                [&](ApplicationContext&) {return PopupInterface::ResizeGrid(context);}
            );

            return true;
        }
        
        if(context.interface.themes_grid_popup)
        {
            CenteredPopUpBase
            (
                "Grid Themes",
                context.interface.themes_grid_popup, 
                context, 
                [&](ApplicationContext&) {return PopupInterface::GridColorThemes(context);}
            );

            return true;
        }
        
        if(context.interface.set_background_popup)
        {
            CenteredPopUpBase
            (
                "Set Background Color",
                context.interface.set_background_popup, 
                context, 
                [&](ApplicationContext&) {return PopupInterface::SetBackgroundColor(context);}
            );

            return true;
        }
        
        if(context.interface.show_keybinds_popup)
        {
            CenteredPopUpBase
            (
                "Keybinds",
                context.interface.show_keybinds_popup, 
                context, 
                [&](ApplicationContext&) {return PopupInterface::ShowKeybinds(context);}
            );

            return true;
        }

        return false;
    }

    void UpdateToolUI(ApplicationContext &context, Module &active)
    {
        if(Input::IsKeyDown(Keyboard::Key::Tab))
            context.interface.show_sidebar_window = !context.interface.show_sidebar_window;
            
        if(Input::IsKeyDown(Keyboard::Key::LShift))
            context.interface.show_settings_window = !context.interface.show_settings_window;

        if(context.interface.show_modules_bar)
            Modulesbar(context);

        if(context.interface.show_sidebar_window) 
        {
            SidebarWindowBase(active.GetSidebarTitle(), context, [&](ApplicationContext&){return active.SidebarInterface(context);});
            grid_offset.x = -context.interface.GetSidebarWidth();
        }

        if(context.interface.show_settings_window) 
            SettingsWindowBase(active.GetSettingsTitle(), context, [&](ApplicationContext&){return active.SettingsInterface(context);});
    }

public:
    void Update(ApplicationContext &context, Module &active)
    {
        context.interface.show_popup = UpdatePopUpUI(context);

        if(context.interface.show_popup)
        {
            context.grid_render.SetOffset(Vector2f(0, 0));
            return;
        }

        if(Input::IsKeyDown(Keyboard::Key::H))
            context.interface.hide_interface = !context.interface.hide_interface;

        grid_offset = Vector2f(0, 0);

        if(context.interface.hide_interface)
        {
            context.grid_render.SetOffset(Vector2f(0, 0));
            return;
        }
        
        Menubar(context);
        Bottombar(context);
        
        grid_offset.y = 0;

        UpdateToolUI(context, active);
        context.grid_render.SetOffset(grid_offset);
    }
};

#include "Interfaces/MenubarUI.h"
#include "Interfaces/BottombarUI.h"
#include "Interfaces/ModulesbarUI.h"