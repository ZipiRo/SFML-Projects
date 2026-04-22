class Topo : public Module
{
private:
    void SidebarInterface(ApplicationContext&) override;
    void SettingsInterface(ApplicationContext&) override;

    std::string GetSidebarTitle() override
    {
        return "Topo";
    }

    std::string GetSettingsTitle() override 
    {
        return "Topo Settings";
    }

public:
    Topo() {}

    void Init(ApplicationContext &context) override
    {
        context.grid.ClearColors();
    }

    void Update(ApplicationContext &context) override
    {
        if(context.interface.show_settings_window || 
            context.interface.show_popup) return;
        
        Vector2i mouse_position = Mouse::getPosition(context.window);
        Point cursor = context.grid_cursor.GetCursorPoint(mouse_position, context.grid_render);

        if(Input::IsMouseButton(Mouse::Button::Left) && cursor.valid)
            context.grid.SetCell(cursor.position.x, cursor.position.y, CELL_WALL);
        
        if(Input::IsMouseButton(Mouse::Button::Right) && cursor.valid)
            context.grid.SetCell(cursor.position.x, cursor.position.y, CELL_ROOM);

        if(Input::IsKeyDown(Keyboard::Key::F))
            context.grid.Fill(CELL_WALL);

        if(Input::IsKeyDown(Keyboard::Key::C))
            context.grid.Fill(CELL_ROOM);
            
        if(Input::IsKeyDown(Keyboard::Key::R))
            context.grid.ClearColors();
    }
};

#include "Interfaces/TopoUI.h"