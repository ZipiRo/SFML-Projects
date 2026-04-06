#include <stack>
#include <queue>
#include <algorithm>
#include <map>

static std::vector<Vector2i> directions = { Vector2i(-1, 0), Vector2i(0, 1), Vector2i(1, 0), Vector2i(0, -1) };

template <typename T>
std::vector<T> Shuffle(std::vector<T> v) 
{
    for (int i = v.size() - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        std::swap(v[i], v[j]);
    }

    return v;
}

struct PathfindAlgorithm
{
    std::string name;
    std::string description;
    bool finished;
    bool path_found;
    bool done;
    int path_index;
    int grid_size;
    Vector2i start_point, finish_point;
    std::vector<Vector2i> path;

    PathfindAlgorithm()
    {
        name = "None";
        description = "None";
        done = false;
        path_index = 0;
    }

    virtual void Init(Vector2i start, Vector2i finish, int size) = 0;
    virtual void StepAlgorithm(vector2<Cell> &grid) = 0;
    
    void StepPath(vector2<Cell> &grid)
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

#include "Algorithms/BFS.h"
#include "Algorithms/BD_BFS.h"
#include "Algorithms/DFS.h"
#include "Algorithms/RD_DFS.h"

struct AlgorithmEntry 
{
    std::string name;
    std::function<PathfindAlgorithm*()> create;
};

std::vector<AlgorithmEntry> algorithms;
std::map<std::string, int> algo;

void RegisterAlgorithms()
{
    int algo_count = 0;
    algorithms.push_back({"DFS", [](){ return new DepthFirstSearch(); }});
    algo["DFS"] = algo_count++;
    algorithms.push_back({"BFS", [](){ return new BreadthFirstSearch(); }});
    algo["BFS"] = algo_count++;
    algorithms.push_back({"RD_DFS", [](){ return new RandomDirectionDFS(); }});
    algo["RD_DFS"] = algo_count++;
    algorithms.push_back({"BD_BFS", [](){ return new BiDirectionalBFS(); }});
    algo["BD_BFS"] = algo_count++;
}