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
    GridTheme Default()
    {
        GridTheme theme{.name = "Default"};

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
   
    GridTheme MonoChrome()
    {
        GridTheme theme{ .name = "MonoChrome" };

        theme.colors[RoomColor] = Color(255, 255, 255);
        theme.colors[WallColor] = Color(0, 0, 0);
        theme.colors[LinesColor] = Color(30, 30, 30);
        theme.colors[PathStartColor] = Color(153, 212, 85);
        theme.colors[PathEndColor] = Color(208, 34, 34);
        theme.colors[PathExploredColor] = Color(160, 160, 160);
        theme.colors[PathFrontierColor] = Color(120, 120, 120);
        theme.colors[PathBacktrackColor] = Color(80, 80, 80);
        theme.colors[PathColor] = Color(40, 40, 40);
        theme.colors[MazeStartColor] = Color(206, 89, 31); 
        theme.colors[MazePrimaryColor] = Color(50, 50, 50);
        theme.colors[MazeSecondaryColor] = Color(120, 120, 120);

        return theme;
    }

    GridTheme OceanDepths()
    {
        GridTheme theme{ .name = "OceanDepths" };

        theme.colors[RoomColor] = Color(12, 24, 38);
        theme.colors[WallColor] = Color(4, 10, 18);
        theme.colors[LinesColor] = Color(4, 10, 18);
        theme.colors[PathStartColor] = Color(46, 204, 113); 
        theme.colors[PathEndColor] = Color(231, 76, 60);  
        theme.colors[PathExploredColor] = Color(52, 152, 219);
        theme.colors[PathFrontierColor] = Color(102, 217, 255);
        theme.colors[PathBacktrackColor] = Color(41, 80, 120); 
        theme.colors[PathColor] = Color(241, 196, 15); 
        theme.colors[MazeStartColor] = Color(155, 89, 182);
        theme.colors[MazePrimaryColor] = Color(26, 188, 156);
        theme.colors[MazeSecondaryColor] = Color(22, 160, 133); 

        return theme;
    }

    GridTheme NeonContrast()
    {
        GridTheme theme{ .name = "NeonContrast" };

        theme.colors[RoomColor] = Color(18, 18, 22);   
        theme.colors[WallColor] = Color(40, 40, 48);   
        theme.colors[LinesColor] = Color(40, 40, 48);   
        theme.colors[PathStartColor] = Color(0, 255, 140);  
        theme.colors[PathEndColor] = Color(255, 70, 90);  
        theme.colors[PathExploredColor] = Color(80, 140, 255); 
        theme.colors[PathFrontierColor] = Color(120, 200, 255);
        theme.colors[PathBacktrackColor] = Color(90, 90, 120);  
        theme.colors[PathColor] = Color(255, 230, 80); 
        theme.colors[MazeStartColor] = Color(255, 140, 0);  
        theme.colors[MazePrimaryColor] = Color(255, 60, 60);  
        theme.colors[MazeSecondaryColor] = Color(160, 40, 200); 

        return theme;
    }
}


std::vector<GridTheme> GetGridColorThemes()
{
    std::vector<GridTheme> themes;  

    themes.push_back(ColorTheme::Default());
    themes.push_back(ColorTheme::MonoChrome());
    themes.push_back(ColorTheme::OceanDepths());
    themes.push_back(ColorTheme::NeonContrast());

    return themes;
}