#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "Template";

const int FPS = 60;
const float deltaTime = 1.0f / FPS;

const float PI = 3.1415926354f;

const sf::Color BackgroundColor = Color::Black;

View camera;
View canvas;

/// CODE FROM HERE

struct Light
{
    Vector2f position;
    Color color;
    float radius;
    float intensity;   // 0.0 – 1.0
    int iterations;

    void Draw()
    {
        float step = radius / iterations;

        for(int i = iterations; i > 0; --i)
        {
            float currentRadius = step * i;

            CircleShape shape(currentRadius);
            shape.setOrigin({currentRadius, currentRadius});
            shape.setPosition(position);

            float t = (float)i / iterations;
            float alpha = t * t * intensity * 255.f; // quadratic falloff

            Color c = color;
            c.a = alpha;

            shape.setFillColor(c);
            window.draw(shape);
        }
    }
};


Light A, B, C, D;

void Start()
{
    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    A = Light{Vector2f(0, 0), Color::Red, 30, 0.6f, 5};
    B = Light{Vector2f(0, 12), Color::Blue, 30, 0.2f, 5};
    C = Light{Vector2f(12, 0), Color::Green, 60, 0.6f, 5};
    D = Light{Vector2f(0, 66), Color::Cyan, 20, 0.6f, 5};

}

void Update()
{
    A.position.y += 1.1f;

}

void Draw()
{
    window.setView(camera);
    A.Draw();
    B.Draw();
    C.Draw();
    D.Draw();

    window.setView(canvas);
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