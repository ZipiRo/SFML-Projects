struct MazeAlgorithm
{
    bool done = false;

    Color primary_color;
    Color secondary_color;

    virtual void Init(const Grid &grid, Vector2i start) = 0;
    virtual void Step(Grid &grid) = 0;
    virtual void Direct(Grid &grid) {}
};

struct MazeAlgorithmEntry
{
    std::string abbr;
    std::string name;
    std::string desc;
    std::function<std::unique_ptr<MazeAlgorithm>()> Get;
};

#include "RecursiveBactracking.h"
#include "RecursiveDivision.h"

std::vector<MazeAlgorithmEntry> GetMazeAlgorithms()
{
    std::vector<MazeAlgorithmEntry> algorithms;
    MazeAlgorithmEntry algorithm_entry; 

    algorithm_entry.abbr = RB::abbr;
    algorithm_entry.name = RB::name;
    algorithm_entry.desc = RB::desc;
    algorithm_entry.Get = [](){ return std::make_unique<RB>(); };

    algorithms.push_back(algorithm_entry);

    algorithm_entry.abbr = RD::abbr;
    algorithm_entry.name = RD::name;
    algorithm_entry.desc = RD::desc;
    algorithm_entry.Get = [](){ return std::make_unique<RD>(); };

    algorithms.push_back(algorithm_entry);

    return algorithms;
} 
