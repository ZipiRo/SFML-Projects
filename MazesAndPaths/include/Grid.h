const Color DEFAULT_CELL_OUTLINE_COLOR = Color(20, 20, 20);
const Color DEFAULT_ROOM_COLOR = Color(40, 40, 40);
const Color DEFAULT_WALL_COLOR = DEFAULT_CELL_OUTLINE_COLOR;
const Color DEFAULR_CURSOR_COLOR = Color::Black;

enum CellType
{
    CELL_NONE,
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

    Cell()
    {
        type = CELL_NONE;
        color = nullptr;
    }

    Cell(int type)
    {
        this->type = type;
        this->color = nullptr;
    }

    Cell(Color &color, int type = CELL_NONE)
    {
        this->type = type;
        this->color = &color;
    }
};

class Grid
{
private:
    RectangleShape cell_shape;
    RectangleShape cursor_shape;

    Vector2f default_max_length;
    Vector2f length;
    Vector2f position;
    Vector2f cell_size;
    Vector2f max_length;

    bool show_sidebar;
    bool show_settings;
    bool square_grid;

    int size_input[2] = {};
    bool lock_size_input = false;

    void Init(const Vector2i &size, const Vector2f &default_max_length)
    {   
        SetDefaultColors();
        
        show_sidebar = true;
        show_settings = false;

        this->size = size;
        this->default_max_length = default_max_length;
        
        graph = std::vector<std::vector<Cell>>(size.y, std::vector<Cell>(size.x, Cell(CELL_ROOM)));

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

    void PlaceCell(PlaceType type, const Vector2i &where)
    {
        if(type == PLACE_ROOM) graph[where.y][where.x] = Cell(CELL_ROOM);
        else if(type == PLACE_WALL) graph[where.y][where.x] = Cell(CELL_WALL);
    }

    void InitSettingsWindow()
    {
        size_input[0] = size.x;
        size_input[1] = size.y;
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
        
        ImGui::BeginDisabled(show_settings);

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Actions"))
        {
            if(ImGui::Button("Reset (R)")) 
                Reset();

            ImGui::SameLine();
            if(ImGui::Button("Fill (F)")) 
                Fill(PLACE_WALL);
            
            ImGui::SameLine();
            if(ImGui::Button("Clear (C)")) 
                Fill(PLACE_ROOM);
            
            if(ImGui::Button("Settings (LShift)"))
            {
                show_settings = true;
                InitSettingsWindow();
            }
        }
        
        ImGui::EndDisabled();
        ImGui::End();
    }

    void SettingsWindow()
    {
        if(!show_settings) return;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse;
                                
        ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Always);
        ImGui::Begin("Grid Settings", &show_settings, flags);

        if(ImGui::IsKeyDown(ImGuiKey_Escape))
            show_settings = false;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if(ImGui::CollapsingHeader("Grid Size"))
        {   
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
            if(size_input[0] != size.x || size_input[1] != size.y)
            {
                changes = true;

                if(lock_size_input)
                    size_input[1] = size_input[0];
            }

            if((ImGui::Button("Apply (Enter)") || ImGui::IsKeyDown(ImGuiKey_Enter)) && changes)
            {
                Init(Vector2i(size_input[0], size_input[1]), default_max_length);
                Fill(PLACE_ROOM);
            }

            ImGui::Checkbox("Square Grid", &square_grid);
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

public:
    std::vector<std::vector<Cell>> graph;

    Vector2i size;
    Vector2f offset;

    Color cell_outline_color;
    Color room_color;
    Color wall_color;
    Color cursor_color;

    Point cursor;

    Grid() {}

    Grid(const Vector2i &size, const Vector2f &max_length)
    {
        square_grid = true;
        Init(size, max_length);
    }

    void SetGraph(const std::vector<std::vector<int>> &new_graph)
    {
        Vector2i new_size = Vector2i(new_graph[0].size(), new_graph.size());
        Init(new_size, default_max_length);

        for(int i = 0; i < size.y; i++)
            for(int j = 0; j < size.x; j++)
            {
                if(new_graph[i][j] == CELL_WALL) PlaceCell(PLACE_WALL, Vector2i(j, i));
                else PlaceCell(PLACE_ROOM, Vector2i(j, i));
            }    
    }

    void UserInterface()
    {
        SidebarWindow();
        SettingsWindow();
    }

    void Start()
    {
        show_sidebar = true;
        show_settings = false;
    }
    
    void Fill(PlaceType type)
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
        {
            show_settings = !show_settings;
            if(show_settings)
                InitSettingsWindow();
        }
        
        if(show_settings) return;
        
        if(IsKeyboardButtonDown(KEY_HIDE_SIDEBAR))
            show_sidebar = !show_sidebar;

        CheckGridCursor(canvas_mouse_position);

        if(IsKeyboardButtonDown(KEY_CLEAR_GRID)) 
            Fill(PLACE_ROOM);

        if(IsKeyboardButtonDown(KEY_FILL_GRID)) 
            Fill(PLACE_WALL);

        if(IsKeyboardButtonDown(KEY_RESET))
            Reset();
        
        if(IsMouseButtonPressed(BUTTON_PLACE) && cursor.valid) PlaceCell(PLACE_WALL, cursor.position);
        else if(IsMouseButtonPressed(BUTTON_REMOVE) && cursor.valid) PlaceCell(PLACE_ROOM, cursor.position);
    }

    void Draw()
    {
        if(square_grid) max_length = Vector2f(default_max_length.y, default_max_length.y);
        else max_length = Vector2f(default_max_length.x + offset.x, default_max_length.y);

        cell_size = Vector2f(max_length.x / size.x, max_length.y / size.y);
        length = Vector2f(size.x * cell_size.x, size.y * cell_size.y);
        position = Vector2f((window_width + offset.x) / 2 - length.x / 2, (window_height + offset.y) / 2 - length.y / 2);

        cell_shape.setSize(cell_size);

        for(int i = 0; i < size.y; i++)
        {
            for(int j = 0; j < size.x; j++)
            {
                const auto &cell = graph[i][j];

                Color fill = Color::Magenta;
                if(cell.color != nullptr) fill = *cell.color;
                else if(cell.type == CELL_ROOM) fill = room_color;
                else if(cell.type == CELL_WALL) fill = wall_color;

                cell_shape.setFillColor(fill);
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