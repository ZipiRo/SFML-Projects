class Topo : public Module
{
private:
    enum TopoTool
    {
        BRUSH,
        BUCKET,
        RECTANGLE
    };

    TopoTool using_tool = BRUSH;

    RectangleShape rect_tool_shape;
    Mouse::Button rect_tool_hold_button;
    bool rect_tool_holding = false;
    CellType rect_tool_type;

    Vector2f drag_start;
    Vector2f drag_end;

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

    void BrushTool(Vector2i where, CellType type, Grid &grid)
    {
        if(grid.Get(where.x, where.y).type == type) return;
        grid.SetCell(where.x, where.y, type);
        SoundPlayer::Play(ResourceManager::Sounds.Get("Place"));
    }

    void BucketTool(Vector2i where, CellType type, Grid &grid)
    {
        if(grid.Get(where.x, where.y).type == type) return;

        std::queue<Vector2i> queue;
        std::vector<std::vector<bool>> visited;
        visited.assign(grid.GetSize().y, std::vector<bool>(grid.GetSize().x));

        visited[where.y][where.x] = true;
        queue.push(where);

        while (!queue.empty())
        {
            Vector2i current = queue.front();
            queue.pop();
                    
            grid.SetCell(current.x, current.y, type);

            for(const auto &direction : directions)
            {
                Vector2i next = current + direction;

                if(next.x < 0 || next.y < 0 || next.x >= grid.GetSize().x || next.y >= grid.GetSize().y) 
                    continue;

                if(grid.Get(next.x, next.y).type != type && !visited[next.y][next.x])
                {
                    visited[next.y][next.x] = true;
                    grid.SetCell(next.x, next.y, type);
                    queue.push(next);
                }
            }
        }

        SoundPlayer::Play(ResourceManager::Sounds.Get("Place"));
    }
    
    void RectangleDraw(Vector2f start, Vector2f end, CellType type, ApplicationContext &context)
    {
        Vector2f grid_position = context.grid_render.GetOffsetPosition(); 
        Vector2f grid_length = context.grid_render.GetLength();
        Vector2f cell_size = context.grid_render.GetCellSize();

        start.x = std::clamp(start.x, grid_position.x, grid_position.x + grid_length.x);
        start.y = std::clamp(start.y, grid_position.y, grid_position.y + grid_length.y);
        end.x = std::clamp(end.x, grid_position.x, grid_position.x + grid_length.x - cell_size.x);
        end.y = std::clamp(end.y, grid_position.y, grid_position.y + grid_length.y - cell_size.y);
        
        Vector2i local_start = context.grid_render.GetCellPoint(start).position;
        Vector2i local_end = context.grid_render.GetCellPoint(end).position;

        if(local_start.x > local_end.x) std::swap(local_start.x, local_end.x);
        if(local_start.y > local_end.y) std::swap(local_start.y, local_end.y);
        
        for(int x = local_start.x; x <= local_end.x; x++)
        {
            context.grid.SetCell(x, local_start.y, type);
            context.grid.SetCell(x, local_end.y, type);
        }
        
        for(int y = local_start.y; y <= local_end.y; y++)
        {
            context.grid.SetCell(local_start.x, y, type);
            context.grid.SetCell(local_end.x, y, type);
        }
    }   

    void StartRectangleTool(Vector2i mouse_position, CellType type, Color rect_tool_line_color, Mouse::Button hold_button)
    {
        if(!rect_tool_holding)
        {
            drag_start = Vector2f(mouse_position);

            rect_tool_shape.setPosition(drag_start);
            rect_tool_shape.setOutlineColor(rect_tool_line_color);
            rect_tool_hold_button = hold_button;
            rect_tool_type = type;
            rect_tool_holding = true;
        }
    }

    void RectangleTool(Vector2i mouse_position, ApplicationContext &context)
    {
        if(rect_tool_holding)
        {
            drag_end = Vector2f(mouse_position);
            rect_tool_shape.setSize(drag_end - drag_start);

            if(Input::IsMouseButtonUp(rect_tool_hold_button))
            {
                RectangleDraw(drag_start, drag_end, rect_tool_type, context);
                rect_tool_shape.setSize(Vector2f(0, 0));
                rect_tool_holding = false;
                SoundPlayer::Play(ResourceManager::Sounds.Get("Place"));
            }
        }
    }

public:
    Topo() 
    {
        rect_tool_shape.setFillColor(Color::Transparent);
        rect_tool_shape.setOutlineThickness(2);
    }

    void Init(ApplicationContext &context) override
    {
        context.grid.ClearColors();
    }

    void Update(ApplicationContext &context) override
    {   
        Vector2i mouse_position = Mouse::getPosition(context.window);
        Point cursor = context.grid_cursor.GetCursorPoint(mouse_position, context.grid_render);

        switch (using_tool)
        {
        case BRUSH:
            if(Input::IsMouseButton(Mouse::Button::Left) && cursor.valid)
                BrushTool(cursor.position, CELL_WALL, context.grid);
            
            else if(Input::IsMouseButton(Mouse::Button::Right) && cursor.valid)
                BrushTool(cursor.position, CELL_ROOM, context.grid);
            break;
        case BUCKET:
            if(Input::IsMouseButtonDown(Mouse::Button::Left) && cursor.valid)
                BucketTool(cursor.position, CELL_WALL, context.grid);

            else if(Input::IsMouseButtonDown(Mouse::Button::Right) && cursor.valid)
                BucketTool(cursor.position, CELL_ROOM, context.grid);
            break;
        case RECTANGLE:
            if(Input::IsMouseButtonDown(Mouse::Button::Left) && cursor.valid && !rect_tool_holding)
                StartRectangleTool
                (
                    mouse_position, 
                    CELL_WALL, 
                    context.grid_render.GetColorTheme().colors[WallColor], 
                    Mouse::Button::Left
                );

            else if(Input::IsMouseButtonDown(Mouse::Button::Right) && cursor.valid && !rect_tool_holding)
                StartRectangleTool
                (
                    mouse_position, 
                    CELL_ROOM, 
                    context.grid_render.GetColorTheme().colors[RoomColor],  
                    Mouse::Button::Right
                );
            
            RectangleTool(mouse_position, context);
            break;
        default:
            break;
        }

        if(Input::IsKeyDown(Keyboard::Key::F))
            context.grid.Fill(CELL_WALL);

        if(Input::IsKeyDown(Keyboard::Key::C))
            context.grid.Fill(CELL_ROOM);
            
        if(Input::IsKeyDown(Keyboard::Key::R))
            context.grid.ClearColors();
        
        if(Input::IsKeyDown(Keyboard::Key::B))
            using_tool = BRUSH;

        if(Input::IsKeyDown(Keyboard::Key::G))
            using_tool = BUCKET;

        if(Input::IsKeyDown(Keyboard::Key::S))
            using_tool = RECTANGLE;
        
    }

    void Draw(ApplicationContext &context) override 
    {
        if(rect_tool_holding && using_tool == RECTANGLE)
        {
            context.window.draw(rect_tool_shape);
        }
    }
};

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