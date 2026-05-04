class RB : public MazeAlgorithm
{
private:
    std::vector<std::vector<bool>> visited;
    std::stack<Vector2i> stack;
    Vector2i start;

    std::vector<Vector2i> GetNeighbors(const Grid &grid, const Vector2i &current)
    {
        std::vector<Vector2i> neighbors;
        for(const auto& direction : directions)
        {
            Vector2i neighbor = current + direction * 2;

            if(neighbor.x < 0 || neighbor.y < 0 || neighbor.x >= grid.GetSize().x || neighbor.y >= grid.GetSize().y) continue;
            if(grid.Get(neighbor.x, neighbor.y).type != CELL_WALL) continue;

            if(!visited[neighbor.y][neighbor.x])
                neighbors.push_back(neighbor);
        }

        return neighbors;
    }

public:
    static std::string abbr;
    static std::string name;
    static std::string desc;
    static std::string rules;

    RB() {}

    void Init(const Grid& grid, Vector2i start) override
    {
        this->start = start;
        done = false;

        visited = std::vector<std::vector<bool>>(grid.GetSize().y, std::vector<bool>(grid.GetSize().x));
        
        stack.push(start);
        visited[start.y][start.x] = true;
    }

    void Step(Grid& grid) override
    {
        if(stack.empty())
        {
            done = true;
            return;
        }

        Vector2i current = stack.top();

        std::vector<Vector2i> neighbors = GetNeighbors(grid, current);

        if(!neighbors.empty())
        {
            Vector2i next = neighbors[rand() % neighbors.size()];
            Vector2i direction = (next - current) / 2;

            Vector2i wall = current + direction;

            grid.SetCell(wall.x, wall.y, CELL_ROOM, primary_color);
            grid.SetCell(next.x, next.y, CELL_ROOM, primary_color);

            visited[next.y][next.x] = true;
            stack.push(next);
        }
        else
        {
            stack.pop();
            grid.SetCell(current.x, current.y, CELL_NONE, secondary_color);
        }
    }
};

std::string RB::abbr = "RB";
std::string RB::name = "Recursive Backtrack";
std::string RB::desc = "Recursive Backtracker (RB) is a depth-first maze generation algorithm that moves forward through random unvisited neighbors, carving paths, and backtracks when it hits a dead end until all cells are visited.";
std::string RB::rules = "1.Use a filled grid (all walls) for the algorithm to work";