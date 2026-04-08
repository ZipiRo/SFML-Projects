Vector2i CheckMouseHoverOnCell(const Vector2f &mouse_position)
{
    hovered_cell = Vector2i(-1, -1);
    for(int i = 0; i < grid_size; i++)
    {
        for (int j = 0; j < grid_size; j++)
        {
            int left = grid_position.x + j * cell_size;
            int top = grid_position.y + i * cell_size;
            int right = left + cell_size;
            int bottom = top + cell_size;
            
            if(CheckPointOverlapBox(mouse_position, Vector2f(left, top), Vector2f(right, bottom)))
                hovered_cell = Vector2i(j, i);
        }
    }

    return hovered_cell;
}

void PlaceCell(int place, const Vector2i &hovered_cell, vector2<Cell> &grid)
{
    if(hovered_cell == Vector2i(-1, -1)) return;

    int type;
    
    if(place == PLACE_WALL) type = CELL_WALL;
    else if(place == PLACE_START) type = CELL_START;
    else if(place == PLACE_FINISH) type = CELL_FINISH;

    if((type == CELL_START && start_placed) || 
        (type == CELL_FINISH && finish_placed)) return;

    Cell &cell = grid[hovered_cell.y][hovered_cell.x];

    if(cell.type != CELL_ROOM) return;

    if(type == CELL_START) 
    {
        start_placed = true;
        start_point = hovered_cell;
    }
    else if(type == CELL_FINISH) 
    {
        finish_placed = true;
        finish_point = hovered_cell;
    }

    cell.type = type;

    sound_player.setBuffer(soundbuffers["place"]);
    sound_player.setPitch(1);
    sound_player.play();
}

void ClearCell(const Vector2i &hovered_cell, vector2<Cell> &grid)
{
    if(hovered_cell == Vector2i(-1, -1)) return;

    Cell &cell = grid[hovered_cell.y][hovered_cell.x];

    if(cell.type == CELL_ROOM) return;

    if(cell.type == CELL_START && start_placed) 
    {
        start_point = Vector2i(-1, -1);
        start_placed = false;
    }
    else if(cell.type == CELL_FINISH && finish_placed) 
    {
        finish_point = Vector2i(-1, -1);
        finish_placed = false;
    }

    cell.type = CELL_ROOM;

    sound_player.setBuffer(soundbuffers["remove"]);
    sound_player.setPitch(1);
    sound_player.play();
}

void InitGrid(int size, vector2<Cell> &grid)
{
    grid_size = size;
    cell_size = MAX_GRID_LENGTH / float(grid_size);
    grid_length = grid_size * cell_size;

    grid.resize(grid_size);
    for(auto &row : grid)
        row.resize(grid_size);

    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            grid[i][j].type = CELL_ROOM;

    start_placed = finish_placed = false;
    start_point = finish_point = Vector2i(-1, -1);
    sim_state = SIM_STATE_SETUP;
    algo_state = ALGO_STATE_WAITING;
    placeing = PLACE_WALL;
}

void ClearGrid(vector2<Cell> &grid)
{
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            grid[i][j].type = CELL_ROOM;

    start_placed = finish_placed = false;
    start_point = finish_point = Vector2i(-1, -1);

    sound_player.setBuffer(soundbuffers["remove"]);
    sound_player.setPitch(1);
    sound_player.play();
}

void ResetGrid(vector2<Cell> &grid)
{
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
        {
            int &type = grid[i][j].type; 
            if(!(type == CELL_WALL || type == CELL_START || type == CELL_FINISH))
                type = CELL_ROOM;
        }
}

void SaveGrid(vector2<Cell> &grid, const std::string file_name)
{
    std::string file_path = file_name;
    file_path += ".grid";

    std::ofstream FILE(file_path);

    FILE << "GRID123";
    FILE << '\n';
    FILE << std::to_string(grid_size);
    FILE << '\n';

    for(int i = 0; i < grid_size; i++)
    {
        for(int j = 0; j < grid_size; j++)
        {
            FILE << grid[i][j].type;
        }
        FILE << '\n';
    }

    FILE.close();
}

bool LoadGrid(vector2<Cell> &grid, const char* file_path)
{
    std::ifstream FILE(file_path);

    char mark[10]; 
    FILE.getline(mark, 10);
    if(strcmp(mark, "GRID123") != 0)
        return false; 

    int size;
    FILE.getline(mark, 10);

    if(strlen(mark) > 0) size = atoi(mark);
        else return false; 

    InitGrid(size, grid);

    char buffer[100];
    for(int i = 0; i < size; i++)
    {
        FILE.getline(buffer, 100);
        for(int j = 0; j < size; j++)
        {
            int cell_type = buffer[j] - '0';

            if(cell_type == CELL_NONE || cell_type == CELL_TRACE)
                continue;

            if(cell_type == CELL_START)
            {
                start_placed = true;
                start_point = Vector2i(j, i);
            }
            else if(cell_type == CELL_FINISH)
            {
                finish_placed = true;
                finish_point = Vector2i(j, i);
            }

            grid[i][j].type = cell_type;
        }
    }

    grid_size = size;

    FILE.close();

    return true;
}

void DrawGrid(const vector2<Cell> &grid)
{
    RectangleShape rectangle(Vector2f(cell_size, cell_size));
    rectangle.setOutlineColor(GRID_OUTLINE_COLOR);
    rectangle.setOutlineThickness(1);

    grid_position = Vector2f(
        (window_width + grid_offset.x) * 0.5 - grid_length * 0.5,
        (window_height + grid_offset.y) * 0.5 - grid_length * 0.5
    );

    for(int i = 0; i < grid_size; i++)
    {
        for(int j = 0; j < grid_size; j++)
        {
            Vector2f position = Vector2f(i * cell_size, j * cell_size) + grid_position;
            Cell cell = grid[j][i];

            rectangle.setFillColor(cell.color);
            if(cell.type == CELL_ROOM || cell.type == CELL_FINISH || 
                cell.type == CELL_WALL || cell.type == CELL_START || cell.type == CELL_TRACE) 
                    rectangle.setFillColor(cell_colors[cell.type]);

            rectangle.setPosition(position);
            window.draw(rectangle);
        }
    }
}

void DrawCellCursor(const Vector2i &hovered_cell)
{
    if(hovered_cell == Vector2i(-1, -1)) return;

    RectangleShape rectangle(Vector2f(cell_size - 2, cell_size - 2));
    rectangle.setFillColor(Color::Transparent);
    rectangle.setOutlineColor(HOVER_CURSOR_COLOR);
    rectangle.setOutlineThickness(2);
    rectangle.setPosition(Vector2f(hovered_cell.x * cell_size, hovered_cell.y * cell_size) + grid_position);

    window.draw(rectangle);
}