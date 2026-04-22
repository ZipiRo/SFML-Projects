class RandomDFS : public PathAlgorithm
{
private:
    std::vector<std::vector<bool>> visited;
    std::vector<std::vector<Vector2i>> parent;
    std::stack<Vector2i> stack;
    Vector2i start, end;

public:
    static std::string abbr;
    static std::string name;
    static std::string desc;

    RandomDFS() {}

    void Init(const Grid &grid, Vector2i start, Vector2i end) override
    {
        this->start = start;
        this->end = end;
        
        done = false;
        path_found = false;

        visited = std::vector<std::vector<bool>>(grid.GetSize().y, std::vector<bool>(grid.GetSize().x));
        parent = std::vector<std::vector<Vector2i>>(grid.GetSize().y, std::vector<Vector2i>(grid.GetSize().x));

        stack.push(start);
        visited[start.y][start.x] = true;
    }

    void Step(Grid &grid) override
    {
        if(stack.empty())
        {
            done = true;
            return;
        }

        Vector2i current = stack.top();
        stack.pop();

        grid.SetCell(current.x, current.y, CELL_NONE, explored_color);

        if(current == end)
        {
            path_found = true;
            return;
        }

        std::vector<Vector2i> random_directions = VectorShuffle(directions);

        bool moved = false;
        for(const auto &direction : random_directions)
        {
            Vector2i next = current + direction;

            if(next.x < 0 || next.y < 0 || next.x >= grid.GetSize().x || next.y >= grid.GetSize().y) continue;

            if(grid.Get(next.x, next.y).type != CELL_WALL && !visited[next.y][next.x])
            {
                grid.SetCell(next.x, next.y, CELL_NONE, frontier_color);
                parent[next.y][next.x] = current;

                visited[next.y][next.x] = true;

                stack.push(current);
                stack.push(next);

                moved = true;
                break;
            }
        }
        
        if(!moved)
            grid.SetCell(current.x, current.y, CELL_NONE, backtrack_color);
    }

    std::vector<Vector2i> ConstructPath() override
    {
        std::vector<Vector2i> path;
        Vector2i current = end;

        while (current != start)
        {
            current = parent[current.y][current.x];
            if(current == start) break;
            path.push_back(current);
        }
        
        std::reverse(path.begin(), path.end());
        return path;
    }
};

std::string RandomDFS::abbr = "RDFS";
std::string RandomDFS::name = "Random Depth-First Search";
std::string RandomDFS::desc = "Random Direction DFS (RDFS) is a variation of Depth-First Search where, instead of visiting neighbors in a fixed order, the algorithm chooses a random order each time.";
 