struct GridColorTheme
{
    std::string name;

    Color room_color;
    Color wall_color;
    Color lines_color;

    Color path_start_color;
    Color path_end_color;
    Color path_explored_color;
    Color path_frontier_color;
    Color path_backtrack_color;
    Color path_color;

    Color maze_start_color;
    Color maze_primary_color;
    Color maze_secondary_color;
};

namespace ThemeColor
{
    GridColorTheme Default()
    {
        GridColorTheme theme;

        theme.name = "Default";
        theme.room_color = Color(40, 40, 40);
        theme.wall_color = Color(20, 20, 20);
        theme.lines_color = Color(20, 20, 20);

        theme.path_start_color = Color(153, 212, 85);
        theme.path_end_color = Color(208, 34, 34);
        theme.path_explored_color = Color(48, 133, 179);
        theme.path_frontier_color = Color(77, 193, 255);
        theme.path_backtrack_color = Color(26, 72, 97);
        theme.path_color = Color(105, 210, 63);

        theme.maze_start_color = Color(206, 89, 31);
        theme.maze_primary_color = Color(210, 50, 50);
        theme.maze_secondary_color = Color(142, 34, 34);

        return theme;
    }
}

std::vector<GridColorTheme> GetGridColorThemes()
{
    std::vector<GridColorTheme> themes;  

    themes.push_back(ThemeColor::Default());

    return themes;
}