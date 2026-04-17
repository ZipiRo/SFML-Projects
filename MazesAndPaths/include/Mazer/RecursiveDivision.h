class RD : public Mazer::Algorithm
{
private:
    struct Region
    {
        Vector2i position;
        Vector2i size;
    };

    std::stack<Region> stack;
    Vector2i start;

public:
    static std::string abbr;
    static std::string name;
    static std::string description;

    RD() {}

    void Init(const Grid& grid, Vector2i &start) override
    {
        this->start = start;
        done = false;

        stack.push({ start, grid.size });
    }

    void Step(Grid& grid) override
    {
        if (stack.empty())
        {
            done = true;
            return;
        }

        Region current = stack.top();
        stack.pop();

        if (current.size.x < 3 || current.size.y < 3)
            return;

        int x = current.position.x;
        int y = current.position.y;
        int width = current.size.x;
        int height = current.size.y;

        x = std::max(0, x);
        y = std::max(0, y);

        width  = std::min(width,  grid.size.x - x);
        height = std::min(height, grid.size.y - y);

        bool horizontal = (width < height);

        if (horizontal)
        {
            if (height < 3) return;

            int wall_y = y + 1 + 2 * (rand() % ((height - 1) / 2));
            int door_x = x + (2 * (rand() % ((width + 1) / 2)));

            for (int i = x; i < x + width; i++)
                grid.graph[wall_y][i] = Cell(*primary_color, CELL_WALL);

            grid.graph[wall_y][door_x] = Cell(*secondary_color, CELL_ROOM);

            Region top = { Vector2i(x, y), Vector2i(width, wall_y - y) };
            Region bottom = { Vector2i(x, wall_y + 1), Vector2i(width, y + height - wall_y - 1) };

            if (top.size.x >= 3 && top.size.y >= 3) stack.push(top);
            if (bottom.size.x >= 3 && bottom.size.y >= 3) stack.push(bottom);
        }
        else
        {
            if (width < 3) return;

            int wall_x = x + 1 + 2 * (rand() % ((width - 1) / 2));
            int door_y = y + (2 * (rand() % ((height + 1) / 2)));

            for (int i = y; i < y + height; i++)
                grid.graph[i][wall_x] = Cell(*primary_color, CELL_WALL);

            grid.graph[door_y][wall_x] = Cell(*secondary_color, CELL_ROOM);

            Region left = { Vector2i(x, y), Vector2i(wall_x - x, height) };
            Region right = { Vector2i(wall_x + 1, y), Vector2i(x + width - wall_x - 1, height) };

            if (left.size.x >= 3 && left.size.y >= 3) stack.push(left);
            if (right.size.x >= 3 && right.size.y >= 3) stack.push(right);
        }
    }
};

std::string RD::abbr = "RD";
std::string RD::name = "Recursive Division";
std::string RD::description = "Recursive Division (RD) is a maze generation algorithm that repeatedly splits an area with walls, places a single passage through each wall, and recursively applies the same process to the resulting sub-sections until the maze is fully partitioned.";