const std::string RoomColor = "Room";
const std::string WallColor = "Wall";
const std::string LinesColor = "Lines";
const std::string PathStartColor = "Path Start";
const std::string PathEndColor = "Path End";
const std::string PathExploredColor = "Path Expored";
const std::string PathFrontierColor = "Path Frontier";
const std::string PathBacktrackColor = "Path Backtrack";
const std::string PathColor = "Path";
const std::string MazeStartColor = "Maze Start";
const std::string MazePrimaryColor = "Maze Primary";
const std::string MazeSecondaryColor = "Maze Secondary";

struct GridTheme
{
    std::string name;
    std::map<std::string, Color> colors; 
};

namespace ColorTheme
{
    GridTheme BlackAndGray()
    {
        GridTheme theme{.name = "BlackAndGray"};

        theme.colors[RoomColor] = Color(40, 40, 40);
        theme.colors[WallColor] = Color(20, 20, 20);
        theme.colors[LinesColor] = Color(20, 20, 20);
        theme.colors[PathStartColor] = Color(153, 212, 85);
        theme.colors[PathEndColor] = Color(208, 34, 34);
        theme.colors[PathExploredColor] = Color(48, 133, 179);
        theme.colors[PathFrontierColor] = Color(77, 193, 255);
        theme.colors[PathBacktrackColor] = Color(26, 72, 97);
        theme.colors[PathColor] = Color(105, 210, 63);
        theme.colors[MazeStartColor] = Color(206, 89, 31); 
        theme.colors[MazePrimaryColor] = Color(210, 50, 50);
        theme.colors[MazeSecondaryColor] = Color(142, 34, 34);

        return theme;
    }
   
    GridTheme WhiteAndBlack()
    {
        GridTheme theme{.name = "WhiteAndBlack"};

        theme.colors[RoomColor] = Color(255, 255, 255);
        theme.colors[WallColor] = Color(0, 0, 0);
        theme.colors[LinesColor] = Color(0, 0, 0);
        theme.colors[PathStartColor] = Color(153, 212, 85);
        theme.colors[PathEndColor] = Color(208, 34, 34);
        theme.colors[PathExploredColor] = Color(48, 133, 179);
        theme.colors[PathFrontierColor] = Color(77, 193, 255);
        theme.colors[PathBacktrackColor] = Color(26, 72, 97);
        theme.colors[PathColor] = Color(105, 210, 63);
        theme.colors[MazeStartColor] = Color(206, 89, 31); 
        theme.colors[MazePrimaryColor] = Color(210, 50, 50);
        theme.colors[MazeSecondaryColor] = Color(142, 34, 34);

        return theme;
    }
}

std::vector<GridTheme> GetGridColorThemes()
{
    std::vector<GridTheme> themes;  

    themes.push_back(ColorTheme::BlackAndGray());
    themes.push_back(ColorTheme::WhiteAndBlack());

    return themes;
}