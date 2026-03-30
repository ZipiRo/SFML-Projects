#pragma once

//  Planes Engine
/*
    Planes Romanian Game
    
    A two player game where you have a 10x10 board that you can use to place planes on 
    then try to guess the other players planes placemant, the plane has a head, body, wings
    and tail if you hit the head you destroy the plane, there are three planes on the board
    that you need to guess around by hitting the planes parts and guessing where the head is.

    Plane Shape:
        O       -Head
      OOOOO     -Wings
        O       -Body
       OOO      -Tail

    The planes are placed on the 10x10 board, in any way but not overlapping or cross the border, 
    they have a length of four and a width of five.
    The plane pivot is the head, so it is rotated and moved from the head.
    A plane can be rotated by 90deg, that means that the head direction will face right, up, left or down.

    A player asks the other a coordonate ex(1, 2) then the other player tells him if he hit, 
    head or air. 

    Hit - if a part of the body was hit 
    Head - if the planes head was hit, destroing the whole plane
    Air - if nothing was hit

    The board is a 10x10 matrix that can have as values:
        -Empty/Nothing 0
        -Air 1
        -Used 2
        -Part 3
        -Head 4
        -Destroyed 5

    Example of board matrix:

      1 2 3 4 5 6 7 8 9 10
   
   A  0 0 4 0 0 0 0 0 1 0
   B  3 3 5 3 3 0 3 0 3 0
   C  0 0 2 0 0 0 3 3 3 2
   D  0 3 2 2 0 0 3 0 2 0
   E  0 0 0 0 0 0 0 0 3 0
   F  0 0 0 0 2 0 0 0 0 0
   G  0 0 3 2 2 3 3 0 0 0
   H  0 0 0 0 2 0 0 0 0 0
   I  0 0 0 3 3 3 0 0 0 0
   J  0 0 0 0 0 0 0 0 0 0

   Every player has a board matrix that uses to place the planes, and mark the other players
   guesses, if all the planes are destroyed then the player loses.
*/

#include <vector>

const int BOARD_SIZE = 10;
const int MAX_PLANES = 3;

enum CellState
{
    Empty,
    Air,
    Hit,
    Part,
    Head,
    Mid,
    HeadHit
};

enum Direction
{
    Left,
    Up,
    Right,
    Down
};

enum ShootResult
{
    Invalid,
    PlaneHit,
    Miss, 
    Kill
};

enum GamePhase
{
    Waiting,
    Placing,
    Playing,
    Gameover
};

struct Offset 
{
    int dx, dy; 
};

std::vector<Offset> plane_template = {
    {0, 0},                                     // Head
    {-2, 1}, {-1, 1}, {0, 1}, {1, 1}, {2, 1},   // Wings
    {0, 2},                                     // Body
    {-1, 3}, {0, 3}, {1, 3}                     // Tail
};

Offset RotatePlaneOffset(const Offset &offset, Direction direction)
{
    Offset rotated_offset;

    if(direction == Left) rotated_offset = {offset.dy, -offset.dx};
    else if(direction == Up) rotated_offset = {offset.dx, offset.dy};
    else if(direction == Right) rotated_offset = {-offset.dy, offset.dx};
    else rotated_offset = {-offset.dx, -offset.dy};

    return rotated_offset;
}

Direction GetPlaneDirection(int x, int y, std::vector<std::vector<CellState>> board)
{
    if(board[y][x + 1] == CellState::Mid) return Direction::Left;
    if(board[y + 1][x] == CellState::Mid) return Direction::Up;
    if(board[y][x - 1] == CellState::Mid) return Direction::Right;
    if(board[y - 1][x] == CellState::Mid) return Direction::Down;

    return Direction::Left;
}

class Player
{
private:
    bool ValidPlanePosition(int x, int y, Direction direction)
    {
        if(x < 0 || x < 0 || x > BOARD_SIZE - 1 || y > BOARD_SIZE - 1) return false;

        for(int i = 0; i < plane_template.size(); i++)
        {
            Offset offset = RotatePlaneOffset(plane_template[i], direction);

            if(x + offset.dx < 0 || x + offset.dx > BOARD_SIZE - 1) return false;
            if(y + offset.dy < 0 || y + offset.dy > BOARD_SIZE - 1) return false;

            if(board[y + offset.dy][x + offset.dx] != Empty)
                return false;
        }

        return true ;
    }

public:
    int planes_alive;
    std::vector<std::vector<CellState>> board, guess_board;

    Player()
    {
        board.resize(BOARD_SIZE);
        guess_board.resize(BOARD_SIZE);
        for(int i = 0; i < BOARD_SIZE; i++)
        {
            board[i].resize(BOARD_SIZE);
            guess_board[i].resize(BOARD_SIZE);
            for(int j = 0; j < BOARD_SIZE; j++) 
            {
                board[i][j] = CellState::Empty;
                guess_board[i][j] = CellState::Empty;
            }
        }

        planes_alive = 0;
    }

