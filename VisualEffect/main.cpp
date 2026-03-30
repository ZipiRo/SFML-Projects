#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "VisualEffect - SFML 3.0.2";

const int FPS = 60;
const float deltaTime = 1.0f / FPS;

const float PI = 3.1415926354f;

const sf::Color BackgroundColor = Color(123, 123, 123);

View camera;
View canvas;

RectangleShape rectangle;
int pixel_size = 20;
int pixel_rows = window_height / pixel_size;
int pixel_cols = window_width / pixel_size;

Color base_pixel_color = Color(10, 123, 210);

struct Box
{
    float left;
    float top;
    float right;
    float bottom;

    void Create(float x, float y, float width, float height)
    {
        left = x;
        top = y;
        right = left + width;
        bottom = top + height;
    }
};

bool IntersectBoxPoint(const Box &box, Vector2f point)
{
    return point.x > box.left && point.y > box.top &&
            point.x < box.right && point.y < box.bottom;
}

std::vector<Vector2f> GetPointsInsideCircle(int radius, float x, float y, int ray_count = 15)
{
    std::vector<Vector2f> points;

    float rez = (2 * PI) / ray_count;

    for(int r = 0; r < radius; r += pixel_size)
    {
        for(float ang = 0.0f; ang < 2.0f * PI; ang += rez)
        {
            Vector2f point(x + cos(ang) * r, y + sin(ang) * r);

            point.x = std::floor(point.x / pixel_size) * pixel_size;
            point.y = std::floor(point.y / pixel_size) * pixel_size;

            points.emplace_back(point);
        }
    }

    return points;
}

struct Pixel
{
    Vector2f position;
    Color color;
    Color base_color;
    float size;
};

std::vector<std::vector<Pixel>> pixels;

Vector2f mousePosition;

int circle_radius = 10 * pixel_size;

void Start()
{
    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    rectangle = RectangleShape({1.0f, 1.0f});
    // rectangle.setOrigin({0.5f, 0.5f});

    rectangle.setSize({float(pixel_size), float(pixel_size)});
    rectangle.setFillColor(base_pixel_color);
    
    pixels.resize(pixel_rows);
        for (auto& row : pixels)
            row.resize(pixel_cols);

    for(int i = 0; i < pixel_rows; i++)
        for(int j = 0; j < pixel_cols; j++)
        {
            float x = j * pixel_size; 
            float y = i * pixel_size;

            Pixel pixel;
            pixel.position = Vector2f(x, y);
            pixel.base_color = Color(i % 255, (120 - i + 255) % 255, 250);
            pixel.color = pixel.base_color;
            pixel.size = pixel_size;

            pixels[i][j] = pixel;
        }
}

void Update()
{
    mousePosition = window.mapPixelToCoords(Mouse::getPosition(window), canvas);

    for(int i = 0; i < pixel_rows; i++)
        for(int j = 0; j < pixel_cols; j++)
        {
            Pixel &pixel = pixels[i][j];
            pixel.color = pixel.base_color;
            pixel.size = pixel_size;
        }
    
    std::vector<Vector2f> points = GetPointsInsideCircle(circle_radius, mousePosition.x, mousePosition.y, 30);
    
}

void Draw()
{
    window.setView(camera);

    window.setView(canvas);

    for(int i = 0; i < pixel_rows; i++)
        for(int j = 0; j < pixel_cols; j++)
        {
            rectangle.setPosition(pixels[i][j].position);
            rectangle.setSize(Vector2f(pixels[i][j].size, pixels[i][j].size));
            rectangle.setFillColor(pixels[i][j].color);
            window.draw(rectangle);
        }
}

int main()
{
    window = RenderWindow(VideoMode({window_width, window_height}), window_title);
 
    Start();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }
 
        Update();

        window.clear(BackgroundColor);

        Draw();

        window.display();

        sleep(seconds(deltaTime));
    }
}