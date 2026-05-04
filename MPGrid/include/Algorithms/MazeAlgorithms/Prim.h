class Prim : public MazeAlgorithm
{
private:
    Vector2i start;
    std::vector<Vector2i> frontiers;

    enum AlgoritmState
    {
        INIT,
        STEP
    } algo_state;

    void AddFrontiers(Vector2i current, const Grid &grid)
    {
        for(const auto &direction : directions)
        {
            Vector2i frontier = current + direction * 2;

            if(!grid.InBounds(frontier.x, frontier.y)) continue;
            if(grid.Get(frontier.x, frontier.y).type != CELL_WALL) continue;

            frontiers.push_back(frontier);
        }
    }

    std::vector<Vector2i> GetNeighbours(Vector2i current, const Grid &grid)
    {
        std::vector<Vector2i> neighbours;
        
        for(const auto &direction : directions)
        {
            Vector2i neighbour = current + direction * 2;

            if(!grid.InBounds(neighbour.x, neighbour.y)) continue;
            if(grid.Get(neighbour.x, neighbour.y).type != CELL_ROOM) continue;

            neighbours.push_back(neighbour);
        }

        return neighbours;
    }

public:
    static std::string abbr;
    static std::string name;
    static std::string desc;

    Prim() {}

    void Init(const Grid& grid, Vector2i start) override
    {
        this->start = start;
        done = false;

        algo_state = INIT;
    }

    void Step(Grid& grid) override
    {
        if(algo_state == INIT)
        {
            grid.SetCell(start.x, start.y, CELL_ROOM, primary_color);
            AddFrontiers(start, grid);
            algo_state = STEP;
        }

        if(frontiers.empty())
        {
            done = true;
            return;
        }

        int index = rand() % frontiers.size();
        Vector2i current = frontiers[index];
        frontiers.erase(frontiers.begin() + index);

        std::vector<Vector2i> neighbours = GetNeighbours(current, grid);

        if(!neighbours.empty())
        {
            Vector2i neighbour = neighbours[rand() % neighbours.size()];

            grid.SetCell(current.x, current.y, CELL_ROOM, primary_color);
            grid.SetCell((current.x + neighbour.x) / 2, (current.y + neighbour.y) / 2, CELL_ROOM, secondary_color);

            AddFrontiers(current, grid);
        }
    }
};

std::string Prim::abbr = "Prim";
std::string Prim::name = "Prim";
std::string Prim::desc = "Prim’s algorithm (Prim) is a method for building a maze by starting from a cell and gradually expanding outward, always choosing the next cell through the smallest available connection until the whole area is connected.";