class GridRenderer
{
private:
    VertexArray grid_shape;
    VertexArray grid_lines;

    Vector2i size;
    Vector2f cell_size;
    Vector2f position;
    Vector2f length;
    Vector2f offset;

    GridTheme current_theme;
    int using_theme;

    bool refresh_shape_colors = false;
    bool refresh_lines_colors = false;

    void SetCellColor(Vector2i position, Color color)
    {
        int index = (position.y * size.x + position.x) * 6;

        for(int i = 0; i < 6; i++)
            grid_shape[index + i].color = color;
    }

    void SetLinesColor(Color color)
    {
        for(int i = 0; i < grid_lines.getVertexCount(); i++)
            grid_lines[i].color = color;
    }

public:
    const std::vector<GridTheme> color_themes = GetGridColorThemes();
    
    GridRenderer()
    {
        current_theme = color_themes[0];

        refresh_shape_colors = false;
        refresh_lines_colors = false;
    }

    void SetPosition(Vector2f position)
    {
        this->position = position;
    }

    Vector2f GetOffsetPosition() const
    {
        return position + offset;
    }

    Vector2f GetPosition() const
    {
        return position;
    }

    Vector2f GetLength() const
    {
        return length;
    }

    Vector2f GetCellSize() const
    {
        return cell_size;
    }

    void SetOffset(Vector2f offset)
    {
        this->offset = Vector2f(offset.x / 2, offset.y / 2); 
    }
 
    void SetColorTheme(int index)
    {
        if(index < 0 || index >= color_themes.size()) return;
        
        current_theme = color_themes[index];
        refresh_shape_colors = true;
        refresh_lines_colors = true;
    }

    GridTheme GetColorTheme()
    {
        return current_theme;
    }

    Point GetCellPoint(Vector2f from) const
    {
        if(!(from.x >= GetOffsetPosition().x && from.y >= GetOffsetPosition().y &&
            from.x < GetOffsetPosition().x + length.x && from.y < GetOffsetPosition().y + length.y))
                return Point{.valid = false};

        Point point;
        point.position = Vector2i((from.x - GetOffsetPosition().x) / cell_size.x,
                                    (from.y - GetOffsetPosition().y) / cell_size.y);
        point.valid = true;

        return point;
    }

    void Build(Vector2f cell_size, Vector2i size)
    {
        this->cell_size = cell_size;
        this->size = size;

        length = Vector2f(cell_size.x * size.x, cell_size.y * size.y);

        grid_shape = VertexArray(PrimitiveType::Triangles, size.x * size.y * 6);

        int index = 0;

        for(int y = 0; y < size.y; y++)
        {
            for(int x = 0; x < size.x; x++)
            {
                Vector2f cell_pos(Vector2f(x * cell_size.x, y * cell_size.y));

                Vertex* quad = &grid_shape[index * 6];

                quad[0].position = Vector2f(cell_pos);
                quad[1].position = Vector2f(cell_pos.x + cell_size.x, cell_pos.y);
                quad[2].position = Vector2f(cell_pos.x + cell_size.x, cell_pos.y + cell_size.y);

                quad[3].position = Vector2f(cell_pos);
                quad[4].position = Vector2f(cell_pos.x + cell_size.x, cell_pos.y + cell_size.y);
                quad[5].position = Vector2f(cell_pos.x, cell_pos.y + cell_size.y);

                quad[0].color = current_theme.colors[RoomColor];
                quad[1].color = current_theme.colors[RoomColor];
                quad[2].color = current_theme.colors[RoomColor];
                quad[3].color = current_theme.colors[RoomColor];
                quad[4].color = current_theme.colors[RoomColor];
                quad[5].color = current_theme.colors[RoomColor];

                index++;
            }
        }
        
        grid_lines = VertexArray(PrimitiveType::Lines, (size.x + size.y + 2) * 2);

        if(cell_size.x < 2.0f || cell_size.y < 2.0f) return;
        
        index = 0;

        for(int i = 0; i <= size.x; i++)
        {
            float x = i * cell_size.x;
            
            Vertex* line = &grid_lines[index * 2];

            line[0].position = Vector2f(x, 0);
            line[1].position = Vector2f(x, cell_size.y * size.y);

            line[0].color = current_theme.colors[LinesColor];
            line[1].color = current_theme.colors[LinesColor];
            
            index++;
        }
        

        for(int i = 0; i <= size.y; i++)
        {
            float y = i * cell_size.y;
            
            Vertex* line = &grid_lines[index * 2];

            line[0].position = Vector2f(0, y);
            line[1].position = Vector2f(cell_size.x * size.x, y);

            line[0].color = current_theme.colors[LinesColor];
            line[1].color = current_theme.colors[LinesColor];

            index++;
        }
    }

    void Update(Grid &grid)
    {
        for(int y = 0; y < grid.GetSize().y; y++)
        {
            for(int x = 0; x < grid.GetSize().x; x++)
            {
                if(!grid.Get(x, y).dirty && !refresh_shape_colors) continue;
                Cell cell = grid.Get(x, y);
            
                if(cell.type == CELL_ROOM)
                {
                    if(cell.color != Color::Magenta) SetCellColor(cell.position, cell.color);
                    else SetCellColor(cell.position, current_theme.colors[RoomColor]);
                }
                else if(cell.type == CELL_WALL)
                {
                    if(cell.color != Color::Magenta) SetCellColor(cell.position, cell.color);
                    else SetCellColor(cell.position, current_theme.colors[WallColor]);
                }
                else SetCellColor(cell.position, cell.color);

                cell.dirty = false;
            }
        }

        if(refresh_shape_colors)
            refresh_shape_colors = false;

        if(refresh_lines_colors)
        {
            SetLinesColor(current_theme.colors[LinesColor]);
            refresh_lines_colors = false;
        }
    }

    void Draw(RenderWindow &window)
    {
        RenderStates states;
        states.transform.translate(position + offset);

        window.draw(grid_shape, states);

        if(cell_size.x < 2.0f || cell_size.y < 2.0f) return;
        window.draw(grid_lines, states);
    }
};