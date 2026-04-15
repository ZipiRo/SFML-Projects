const Color DEFAULT_CELL_OUTLINE_COLOR = Color(20, 20, 20);
const Color DEFAULT_ROOM_COLOR = Color(40, 40, 40);
const Color DEFAULT_WALL_COLOR = DEFAULT_CELL_OUTLINE_COLOR;
const Color DEFAULR_CURSOR_COLOR = Color::Black;

enum CellType
{
    CELL_ROOM,
    CELL_WALL
};

enum PlaceType
{
    PLACE_ROOM,
    PLACE_WALL
};

struct Cell
{
    int type;
    Color *color;
};

class Grid
{
private:
    RectangleShape cell_shape;
    RectangleShape cursor_shape;

    void Init(const Vector2i &size, const Vector2f &max_length)
    {   
        show_sidebar = true;
        show_settings = false;

        this->size = size;
        this->max_length = max_length;

        cell_size = Vector2f(max_length.x / size.x, max_length.y / size.y);
        length = Vector2f(size.x * cell_size.x, size.y * cell_size.y);
        position = Vector2f((window_width + offset.x) / 2 - length.x / 2, (window_height + offset.y) / 2 - length.y / 2);
        
        graph = std::vector<std::vector<Cell>>(size.y, std::vector<Cell>(size.x));

        cell_shape = RectangleShape({1.0f, 1.0f});
        cell_shape.setOutlineThickness(1);

        cursor_shape = RectangleShape({1.0f, 1.0f});
        cursor_shape.setFillColor(Color::Transparent);
        cursor_shape.setOutlineThickness(2);
    }

    void SetDefaultColors()
    {
        cell_outline_color = DEFAULT_CELL_OUTLINE_COLOR;
        room_color = DEFAULT_ROOM_COLOR;
        wall_color = DEFAULT_WALL_COLOR;
        cursor_color = DEFAULR_CURSOR_COLOR;
    }

    void PlaceCell(int type, const Vector2i &where)
    {
        if(type == PLACE_ROOM) graph[where.y][where.x] = Cell{CELL_ROOM, &room_color};
        else if(type == PLACE_WALL) graph[where.y][where.x] = Cell{CELL_WALL, &wall_color};
    }

public:
    std::vector<std::vector<Cell>> graph;

    Vector2i size;
    Vector2f length;
    Vector2f position;
    Vector2f cell_size;
    Vector2f offset;
    Vector2f max_length;

    Color cell_outline_color;
    Color room_color;
    Color wall_color;
    Color cursor_color;

    bool show_sidebar;
    bool show_settings;

    int int_input2[2] = {};

    Point cursor;

    Grid() {}

    Grid(const Vector2i &size, const Vector2f &max_length)
    {
        Init(size, max_length);
        SetDefaultColors();
        Fill(PLACE_ROOM);
    }

    Grid(const std::vector<std::vector<Cell>> &graph, const Vector2f &max_length)
    {
        Vector2i new_size = Vector2i(graph[0].size(), graph.size());

        Init(new_size, max_length);
        SetDefaultColors();

        for(int i = 0; i < size.y; i++)
            for(int j = 0; j < size.x; j++)
                if(graph[i][j].type == CELL_WALL) 
                    PlaceCell(CELL_WALL, Vector2i(j, i));
    }

    void SidebarWindow()
    {
        if(!show_sidebar) return;
        offset.x = -SIDEBAR_WIDTH;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;

        ImGui::SetNextWindowPos(ImVec2(window_width - SIDEBAR_WIDTH, MENU_BAR_HEIGHT), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(SIDEBAR_WIDTH, window_height), ImGuiCond_Always);

        ImGui::Begin("Grid", nullptr, flags);

        ImGui::End();
    }

    void SettingsWindow()
    {
        if(!show_settings) return;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;
                                
        ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Always);
        ImGui::Begin("Grid Settings", nullptr, flags);