    bool PlacePlane(int x, int y, Direction direction)
    {
        if(!ValidPlanePosition(x, y, direction)) return false;

        for(int i = 0; i < plane_template.size(); i++)
        {
            Offset offset = RotatePlaneOffset(plane_template[i], direction);
            CellState cell_state = CellState::Part;
            if(i == 0) cell_state = CellState::Head;
            else if(i == 3) cell_state = CellState::Mid;

            board[y + offset.dy][x + offset.dx] = cell_state;
        }
        
        planes_alive++;
        return true;
    }
};

class GameState
{
public:
    int winner;
    int current_player;
    GamePhase phase;
    Player player[2];

    GameState()
    {
        player[0] = Player();
        player[1] = Player();
        current_player = 0;
        winner = -1;
        phase = GamePhase::Waiting;
    }
};

struct PlaceAction
{
    int x, y, direction;
};

struct ShoootAction
{
    int x, y;
};

class PlanesEngine
{
private:
    GameState game;

    void DestroyPlane(int x, int y, Player &player)
    {
        Direction plane_direction = GetPlaneDirection(x, y, player.board);
        for(int i = 0; i < plane_template.size(); i++)
        {
            Offset offset = RotatePlaneOffset(plane_template[i], plane_direction);
            player.board[y + offset.dy][x + offset.dx] = CellState::Hit;
        }
    }

public:
    PlanesEngine()
    {
        game = GameState();
    }

    void Start()
    {
        game.phase = GamePhase::Placing;
    }

    void Reset()
    {
        game = GameState();
        game.phase = GamePhase::Waiting;
    }

    bool PlacePlane(const PlaceAction &place_action)
    {   
        if(game.phase != GamePhase::Placing) return false;

        Player &current_player = game.player[game.current_player];

        if(current_player.planes_alive >= MAX_PLANES)
            return false;

        if(!current_player.PlacePlane(place_action.x, place_action.y, Direction(place_action.direction)))
            return false;

        if(current_player.planes_alive == MAX_PLANES)
        {
            game.current_player = 1 - game.current_player;
            
            if(game.current_player == 0)  
                game.phase = GamePhase::Playing;
        }

        return true;
    }

    ShootResult ShootPlane(const ShoootAction &shoot_action)
    {
        if(game.phase != GamePhase::Playing) return ShootResult::Invalid;

        if(shoot_action.x < 0 || shoot_action.x > BOARD_SIZE - 1 || shoot_action.y < 0 || shoot_action.y > BOARD_SIZE - 1) 
            return ShootResult::Invalid;

        Player &other_player = game.player[1 - game.current_player];
        CellState &guess_cell = game.player[game.current_player].guess_board[shoot_action.y][shoot_action.x];
        CellState &target_cell = other_player.board[shoot_action.y][shoot_action.x];

        if(target_cell == CellState::Empty && guess_cell != CellState::Air)
        {
            target_cell = CellState::Air;
            guess_cell = CellState::Air;

            game.current_player = 1 - game.current_player;
            return ShootResult::Miss;
        }
        else if(guess_cell == CellState::Air || guess_cell == CellState::Hit || guess_cell == CellState::HeadHit)
        {    
            return ShootResult::Invalid;
        }
        else if(target_cell == CellState::Hit)
        {
            guess_cell = CellState::Hit;

            game.current_player = 1 - game.current_player;
            return ShootResult::PlaneHit;
        }
        else if(target_cell == CellState::Part || target_cell == CellState::Mid)
        {
            guess_cell = CellState::Hit;
            target_cell = CellState::Hit;

            game.current_player = 1 - game.current_player;
            return ShootResult::PlaneHit;
        }
        else if(target_cell == CellState::Head)
        {
            DestroyPlane(shoot_action.x, shoot_action.y, other_player);
            other_player.planes_alive--;

            target_cell = CellState::HeadHit;
            guess_cell = CellState::HeadHit;

            if(other_player.planes_alive <= 0)
            {
                game.phase = Gameover;
                game.winner = game.current_player;
            }

            game.current_player = 1 - game.current_player;
            return ShootResult::Kill;
        }

        return ShootResult::Invalid;
    }

    // Querry Functions

    const GameState &GetGame() const { return game; }
    bool IsWainting() { return game.phase == GamePhase::Waiting; }
    bool IsPlacing() { return game.phase == GamePhase::Placing; }
    bool IsPlaying() { return game.phase == GamePhase::Playing; }
    bool IsGameOver() { return game.phase == GamePhase::Gameover; }

    const Player &GetCurrentPlayer() { return game.player[game.current_player]; }
    const Player &GetOtherPlayer() { return game.player[1 - game.current_player]; }
};