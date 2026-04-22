enum CellType
{
    CELL_ROOM,
    CELL_WALL,
    CELL_NONE
};

struct Cell
{
    CellType type;
    Vector2i position;
    Color color;
    bool dirty = false;

    Cell() {}

    Cell(CellType type, Vector2i position, Color color = Color::Magenta)
        : type(type), position(position), color(color) 
        {
            dirty = true;
        }
};

class Grid
{
private:
    std::vector<std::vector<Cell>> grid;
    Vector2i size;

public: 

    void Create(int cols, int rows)
    {
        size = Vector2i(cols, rows);
        grid.assign(rows, std::vector<Cell>(cols));
        Fill(CELL_ROOM);
    }

    bool InBounds(int x, int y) const
    {
        return x >= 0 && y >= 0 && x < size.x && y < size.y;
    }

    const Cell &Get(int x, int y) const
    {
        return grid[y][x];
    }

    Vector2i GetSize() const
    {
        return size;
    }

    void SetCell(int x, int y, CellType type, Color color = Color::Magenta)
    {
        if(!InBounds(x, y)) return;
        grid[y][x] = Cell(type, Vector2i(x, y), color);
    }

    void Fill(CellType type)
    {
        for (int i = 0; i < size.y; i++)
            for (int j = 0; j < size.x; j++)
                SetCell(j, i, type);
    }

    void ClearColors()
    {
        for (int y = 0; y < size.y; y++)
            for (int x = 0; x < size.x; x++)
            {
                Cell &cell = grid[y][x];
                if(cell.type == CELL_NONE) cell = Cell(CELL_ROOM, Vector2i(x, y)); 
                else
                {
                    if(cell.type == CELL_ROOM) cell = Cell(CELL_ROOM, Vector2i(x, y));
                    else if(cell.type == CELL_WALL) cell = Cell(CELL_WALL, Vector2i(x, y));
                }
            }
    }
};
