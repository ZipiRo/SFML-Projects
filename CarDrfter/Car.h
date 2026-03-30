#include <array>

struct Car
{
    Vector2f position = Vector2f(0, 0);
    Vector2f velocity = Vector2f(0, 0);
    Vector2f force = Vector2f(0, 0);
    Vector2f right = Vector2f(1, 0);

    float mass = 1.0f;
    float motorForce = 0.0f;
    float angularVelocity = 0.0f;

    float angle = 0.0f;
    float turnAngle = 0.0f;
    float turnDirection = 0.0f;

    float speed = 0.0f;
    float maxSpeed = 1.0f;

    Texture texture;
    Sprite sprite{texture};

    std::array<std::vector<sf::Vector2f>, 2> whell_marks;

    void Step()
    {
        const Vector2f acceleration = force / mass;
        velocity += acceleration * deltaTime;

        speed = velocity.length();

        if (speed > 1.0f)
        {
            const float angularAcc = turnDirection * turnAngle;
            angularVelocity += angularAcc * deltaTime;

            angle += angularVelocity * deltaTime;
        }

        right = Vector2f(std::cos(angle), std::sin(angle));

        position += velocity * deltaTime;

        if(speed > maxSpeed)
            velocity = velocity.normalized() * maxSpeed;

        velocity *= 0.93f;
        angularVelocity *= 0.96f;

        force = Vector2f(0, 0);
        turnDirection = 0.0f;
    }

    void Turn(int steer)
    {
        turnDirection += static_cast<float>(steer);
    }

    void Accelerate()
    {
        force = right * motorForce * mass;
    }

    void Reverse()
    {
        force = -right * (motorForce * 0.5f) * mass;
    }

    void DrawCar()
    {
        sprite.setPosition(position);
        sprite.setRotation(sf::radians(angle));
        
        window.draw(sprite);
    }
};