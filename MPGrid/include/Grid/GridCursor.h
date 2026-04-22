class GridCursor
{
private:
    Point cursor;
    RectangleShape cursor_shape;

    Color cursor_color;
    float cursor_outline_thickness;
public:

    GridCursor() {}
    
    void Init(Vector2f cell_size)
    {
        cursor_color = Color::Black;
        cursor_outline_thickness = 2.0f;

        cursor_shape = RectangleShape(Vector2f(cell_size.x - 2, cell_size.y - 2));
        cursor_shape.setFillColor(Color::Transparent);
        cursor_shape.setOutlineColor(cursor_color);
        cursor_shape.setOutlineThickness(cursor_outline_thickness);
    }

    void SetCursorColor(Color color)
    {
        cursor_color = color;
        cursor_shape.setOutlineColor(cursor_color);
    }

    void SetOutlineThickness(float x)
    {
        cursor_outline_thickness = x;
        cursor_shape.setOutlineThickness(cursor_outline_thickness);
    }

    Point GetCursorPoint(Vector2i mouse_position, const GridRenderer &grid_render)
    {
        cursor = grid_render.GetCellPoint(Vector2f(mouse_position));

        return cursor;
    }

    void Draw(RenderWindow &window, const GridRenderer &grid_render)
    {
        Vector2f grid_position = grid_render.GetOffsetPosition();
        Vector2f cell_size = grid_render.GetCellSize();

        if(cursor.valid)
        {
            cursor_shape.setPosition(Vector2f(grid_position.x + cursor.position.x * cell_size.x, 
                                        grid_position.y + cursor.position.y * cell_size.y));

            window.draw(cursor_shape);
            cursor.valid = false;
        }
    }
};