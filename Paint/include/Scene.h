const int PIXEL_SIZE = 10;
const int PIXEL_ROWS = window_height / PIXEL_SIZE;
const int PIXEL_COLS = window_width / PIXEL_SIZE;

struct Pixel
{
    Color color;
};

Pixel screen[PIXEL_COLS][PIXEL_COLS];

RectangleShape PIXEL_SHAPE;  

Color screenColor;

void ClearCanvas()
{    
    for(int i = 0; i < PIXEL_ROWS; i++)
        for(int j = 0; j < PIXEL_COLS; j++)
        {
            screen[i][j].color = screenColor;
        }
}

void DrawCanvas()
{
    for(int i = 0; i < PIXEL_ROWS; i++)
    {
        for(int j = 0; j < PIXEL_COLS; j++)
        {
            Pixel &pixel = screen[i][j];

            PIXEL_SHAPE.setFillColor(pixel.color);
            PIXEL_SHAPE.setPosition({(float)j * PIXEL_SIZE, (float)i * PIXEL_SIZE});

            window.draw(PIXEL_SHAPE);
        }
    }
}

struct Point
{
    Vector2f position;
    Color color;
};

std::vector<Point> point_positions;

Vector2f last_mouse_position;

Vector2f brush_draw_points[9] = {
    {0, 0}, {-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}
};

void Start()
{   
    PIXEL_SHAPE = RectangleShape({1.0f, 1.0f});
    PIXEL_SHAPE.setSize({PIXEL_SIZE, PIXEL_SIZE});
    screenColor = Color::Black;
}

void Update()
{
    if(Mouse::isButtonPressed(Mouse::Button::Left))
    {
        if(canvasMousePosition.x > 0 && canvasMousePosition.x < window_width &&
            canvasMousePosition.y > 0 && canvasMousePosition.y < window_height)
        {
            if(last_mouse_position != canvasMousePosition)
            {
                float x_to_screen = canvasMousePosition.x / PIXEL_SIZE; 
                float y_to_screen = canvasMousePosition.y / PIXEL_SIZE;

                for(int i = 0; i < 9; i++)
                {
                    float x = x_to_screen + brush_draw_points[i].x;
                    float y = y_to_screen + brush_draw_points[i].y;

                    if(screen[(int)y][(int)x].color == Color::White) continue;

                    if(x > 0 && x < window_width && y > 0 && y < window_height)
                    {
                        Point point
                        {
                            .position = {x, y}, 
                            .color = Color::White
                        };

                        point_positions.emplace_back(point);
                    }
                }

                last_mouse_position = canvasMousePosition;
            }
        }
    }
}

void Draw()
{
    window.setView(camera);

    window.setView(canvas);

    ClearCanvas();

    for(const auto &point : point_positions)
    {
        screen[(int)point.position.y][(int)point.position.x].color = point.color;
    }

    DrawCanvas();
}