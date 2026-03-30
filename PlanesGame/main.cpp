#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
 
#include "include/Timer.h"

using namespace sf;

RenderWindow window;
const int window_width = 800;
const int window_height = 600;
const char *window_title = "Planes";

const int FPS = 60;

const float PI = 3.1415926354f;

const Color BackgroundColor = Color(123, 123, 123);

View camera;
View canvas;

Vector2f canvasMousePosition;
Vector2f worldMousePosition;

Font font_JetBrains{};

#include "include/Board.h"

const int CELL_SIZE = 30;

Board player_main_board;
Board player_guess_board;

Text board_title_text{font_JetBrains};

Player example_player;

PlanesEngine engine;

void InitNoShootCells(std::vector<Vector2i> &cells)
{
    for(int i = 0; i < BOARD_SIZE; i++)
        for(int j = 0; j < BOARD_SIZE; j++)
            cells.push_back(Vector2i(i, j));
}

std::vector<Vector2i> ShuffleCells(std::vector<Vector2i> cells) 
{
    for (int i = cells.size() - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        std::swap(cells[i], cells[j]);
    }

    return cells;
}

void PlaceRandomPlanes()
{
    int x, y, direction;

    x = rand() % 10 + 1;
    y = rand() % 10 + 1;
    direction = rand() % 3 + 1;

    while (engine.GetCurrentPlayer().planes_alive != 3)
    {
        while (!engine.PlacePlane({x, y, Direction(direction)}))
        {            
            x = rand() % 10 + 1;
            y = rand() % 10 + 1;
            direction = rand() % 3 + 1;
        }
    }

}

std::vector<Vector2i> no_shoot_cells;
void RandomShootAction()
{
    no_shoot_cells = ShuffleCells(no_shoot_cells);
    Vector2i shoot_cell = no_shoot_cells.back();
    no_shoot_cells.pop_back();

    while(engine.ShootPlane({shoot_cell.x, shoot_cell.y}) == Invalid)
    {
        shoot_cell = no_shoot_cells.back();
        no_shoot_cells.pop_back();
    }
}

bool left_pressed = false;
int place_direction = 0;
void PlayerPlace()
{
    if(Keyboard::isKeyPressed(Keyboard::Key::A)) place_direction = 0;
    else if(Keyboard::isKeyPressed(Keyboard::Key::W)) place_direction = 1;
    else if(Keyboard::isKeyPressed(Keyboard::Key::D)) place_direction = 2;
    else if(Keyboard::isKeyPressed(Keyboard::Key::S)) place_direction = 3;
    
    sf::Vector2i current_cell_hover = player_main_board.GetHoveredCellIndex(canvasMousePosition);

    Player player_board = engine.GetGame().player[0];

    player_board.PlacePlane(current_cell_hover.x, current_cell_hover.y, Direction(place_direction));
    player_main_board.SetBoard(player_board.board);

    if(Mouse::isButtonPressed(Mouse::Button::Left) && !left_pressed)
    {
        if(current_cell_hover.x == -1 || current_cell_hover.y == -1) return;
        engine.PlacePlane({current_cell_hover.x, current_cell_hover.y, Direction(place_direction)});

        left_pressed = true;
    }
    else if(!Mouse::isButtonPressed(Mouse::Button::Left))
    {
        left_pressed = false;
    }
}

void PlayerShoot()
{
    sf::Vector2i current_cell_hover = player_guess_board.GetHoveredCellIndex(canvasMousePosition);

    if(Mouse::isButtonPressed(Mouse::Button::Left) && !left_pressed)
    {
        if(current_cell_hover.x == -1 || current_cell_hover.y == -1) return;
        engine.ShootPlane({current_cell_hover.x, current_cell_hover.y});

        left_pressed = true;
    }
    else if(!Mouse::isButtonPressed(Mouse::Button::Left))
    {
        left_pressed = false;
    }
}

void Start()
{
    canvas = window.getView();
    camera = View(Vector2f(0, 0), Vector2f(window_width, window_height));

    srand(time(0));

    InitNoShootCells(no_shoot_cells);

    if(!font_JetBrains.openFromFile("resources/JetBrainsMono-Regular.ttf"));

    board_title_text.setFont(font_JetBrains);
    board_title_text.setFillColor(sf::Color::Black);
    board_title_text.setCharacterSize(25);

    Vector2f board_mid_on_screen;
    board_mid_on_screen.x = (canvas.getSize().x * 0.5) - (BOARD_SIZE * CELL_SIZE * 0.5);
    board_mid_on_screen.y = (canvas.getSize().y * 0.5) - (BOARD_SIZE * CELL_SIZE * 0.5);

    player_main_board = Board(BOARD_SIZE, CELL_SIZE, "MAIN BOARD");
    player_main_board.SetPosition(board_mid_on_screen.x - (BOARD_SIZE * CELL_SIZE * 0.5) - 50, board_mid_on_screen.y);
    player_main_board.SetBoardCellColors(sf::Color::White, sf::Color::Blue, sf::Color::Red, sf::Color::Black, sf::Color::Green);
    player_main_board.SetCellOutlineColor(sf::Color(50, 50, 50));

    player_guess_board = Board(BOARD_SIZE, CELL_SIZE, "GUESS BOARD");
    player_guess_board.SetPosition(board_mid_on_screen.x + (BOARD_SIZE * CELL_SIZE * 0.5) + 50, board_mid_on_screen.y);
    player_guess_board.SetBoardCellColors(sf::Color::White, sf::Color::Blue, sf::Color::Red, sf::Color::Black, sf::Color::Green);
    player_guess_board.SetCellOutlineColor(sf::Color(50, 50, 50));

    engine.Start();
}

void Update()
{
    player_main_board.SetBoard(engine.GetGame().player[0].board);
    player_guess_board.SetBoard(engine.GetGame().player[0].guess_board);

    player_main_board.Update();
    player_guess_board.Update();

    if(engine.IsPlacing())
    {
        if(engine.GetGame().current_player != 0)
        {
            PlaceRandomPlanes();
            return;
        }

        PlayerPlace();
    }
    else if(engine.IsPlaying())
    {
        if(engine.GetGame().current_player != 0)
        {
            RandomShootAction();
            return;
        }

        PlayerShoot();
    }
    else if(engine.IsGameOver())
    {

    }
}

void Draw()
{
    window.setView(camera);

    window.setView(canvas);

    player_main_board.Draw(window);
    player_guess_board.Draw(window);
}

int main()
{
    window = RenderWindow(VideoMode({window_width, window_height}), window_title);
    
    Timer::GetInstance();
    Start();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }
        
        Timer::Tick();
        if(Timer::deltaTime >= 1.0f / FPS)
        {
            Timer::Reset();

            canvasMousePosition = window.mapPixelToCoords(Mouse::getPosition(window), canvas);
            worldMousePosition = window.mapPixelToCoords(Mouse::getPosition(window), camera);

            Update();
            window.clear(BackgroundColor);
            Draw();
            window.display();
        }
    }
}