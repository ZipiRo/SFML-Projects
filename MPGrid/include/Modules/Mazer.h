class Mazer : public Module
{
private:
    enum AlgoState
    {
        ALGO_STAL,
        ALGO_INIT,
        ALGO_RUN,
        ALGO_DONE
    };

    Point start;

    std::unique_ptr<MazeAlgorithm> algorithm;
    const std::vector<MazeAlgorithmEntry> algorithms = GetMazeAlgorithms();

    bool pause_algorithm;
    bool running_algorithm;
    bool reset_grid;
    int using_algorithm;

    bool no_step_algorithm;

    float step_timer;
    float start_timer;

    float algo_step_delay;
    int start_algo_delay;

    AlgoState algorithm_state;

    void SidebarInterface(ApplicationContext&) override;
    void SettingsInterface(ApplicationContext&) override;

    void PlaceStart(Vector2i where, Grid &grid, GridTheme theme)
    {
        if(!start.valid)
        {
            start.position = where;
            start.valid = true;

            grid.SetCell(where.x, where.y, CELL_NONE, theme.colors[MazeStartColor]);
        }
    }

    void RemoveStart(Vector2i where, Grid &grid)
    {
        if(where == start.position && start.valid)
        {
            start.valid = false;
            grid.SetCell(where.x, where.y, CELL_ROOM);
        }
    }

    void RandomStart(Grid &grid, GridTheme theme)
    {
        Vector2i random_position = Vector2i(rand() % grid.GetSize().x, rand() % grid.GetSize().y);
        while (!start.valid)
        {
            random_position = Vector2i(rand() % grid.GetSize().x, rand() % grid.GetSize().y);
            PlaceStart(random_position, grid, theme);
        }
    }

    void Reset()
    {
        algorithm_state = ALGO_STAL;
        pause_algorithm = false;
        running_algorithm = false;
        start.valid = false;
        reset_grid = true;
    }

    void Run(GridTheme theme)
    {
        if(!start.valid) return;
        running_algorithm = true;
        start_timer = 0.0f;

        algorithm = algorithms[using_algorithm].Get();
        algorithm->primary_color = theme.colors[MazePrimaryColor];
        algorithm->secondary_color = theme.colors[MazeSecondaryColor];

        algorithm_state = ALGO_INIT;
    }

    std::string GetSidebarTitle() override
    {
        return "Mazer";
    }

    std::string GetSettingsTitle() override 
    {
        return "Mazer Settings";
    }

    void AlgorithmUpdate(Grid &grid, float delta_time);
public:
    Mazer() 
    {
        pause_algorithm = false;
        using_algorithm = 0;
        reset_grid = false;
        algorithm_state = ALGO_STAL;

        algo_step_delay = 0.05f;
        start_algo_delay = 3;

        no_step_algorithm = false;
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
            AlgorithmUpdate(context.grid, context.delta_time);
            return;
        }

        Vector2i mouse_position = Mouse::getPosition(context.window);
        Point cursor = context.grid_cursor.GetCursorPoint(mouse_position, context.grid_render);

        if(Input::IsMouseButtonDown(Mouse::Button::Left) && cursor.valid)
            PlaceStart(cursor.position, context.grid, context.grid_render.GetColorTheme());
        
        if(Input::IsMouseButtonDown(Mouse::Button::Right) && cursor.valid)
            RemoveStart(cursor.position, context.grid);

        if(Input::IsKeyDown(Keyboard::Key::T))
            RandomStart(context.grid, context.grid_render.GetColorTheme());

        if(Input::IsKeyDown(Keyboard::Key::F))
        {
            context.grid.Fill(CELL_WALL);
            start.valid = false;
        }

        if(Input::IsKeyDown(Keyboard::Key::C))
        {
            context.grid.Fill(CELL_ROOM);
            start.valid = false;
        }
     
        if(Input::IsKeyDown(Keyboard::Key::Space))
            Run(context.grid_render.GetColorTheme());
        
        if(Input::IsKey(Keyboard::Key::LControl) && Input::MouseWheelDelta() > 0)
            using_algorithm = std::min((int)algorithms.size() - 1, using_algorithm + 1);

        if(Input::IsKey(Keyboard::Key::LControl) && Input::MouseWheelDelta() < 0)
            using_algorithm = std::max(0, using_algorithm - 1);
    }
};

void Mazer::AlgorithmUpdate(Grid &grid, float delta_time)
{
    if(pause_algorithm) return;

    switch (algorithm_state)
    {
    case ALGO_INIT:
        start_timer += delta_time;
        if(start_timer < start_algo_delay) return;

        algorithm->Init(grid, start.position);

        step_timer = algo_step_delay;
        algorithm_state = ALGO_RUN;
        break;
    case ALGO_RUN:
        step_timer += delta_time;
        if(step_timer < algo_step_delay) return;

        no_step_algorithm ? algorithm->Direct(grid) : algorithm->Step(grid);

        if(algorithm->done)
            algorithm_state = ALGO_DONE;

        step_timer = 0.0f;
        break;
    case ALGO_DONE: 
        Reset();
        break;
    default:
        break;
    }
}

#include "Interfaces/MazerUI.h"