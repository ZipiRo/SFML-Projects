class DFS : public Pathfinder::Algorithm
{
private:
    std::vector<std::vector<bool>> visited;
    std::vector<std::vector<Vector2i>> parent;
    std::stack<Vector2i> stack;
    Vector2i start, end;

public:
    static std::string abbr;
    static std::string name;
    static std::string description;

    DFS() {}

    void Init(const Grid &grid, const Vector2i &start, const Vector2i &end) override
    {
        this->start = start;
        this->end = end;
        
        done = false;
        path_found = false;

        visited = std::vector<std::vector<bool>>(grid.size.y, std::vector<bool>(grid.size.x));
        parent = std::vector<std::vector<Vector2i>>(grid.size.y, std::vector<Vector2i>(grid.size.x));

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

        grid.graph[current.y][current.x].color = &*visited_color;

        if(current == end)
        {
            path_found = true;
            return;
        }

        bool moved = false;
        for(const auto &direction : directions)
        {
            Vector2i next = current + direction;

            if(next.x < 0 || next.y < 0 || next.x >= grid.size.x || next.y >= grid.size.y) continue;

            if(grid.graph[next.y][next.x].type != CELL_WALL && !visited[next.y][next.x])
            {
                grid.graph[next.y][next.x].color = &*frontier_color;
                parent[next.y][next.x] = current;

                visited[next.y][next.x] = true;

                stack.push(current);
                stack.push(next);

                moved = true;
                break;
            }
        }
        
        if(!moved)
            grid.graph[current.y][current.x].color = &*backtrack_color;
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

std::string DFS::abbr = "DFS";
std::string DFS::name = "Depth-First Search";
std::string DFS::description = "Depth-First Search (DFS) is a graph traversal algorithm that explores as far as possible along one branch before backtracking.";