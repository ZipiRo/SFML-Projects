struct PathAlgorithm
{
    bool done = false;
    bool path_found = false;

    Color explored_color;
    Color frontier_color;
    Color backtrack_color;

    virtual void Init(const Grid &grid, Vector2i start, Vector2i end) = 0;
    virtual void Step(Grid &grid) = 0;
    virtual std::vector<Vector2i> ConstructPath() = 0;
    virtual void Direct(Grid &grid) {}
};

struct PathAlgoithmEntry
{
    std::string abbr;
    std::string name;
    std::string desc;
    std::function<std::unique_ptr<PathAlgorithm>()> Get;
};

#include "Depth-FirstSearch.h"
#include "Breadth-FirstSearch.h"
#include "RandomDFS.h"

std::vector<PathAlgoithmEntry> GetPathAlgorithms()
{
    std::vector<PathAlgoithmEntry> algorithms;
    PathAlgoithmEntry algorithm_entry; 

    algorithm_entry.abbr = DFS::abbr;
    algorithm_entry.name = DFS::name;
    algorithm_entry.desc = DFS::desc;
    algorithm_entry.Get = []() { return std::make_unique<DFS>(); };

    algorithms.push_back(algorithm_entry);

    algorithm_entry.abbr = BFS::abbr;
    algorithm_entry.name = BFS::name;
    algorithm_entry.desc = BFS::desc;
    algorithm_entry.Get = []() { return std::make_unique<BFS>(); };

    algorithms.push_back(algorithm_entry);

    algorithm_entry.abbr = RandomDFS::abbr;
    algorithm_entry.name = RandomDFS::name;
    algorithm_entry.desc = RandomDFS::desc;
    algorithm_entry.Get = []() { return std::make_unique<RandomDFS>(); };

    algorithms.push_back(algorithm_entry);

    return algorithms;
} 
