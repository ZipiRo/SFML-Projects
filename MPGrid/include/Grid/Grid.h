const std::string GridSaveFileMark = "@)#GRD";
const std::string GridSaveFileExt = ".grd";
const std::string GridSaveFileDir = "GridSaves"; 

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
    Grid()
    { 
        if(!std::filesystem::exists(GridSaveFileDir))
            std::filesystem::create_directory(GridSaveFileDir);
    }

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

    void Save(std::string file_name);
    bool Load(std::string file_name);
};

void Grid::Save(std::string file_name)
{
    std::filesystem::path file = std::filesystem::path(GridSaveFileDir) / file_name;
    std::string extension = file.extension().string();

    if(file.extension().empty() || extension == ".")
        file += GridSaveFileExt;

    std::ofstream FILE(file.string());

    if(!FILE) return;

    FILE << GridSaveFileMark;
    FILE << '\n';
    FILE << std::to_string(size.x);
    FILE << '\n';
    FILE << std::to_string(size.y);
    FILE << '\n';
    for(int i = 0; i < size.y; i++)
    {
        for(int j = 0; j < size.x; j++)
            FILE << grid[i][j].type;
        FILE << '\n';
    }

    FILE.close();
}

bool Grid::Load(std::string file_name)
{
    std::filesystem::path file = std::filesystem::path(GridSaveFileDir) / file_name;
    std::string extension = file.extension().string();

    if(file.extension().empty() || extension == ".")
        file += GridSaveFileExt;
 
    std::ifstream FILE(file.string());

    char mark[10]; 
    FILE.getline(mark, 10);
    if(std::string(mark).find(GridSaveFileMark) == std::string::npos)
        return false;

    Vector2i size;
    FILE.getline(mark, 10);
    if(std::string(mark).size() > 0) size.x = atoi(mark);
    else return false; 

    FILE.getline(mark, 10);    
    if(std::string(mark).size() > 0) size.y = atoi(mark);
    else return false; 

    Create(size.x, size.y);

    char buffer[1000];
    for(int y = 0; y < size.y; y++)
    {
        FILE.getline(buffer, 1000);
        for(int x = 0; x < size.x; x++)
        {
            CellType type = CellType(buffer[x] - '0');
            grid[y][x].type = type;
        }
    }

    FILE.close();

    return true;
}
