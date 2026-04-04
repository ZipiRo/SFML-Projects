#include <stack>
#include <queue>
#include <algorithm>

static std::vector<Vector2i> directions = { Vector2i(-1, 0), Vector2i(0, 1), Vector2i(1, 0), Vector2i(0, -1) };

enum ALGORITHMS
{
    ALG_DFS,
    ALG_BFS
};

static std::string algorithms_strings[10] = {"DFS", "BFS"};

struct PathfindAlgorithm
{
    std::string name;
    bool finished;
    bool path_found;
    bool done;
    int path_index;
    Vector2i start_point, finish_point;
    std::vector<Vector2i> path;

    PathfindAlgorithm()
    {
        done = false;
        path_index = 0;
    }

    virtual void Init(Vector2i start, Vector2i finish) = 0;
    virtual void StepAlgorithm(Cell grid[GRID_SIZE][GRID_SIZE]) = 0;
    
    void StepPath(Cell grid[GRID_SIZE][GRID_SIZE])
    {
        if(path_index >= path.size()) 
        {
            done = true;
            return;
        }

        Vector2i point = path[path_index++];
        grid[point.y][point.x].type = CELL_PATH;
    }
};

struct DepthFirstSearch : public PathfindAlgorithm
{
    bool visited[GRID_SIZE][GRID_SIZE] = {};
    Vector2i parent[GRID_SIZE][GRID_SIZE];
    std::stack<Vector2i> stack;

    DepthFirstSearch()
    {
        name = "Depth First Search";
        finished = false;
        path_found = false;
    }

    void Init(Vector2i start, Vector2i finish) override
    {
        start_point = start;
        finish_point = finish;

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

    void StepAlgorithm(Cell grid[GRID_SIZE][GRID_SIZE]) override
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

            if(next.x < 0 || next.y < 0 || next.x >= GRID_SIZE || next.y >= GRID_SIZE) continue;

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

struct BreadthFirstSearch : public PathfindAlgorithm
{
    bool visited[GRID_SIZE][GRID_SIZE] = {};
    Vector2i parent[GRID_SIZE][GRID_SIZE];
    std::queue<Vector2i> queue;

    BreadthFirstSearch()
    {
        name = "Breadth First Search";
        finished = false;
        path_found = false;
    }

    void Init(Vector2i start, Vector2i finish) override
    {
        start_point = start;
        finish_point = finish;

        queue.push(start_point);
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

    void StepAlgorithm(Cell grid[GRID_SIZE][GRID_SIZE]) override
    {
        if(queue.empty())
        {
            finished = true;
            return;
        }

        Vector2i current = queue.front();
        queue.pop();
        
        grid[current.y][current.x].type = CELL_VISITED;

        if(current == finish_point)
        {
            ConstructPath();
            path_found = true;
            finished = true;
            return;
        }
        
        for(const auto &direction : directions)
        {
            Vector2i next = current + direction;

            if(next.x < 0 || next.y < 0 || next.x >= GRID_SIZE || next.y >= GRID_SIZE) continue;

            if(grid[next.y][next.x].type != CELL_WALL && !visited[next.y][next.x])
            {
                visited[next.y][next.x] = true;
                parent[next.y][next.x] = current;
    
                queue.push(next);

                grid[next.y][next.x].type = CELL_FRONTIER;
            }
        }
    }
};

void SetAlgorithm(int alg, PathfindAlgorithm* &algorithm)
{
    delete algorithm;
    switch (alg)
    {
    case ALG_DFS:
        algorithm = new DepthFirstSearch();
        break;
    case ALG_BFS:
        algorithm = new BreadthFirstSearch();
    default:
        break;
    }
}