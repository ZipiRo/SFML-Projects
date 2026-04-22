class Pathfinder : public Module
{
private:
    enum AlgoState
    {
        ALGO_STAL, 
        ALGO_INIT,
        ALGO_RUN,
        ALGO_PATH_FOUND
    };

    Point start, end;

    std::vector<Vector2i> path;

    std::unique_ptr<PathAlgorithm> algorithm;
    const std::vector<PathAlgoithmEntry> algorithms = GetPathAlgorithms();

    bool pause_algorithm;
    bool running_algorithm;
    bool reset_grid;
    int using_algorithm;
    int path_index;

    bool no_step_algorithm;
    bool no_step_path;

    float step_timer;
    float start_timer;

    float algo_step_delay;
    float path_step_delay;
    int start_algo_delay;

    AlgoState algorithm_state;
    
    void SidebarInterface(ApplicationContext&) override;
    void SettingsInterface(ApplicationContext&) override;

    void Place(Vector2i where, Grid &grid, GridTheme theme)
    {
        if(grid.Get(where.x, where.y).type != CELL_ROOM) return;

        if(!start.valid)
        {
            start.position = where;
            start.valid = true;

            grid.SetCell(where.x, where.y, CELL_NONE, theme.colors[PathStartColor]);
        }
        else if(!end.valid)
        {
            end.position = where;
            end.valid = true;

            grid.SetCell(where.x, where.y, CELL_NONE, theme.colors[PathEndColor]);
        }
    }

    void Remove(Vector2i where, Grid &grid)
    {
        if(where == start.position && start.valid)
        {
            start.valid = false;
            grid.SetCell(where.x, where.y, CELL_ROOM);
        }
        else if(where == end.position && end.valid)
        {
            end.valid = false;
            grid.SetCell(where.x, where.y, CELL_ROOM);
        }
    }

    void RandomStartEnd(Grid &grid, GridTheme theme)
    {
        Vector2i random_position = Vector2i(rand() % grid.GetSize().x, rand() % grid.GetSize().y);
        while (!start.valid || !end.valid)
        {
            random_position = Vector2i(rand() % grid.GetSize().x, rand() % grid.GetSize().y);
            Place(random_position, grid, theme);
        }
    }

    void Reset()
    {
        algorithm_state = ALGO_STAL;
        pause_algorithm = false;
        running_algorithm = false;
        start.valid = false;
        end.valid = false;
        reset_grid = true;
    }

    void StepPath(Grid &grid, GridTheme theme)
    {
        if(path_index >= path.size()) 
        {
            algorithm_state = ALGO_STAL;
            return;
        }

        Vector2i point = path[path_index++];
        grid.SetCell(point.x, point.y, CELL_NONE, theme.colors[PathColor]);
    }

    void DirectPath(Grid &grid, GridTheme theme)
    {
        while (path_index < path.size())
        {
            Vector2i point = path[path_index++];
            grid.SetCell(point.x, point.y, CELL_NONE, theme.colors[PathColor]);
        }

        algorithm_state = ALGO_STAL;
    }

    void Run(GridTheme theme)
    {
        if(!start.valid || !end.valid) return;
        running_algorithm = true;
        start_timer = 0.0f;

        algorithm = algorithms[using_algorithm].Get();
        algorithm->explored_color = theme.colors[PathExploredColor];
        algorithm->frontier_color = theme.colors[PathFrontierColor];
        algorithm->backtrack_color = theme.colors[PathBacktrackColor];

        algorithm_state = ALGO_INIT;
    }

    std::string GetSidebarTitle() override
    {
        return "Pathfinder";
    }

    std::string GetSettingsTitle() override 
    {
        return "Pathfinder Settings";
    }

    void AlgorithmUpdate(ApplicationContext &context);
public:
    Pathfinder()
    {   
        pause_algorithm = false;
        using_algorithm = 0;
        path_index = 0;
        reset_grid = false;
        algorithm_state = ALGO_STAL;

        algo_step_delay = 0.05f;
        path_step_delay = 0.05f;
        start_algo_delay = 3;

        no_step_algorithm = false;
        no_step_path = false;
    }

    void Init(ApplicationContext &context) override
    {
        Reset();
    }

    void Update(ApplicationContext &context) override
    {   
        if(Input::IsKeyDown(Keyboard::Key::R))
            Reset();

        if(Input::IsKeyDown(Keyboard::Key::P))
            pause_algorithm = !pause_algorithm;
        
        if(reset_grid)
        {
            context.grid.ClearColors();
            reset_grid = false;
        }

        if(running_algorithm)
        {
            AlgorithmUpdate(context);
            return;
        }

        Vector2i mouse_position = Mouse::getPosition(context.window);
        Point cursor = context.grid_cursor.GetCursorPoint(mouse_position, context.grid_render);

        if(Input::IsMouseButtonDown(Mouse::Button::Left) && cursor.valid)
            Place(cursor.position, context.grid, context.grid_render.GetColorTheme());
        
        if(Input::IsMouseButtonDown(Mouse::Button::Right) && cursor.valid)
            Remove(cursor.position, context.grid);

        if(Input::IsKeyDown(Keyboard::Key::T))
            RandomStartEnd(context.grid, context.grid_render.GetColorTheme());
     
        if(Input::IsKeyDown(Keyboard::Key::Space))
            Run(context.grid_render.GetColorTheme());
        
        if(Input::IsKey(Keyboard::Key::LControl) && Input::MouseWheelDelta() > 0)
            using_algorithm = std::min((int)algorithms.size() - 1, using_algorithm + 1);

        if(Input::IsKey(Keyboard::Key::LControl) && Input::MouseWheelDelta() < 0)
            using_algorithm = std::max(0, using_algorithm - 1);
    }
};

void Pathfinder::AlgorithmUpdate(ApplicationContext &context)
{
    if(pause_algorithm) return;
    switch (algorithm_state)
    {
    case ALGO_INIT:
        start_timer += context.delta_time;
        if(start_timer < start_algo_delay) return;

        algorithm->Init(context.grid, start.position, end.position);

        step_timer = algo_step_delay;
        algorithm_state = ALGO_RUN;
        break;
    case ALGO_RUN:
        step_timer += context.delta_time;
        if(step_timer < algo_step_delay) return;

        no_step_algorithm ? algorithm->Direct(context.grid) : algorithm->Step(context.grid);

        if(algorithm->path_found)
        {
            context.grid.ClearColors();
            context.grid.SetCell(start.position.x, start.position.y, CELL_NONE, context.grid_render.GetColorTheme().colors[PathStartColor]);
            context.grid.SetCell(end.position.x, end.position.y, CELL_NONE, context.grid_render.GetColorTheme().colors[PathEndColor]);
            path = algorithm->ConstructPath();
            algorithm_state = ALGO_PATH_FOUND;
        }
        else if(algorithm->done)
        {
            algorithm_state = ALGO_STAL;
            path_index = 0;
            running_algorithm = false;
            start.valid = false;
            end.valid = false;
        }

        step_timer = 0.0f;
        break;
    case ALGO_PATH_FOUND:
        step_timer += context.delta_time;
        if(step_timer < path_step_delay) return;

        no_step_path ? DirectPath(context.grid, context.grid_render.GetColorTheme()): 
                        StepPath(context.grid, context.grid_render.GetColorTheme());

        if(algorithm_state == ALGO_STAL)
        {
            path_index = 0;
            running_algorithm = false;
            start.valid = false;
            end.valid = false;
        }

        step_timer = 0.0f;
        break;
    default:
        break;
    }
}

#include "Interfaces/PathfinderUI.h"