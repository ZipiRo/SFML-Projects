#include "Planes.h"

struct Cell
{
    int type = Empty;
    int hovering = false;
};

class Board
{
private:
    sf::Vector2f position;

    std::vector<std::vector<Cell>> board;
    int board_size;
    int cell_size;

    std::string board_name;

    sf::Vector2i current_cell_hover;

    sf::RectangleShape shape;

    sf::Color cell_empty_color;
    sf::Color cell_air_color;
    sf::Color cell_hit_color;
    sf::Color cell_part_color;
    sf::Color cell_headhit_color;

    sf::Color cell_outline_color;

    sf::Text board_text{font_JetBrains};

public:
    Board() {}
    Board(int board_size, int cell_size, const std::string &board_name)
    {
        position = sf::Vector2f(0, 0);
        current_cell_hover = sf::Vector2i(0, 0);
        this->board_size = board_size;
        this->cell_size = cell_size;
        this->board_name = board_name;

        board.resize(board_size);
        for(int i = 0; i < board_size; i++)
            board[i].resize(board_size);

        cell_empty_color = sf::Color::White;
        cell_outline_color = sf::Color::Black;

        shape = sf::RectangleShape(sf::Vector2f(cell_size, cell_size));
        shape.setFillColor(cell_empty_color);
        shape.setOutlineColor(cell_outline_color);
        shape.setOutlineThickness(2);

        board_text.setFont(font_JetBrains);
        board_text.setFillColor(sf::Color(0, 0, 0, 128));
        board_text.setCharacterSize(10);
        board_text.setStyle(sf::Text::Bold);
    }

    void SetPosition(float x, float y)
    {
        position = sf::Vector2f(x, y);
    }

    sf::Vector2f GetPosition()
    {
        return position;
    }

    void SetBoardCellColors(sf::Color empty_color, sf::Color air_color, sf::Color hit_color, sf::Color part_color, sf::Color headhit_color)
    {
        cell_empty_color = empty_color;
        cell_air_color = air_color;
        cell_hit_color = hit_color;
        cell_part_color = part_color;
        cell_headhit_color = headhit_color;

        shape.setFillColor(cell_empty_color);
    }

    void SetCellOutlineColor(sf::Color color)
    {
        cell_outline_color = color;
        shape.setOutlineColor(cell_outline_color);
    }

    void SetBoard(const std::vector<std::vector<CellState>> &new_board)
    {
        for(int i = 0; i < board_size; i++)
        {
            for(int j = 0; j < board_size; j++)
            {
                board[i][j].type = new_board[i][j];
            }
        }
    }

    sf::Vector2i GetHoveredCellIndex(sf::Vector2f mouse_position)
    {
        for(int i = 0; i < board_size; i++)
        {
            for(int j = 0; j < board_size; j++)
            {
                board[i][j].hovering = false;
                
                int left = j * cell_size + position.x;
                int top = i * cell_size + position.y;
                int right = left + cell_size;
                int bottom = top + cell_size;

                if(mouse_position.x >= left && mouse_position.y >= top &&
                    mouse_position.x <= right && mouse_position.y <= bottom)
                    {
                        board[i][j].hovering = true;
                        current_cell_hover = sf::Vector2i(j, i);
                    }
            }
        }
        return current_cell_hover;
    }

    void Update()
    {
        if(current_cell_hover != Vector2i(-1, -1))
            board[current_cell_hover.y][current_cell_hover.x].hovering = false;
        
        current_cell_hover = Vector2i(-1, -1);
    }

    void Draw(sf::RenderWindow &window)
    {
        board_text.setFillColor(sf::Color::Black);
        board_text.setString(board_name);
        board_text.setCharacterSize(25);
        board_text.setPosition(position - Vector2f(0, 40));

        window.draw(board_text);

        for(int i = 0; i < board_size; i++)
        {
            for(int j = 0; j < board_size; j++)
            {
                Cell current_cell = board[i][j];
                sf::Color fill_current_color;
                sf::Color outline_current_color = cell_outline_color;

                if(current_cell.type == Empty) fill_current_color = cell_empty_color;
                else if(current_cell.type == Air) fill_current_color = cell_air_color;
                else if(current_cell.type == Hit) fill_current_color = cell_hit_color;
                else if(current_cell.type == Part || 
                        current_cell.type == Head || 
                        current_cell.type == Mid) fill_current_color = cell_part_color;
                else if(current_cell.type == HeadHit) fill_current_color = cell_headhit_color;

                if(current_cell.hovering)
                    outline_current_color = sf::Color::Red;

                shape.setFillColor(fill_current_color);
                shape.setOutlineColor(outline_current_color);

                shape.setPosition(sf::Vector2f(j * cell_size, i * cell_size) + position);
                window.draw(shape);
                
                sf::Color fill_text_color = sf::Color::White - fill_current_color;
                fill_text_color.a = 255;

                if(i == 0)
                {
                    board_text.setString(std::to_string(j + 1));
                    board_text.setPosition(sf::Vector2f(j * cell_size, 0) + position);
                }
                else if(j == 0)
                {
                    board_text.setString(std::to_string(i + 1));
                    board_text.setPosition(sf::Vector2f(0, i * cell_size) + position);
                }

                if(i == 0 || j == 0)
                {
                    board_text.setCharacterSize(10);
                    board_text.setFillColor(fill_text_color);
                    window.draw(board_text);
                }
            }
        }
    }
};