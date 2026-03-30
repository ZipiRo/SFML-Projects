#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "CarDrfter";

const int FPS = 60;
const float deltaTime = 1.0f / FPS;

const float PI = 3.1415926354f;

#include "Car.h"

const sf::Color BackgroundColor = Color(123, 123, 123);

View camera;
View canvas;

Texture car_texture;
Sprite car_sprite{car_texture};

Font font;

Text car_speed_text{font};

float angle = 0;
sf::Vector2f right;

Car car;

void Start()
{
    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    if(font.openFromFile("resources/JetBrainsMono-Regular.ttf"));
        car_speed_text.setFont(font);

    car_speed_text.setFillColor(Color::Black);

    if(car_texture.loadFromFile("resources/car_sprite.png"))
        car_sprite.setTexture(car_texture, true);

    FloatRect car_sprite_bounds = car_sprite.getLocalBounds();
    car_sprite.setOrigin({car_sprite_bounds.size.x * 0.5f, car_sprite_bounds.size.y * 0.5f});
    car_sprite.setScale({2, 2});

    car.sprite = car_sprite;
    car.motorForce = 2000;
    car.mass = 1000;
    car.turnAngle = 3 * PI;
    car.maxSpeed = 1000;
}

void Update()
{
    car_speed_text.setString("Speed: " + std::to_string((int)car.speed) + "KM/H");

    if(Keyboard::isKeyPressed(Keyboard::Key::W))
        car.Accelerate();

    if(Keyboard::isKeyPressed(Keyboard::Key::S))
        car.Reverse();

    if(Keyboard::isKeyPressed(Keyboard::Key::A))
        car.Turn(-1);

    if(Keyboard::isKeyPressed(Keyboard::Key::D))
        car.Turn(1);

    car.Step();
}

void Draw()
{
    window.setView(camera);
    car.DrawCar();
    window.draw(car.sprite);

    window.setView(canvas);
    window.draw(car_speed_text);
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