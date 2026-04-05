struct BiDirectionalBFS : public PathfindAlgorithm
{
    struct BFS
    {    
        vector2<bool> visited;
        vector2<Vector2i> parent;
        std::queue<Vector2i> queue;
    } bfs_start, bfs_finish;

    BiDirectionalBFS()
    {
        name = "Bi Directional BFS";
        description = "Bidirectional BFS is an optimized version of Breadth-First Search where you run two BFS searches at the same time: one from the start, one from the finish.";
        finished = false;
        path_found = false;
    }

    void Init(Vector2i start, Vector2i finish, int size) override
    {
        grid_size = size;
        start_point = start;
        finish_point = finish;

        InitBfs(start, bfs_start);
        InitBfs(finish, bfs_finish);
    }

    void ConstructPath()
    {
        Vector2i current;

        current = finish_point;
        while (!(current == start_point))
        {
            current = bfs_start.parent[current.y][current.x];
            if(current == start_point) break;
            path.push_back(current);
        }

        
        current = start_point;
        while (!(current == finish_point))
        {
            current = bfs_finish.parent[current.y][current.x];
            if(current == finish_point) break;
            path.push_back(current);
        }

        std::reverse(path.begin(), path.end());
    }

    void InitBfs(Vector2i point, BFS &bfs)
    {
        bfs.visited.resize(grid_size);
        for(auto &v : bfs.visited)
            v.resize(grid_size);

        bfs.parent.resize(grid_size);
        for(auto &v : bfs.parent)
            v.resize(grid_size);

        bfs.queue.push(point);
        bfs.visited[point.x][point.y] = true;
    }

    Vector2i StepBFS(Vector2i to_point, BFS &bfs, vector2<Cell> &grid)
    {
        if(bfs.queue.empty())
            return Vector2i(-1, -1);

        Vector2i current = bfs.queue.front();
        bfs.queue.pop();
        
        grid[current.y][current.x].type = CELL_VISITED;
        
        for(const auto &direction : directions)
        {
            Vector2i next = current + direction;

            if(next.x < 0 || next.y < 0 || next.x >= grid_size || next.y >= grid_size) continue;

            if(grid[next.y][next.x].type != CELL_WALL && !bfs.visited[next.y][next.x])
            {
                bfs.visited[next.y][next.x] = true;
                bfs.parent[next.y][next.x] = current;
    
                bfs.queue.push(next);

                grid[next.y][next.x].type = CELL_FRONTIER;
            }
        }

        return current;
    }

    void StepAlgorithm(vector2<Cell> &grid) override
    {
        if(bfs_start.queue.empty() && bfs_finish.queue.empty())
        {
            finished = true;
            return;
        }

        Vector2i current_start = StepBFS(finish_point, bfs_start, grid);
        Vector2i current_finish = StepBFS(start_point, bfs_finish, grid);

        if(current_start == current_finish)
        {
            ConstructPath();
            path_found = true;
            finished = true;
            return;
        }
    }
};