        if(ImGui::IsKeyDown(ImGuiKey_Escape))
            show_settings = false;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Grid"))
        {
            ImGui::Text("Size"); ImGui::SameLine(); 
            ImGui::InputInt2("##Size", int_input2);
            ImGui::SameLine();
            if(ImGui::Button("Apply"))
            {
                Init(Vector2i(int_input2[0], int_input2[1]), max_length);
                Fill(PLACE_ROOM);
            }
        }
        
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Colors"))
        {
            ImVec4 imgui_color;

            imgui_color = SFMLToImColor(room_color);
            if(ImGui::ColorEdit4("Room Color", (float*)&imgui_color))
                room_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(wall_color);
            if(ImGui::ColorEdit4("Wall Color", (float*)&imgui_color))
                wall_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(cell_outline_color);
            if(ImGui::ColorEdit4("Outline Color", (float*)&imgui_color))
                cell_outline_color = ImColorToSFML(imgui_color);
                
            imgui_color = SFMLToImColor(cursor_color);
            if(ImGui::ColorEdit4("Cursor Color", (float*)&imgui_color))
                cursor_color = ImColorToSFML(imgui_color);

            if(ImGui::Button("Set Defaults"))
                SetDefaultColors();
        }

        ImGui::End();
    }

    void UserInterface()
    {
        SidebarWindow();
        SettingsWindow();
    }

    void Fill(int type)
    {
        for(int i = 0; i < size.y; i++)
            for(int j = 0; j < size.x; j++)
                PlaceCell(type, Vector2i(j, i));
    }

    void Reset()
    {
        for(int i = 0; i < size.y; i++)
            for(int j = 0; j < size.x; j++)
                if(graph[i][j].type != CELL_WALL) PlaceCell(PLACE_ROOM, Vector2i(j, i));
                else graph[i][j].color = &wall_color;
    }

    void CheckGridCursor(const Vector2f &mouse)
    {
        for(int i = 0; i < size.y; i++)
        {
            for (int j = 0; j < size.x; j++)
            {
                float left = position.x + j * cell_size.x;
                float top = position.y + i * cell_size.y;
                float right = left + cell_size.x;
                float bottom = top + cell_size.y;

                cursor.valid = CheckPointOverlapBox(mouse, Vector2f(left, top), Vector2f(right, bottom));

                if(cursor.valid) 
                {
                    cursor.position = Vector2i(j, i);
                    return;
                }
            }
        }
    }

    void Update()
    {   
        if(IsKeyboardButtonDown(KEY_SHOW_STTINGS))
            show_settings = !show_settings;
        
        if(show_settings) return;
        
        if(IsKeyboardButtonDown(KEY_HIDE_SIDEBAR))
            show_sidebar = !show_sidebar;

        CheckGridCursor(canvas_mouse_position);

        if(IsKeyboardButtonDown(KEY_CLEAR_GRID)) Fill(PLACE_ROOM);

        if(IsKeyboardButtonDown(KEY_FILL_GRID)) Fill(PLACE_WALL);

        if(IsKeyboardButtonDown(KEY_RESET))
            Reset();
        
        if(IsMouseButtonPressed(BUTTON_PLACE) && cursor.valid) PlaceCell(CELL_WALL, cursor.position);
        else if(IsMouseButtonPressed(BUTTON_REMOVE) && cursor.valid) PlaceCell(CELL_ROOM, cursor.position);
    }

    void Draw()
    {
        cell_size = Vector2f(max_length.x / size.x, max_length.y / size.y);
        length = Vector2f(size.x * cell_size.x, size.y * cell_size.y);
        position = Vector2f((window_width + offset.x) / 2 - length.x / 2, (window_height + offset.y) / 2 - length.y / 2);

        cell_shape.setSize(cell_size);

        for(int i = 0; i < size.y; i++)
        {
            for(int j = 0; j < size.x; j++)
            {
                const auto &cell = graph[i][j];
                cell_shape.setFillColor(*cell.color);
                cell_shape.setOutlineColor(cell_outline_color);
                cell_shape.setPosition(position + Vector2f(j * cell_size.x, i * cell_size.y));

                window.draw(cell_shape);
            }
        }

        if(cursor.valid)
        {
            if(cursor.position.x < 0 || cursor.position.x > size.x || cursor.position.y < 0 || cursor.position.y > size.y) return;
            
            cursor_shape.setOutlineColor(cursor_color);
            cursor_shape.setSize(Vector2f(cell_size.x - 2, cell_size.y - 2));
            cursor_shape.setPosition(Vector2f(cursor.position.x * cell_size.x, cursor.position.y * cell_size.y) + position);

            window.draw(cursor_shape);
            
            cursor.valid = false;
        }
    }
};