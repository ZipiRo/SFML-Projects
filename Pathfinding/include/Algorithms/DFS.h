struct DepthFirstSearch : public PathfindAlgorithm
{
    vector2<bool> visited;
    vector2<Vector2i> parent;
    std::stack<Vector2i> stack;

    DepthFirstSearch()
    {
        name = "Depth First Search";
        description = "Depth-First Search (DFS) is a graph traversal algorithm that explores as far as possible along one branch before backtracking.";
        finished = false;
        path_found = false;
    }

    void Init(Vector2i start, Vector2i finish, int size) override
    {
        grid_size = size;
        start_point = start;
        finish_point = finish; 

        visited.resize(grid_size);
        for(auto &v : visited)
            v.resize(grid_size);

        parent.resize(grid_size);
        for(auto &v : parent)
            v.resize(grid_size);

        stack.push(start_point);
        visited[start.x][start.y] = true;
    }

    void ConstructPath()
    {
        Vector2i current = finish_point;

        while (!(current == start_point))
        {
            current = parent[current.y][current.x];
            if(current == start_point) break;
            path.push_back(current);
        }

        std::reverse(path.begin(), path.end());
    }

    void StepAlgorithm(vector2<Cell> &grid) override
    {
        if(stack.empty())
        {
            finished = true;
            return;
        }

        Vector2i current = stack.top();
        stack.pop();
        
        grid[current.y][current.x].type = CELL_VISITED;

        if(current == finish_point)
        {
            ConstructPath();
            path_found = true;
            finished = true;
            return;
        }
        
        bool moved = false;
        for(const auto &direction : directions)
        {
            Vector2i next = current + direction;

            if(next.x < 0 || next.y < 0 || next.x >= grid_size || next.y >= grid_size) continue;

            if(grid[next.y][next.x].type != CELL_WALL && !visited[next.y][next.x])
            {
                grid[next.y][next.x].type = CELL_FRONTIER;
    
                parent[next.y][next.x] = current;
    
                visited[next.y][next.x] = true;
                
                stack.push(current);
                stack.push(next);
                
                moved = true;
                break;
            }
        }

        if(!moved)
            grid[current.y][current.x].type = CELL_BACKTRACK;
    }
};