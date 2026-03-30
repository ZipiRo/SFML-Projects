#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

using namespace sf;

RenderWindow window;
const int window_width = 1280;
const int window_height = 720;
const char *window_title = "WAVE GAME";

const int FPS = 60;
const float deltaTime = 1.0f / FPS;

const float PI = 3.1415926354f;

Font font_JetBrains;

const sf::Color BackgroundColor = Color(123, 123, 123);

View camera;
View canvas;

// CODE FROM HERE

struct Box
{
    float left, top, right, bottom;
};

struct Projectile
{
    Vector2f position;
    Vector2f direction;
    Vector2f size;
    Color color;
    float timer = 0.0f;
    float life_time;
    float speed = 0.0f;
    int damage;
    Box bounds;
};

struct Gun
{
    Vector2f offset;
    Vector2f firePoint;
    Vector2f size;
    Vector2f rightDirection;

    Color color;

    Projectile ammo_type;
    int magazine = 0;
    int fullMagazine;
    
    float reloadtime;
    float reloadtime_timer = 0.0f;

    float reloadrate;
    float reloadrate_timer = 0.0f;
    
    float firerate;
    float firerate_timer = 0.0f;

    float shoot_acceleration;
    bool automatic;
};

struct Enemy 
{
    Vector2f position;
    Vector2f direction;
    Vector2f size;
    int health;
    int speed;
    Box bounds;
};

std::vector<Projectile> projectiles;
std::vector<Enemy> enemys;

RectangleShape player_rect;
Vector2f player_position;
float player_speed = 200.f;
int facingRight = 1;
bool gun_fired = false;
bool gun_reloading = false;
Gun currentGun;

RectangleShape gun_rect;
int bullets = 100;
Gun gun;
Gun auto_gun;

RectangleShape enemy_B_rect;
Enemy enemy_B;
float spawnRadius = 500.f;

CircleShape bullet_circle;
Projectile bullet;
Projectile msbullet;

Text gun_info_text{font_JetBrains};

Vector2i mousePosition;
Vector2f world_mousePosition;


bool IntersectBox(const Box &boxA, const Box &boxB)
{
    return  boxA.right >= boxB.left && boxA.left <= boxB.right &&
            boxA.bottom >= boxB.top && boxA.top <= boxB.bottom;
}

void Start()
{
    srand(time(0));

    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    if(font_JetBrains.openFromFile("resources/JetBrainsMono-Regular.ttf"))
    {
        gun_info_text.setFont(font_JetBrains);
        gun_info_text.setFillColor(Color::Black);
        gun_info_text.setCharacterSize(15);
    }

    player_rect = RectangleShape({20, 20});
    player_rect.setFillColor(Color::Red);
    player_rect.setOrigin({20 * 0.5f, 20 * 0.5f});

    /// BULLET
    bullet.life_time = 1.5f;
    bullet.size = Vector2f(3, 3);
    bullet.color = Color::Yellow;
    bullet.damage = 1;
    ///

    /// MSBULLET
    msbullet.life_time = 1.5f;
    msbullet.size = Vector2f(3, 3);
    msbullet.color = Color(123, 43, 223);
    msbullet.damage = 3;
    ///

    /// GUN 
    gun.size = Vector2f(20, 5);
    gun.color = Color::Black;
    gun.ammo_type = bullet;
    gun.fullMagazine = 16;
    gun.firerate = 0.5f;
    gun.reloadtime = 0.4f;
    gun.reloadrate = 0.1f;
    gun.shoot_acceleration = 350.0f;
    gun.offset = Vector2f(3, 0);
    gun.automatic = true;
    ///

    /// AUTO GUN
    auto_gun.size = Vector2f(30, 6);
    auto_gun.color = Color(193, 123, 123);
    auto_gun.ammo_type = msbullet;
    auto_gun.fullMagazine = 30;
    auto_gun.firerate = 0.2f;
    auto_gun.reloadtime = 0.45f;
    auto_gun.reloadrate = 0.14f;
    auto_gun.shoot_acceleration = 450.0f;
    auto_gun.offset = Vector2f(3, 0);
    auto_gun.automatic = true;
    ///

    /// ENEMY B
    enemy_B.speed = 70.f;
    enemy_B.health = 3;
    enemy_B.size = Vector2f(20, 20);

    enemy_B_rect = RectangleShape({enemy_B.size.x, enemy_B.size.y});
    enemy_B_rect.setFillColor(Color::Green);
    enemy_B_rect.setOrigin({enemy_B.size.x * 0.5f, enemy_B.size.y * 0.5f});
    ///

    /// ADD ENEMYES IN WORLD
    for(int i = 0; i < 5; i++)
    {
        float randAngle = rand() % 360 + 1;
        randAngle *= PI / 180;

        Vector2f randPosition = Vector2f(cos(randAngle), sin(randAngle)) * spawnRadius;

        Enemy new_enemy = enemy_B;
        new_enemy.position = randPosition;
        enemys.push_back(new_enemy);
    }
    ///

    currentGun = auto_gun;

    gun_rect = RectangleShape(currentGun.size);
    gun_rect.setFillColor(currentGun.color);
    gun_rect.setOrigin({0, currentGun.size.y * 0.5f});
    gun_rect.setPosition(player_position + currentGun.offset);

    bullet_circle = CircleShape(currentGun.ammo_type.size.x);
    bullet_circle.setFillColor(currentGun.ammo_type.color);
    bullet_circle.setOrigin({currentGun.ammo_type.size.x, currentGun.ammo_type.size.y});
}

void Update()
{
    mousePosition = Mouse::getPosition(window);
    world_mousePosition = window.mapPixelToCoords(mousePosition, camera);

    /// PLAYER MOVEMANT
    Vector2f direction;

    if(Keyboard::isKeyPressed(Keyboard::Key::W))
        direction += Vector2f(0, -1);

    if(Keyboard::isKeyPressed(Keyboard::Key::S))
        direction += Vector2f(0, 1);

    if(Keyboard::isKeyPressed(Keyboard::Key::A))
    {
        direction += Vector2f(-1, 0);
        facingRight = -1;
    }
    
    if(Keyboard::isKeyPressed(Keyboard::Key::D))
    {
        direction += Vector2f(1, 0);
        facingRight = 1;
    }

    if(direction.x != 0 || direction.y != 0)
    {
        player_position += direction.normalized() * player_speed * deltaTime;
        camera.setCenter(player_position);

        player_rect.setPosition(player_position);
        gun_rect.setPosition(player_position + Vector2f(currentGun.offset.x * facingRight, 0));
    }
    ///

    Vector2f pointToMouse = world_mousePosition - player_position;
    float gunAngle = std::atan2(pointToMouse.y, pointToMouse.x);
    currentGun.rightDirection = Vector2f(cos(gunAngle), sin(gunAngle));
    gun_rect.setRotation(radians(gunAngle));

    if(currentGun.firerate_timer >= 0)
        currentGun.firerate_timer -= deltaTime;

    if(currentGun.reloadrate_timer >= 0)
        currentGun.reloadrate_timer -= deltaTime;


    /// RELOAD CURRENT GUN    
    if(Keyboard::isKeyPressed(Keyboard::Key::R) && 
        bullets > 0 && currentGun.reloadrate_timer <= 0 && !gun_reloading)
    {
        gun_reloading = true;
        currentGun.reloadtime_timer = currentGun.reloadtime;
    }

    if(gun_reloading)
    {
        if(currentGun.reloadtime_timer >= 0)
            currentGun.reloadtime_timer -= deltaTime;

        if(currentGun.reloadtime_timer <= 0)
        {
            int reload_amount = currentGun.fullMagazine;

            if(bullets < currentGun.fullMagazine)
                reload_amount = bullets;
            
            currentGun.magazine = reload_amount;
            bullets -= reload_amount;

            currentGun.reloadrate_timer = currentGun.reloadrate;

            gun_reloading = false;
        }
    }
    ///

    /// FIRE CURRENT GUN
    if(Mouse::isButtonPressed(Mouse::Button::Left) && 
        !gun_fired && currentGun.magazine > 0 && currentGun.firerate_timer <= 0)
    {
        currentGun.magazine--;
        currentGun.firerate_timer = currentGun.firerate;

        currentGun.firePoint = player_position + Vector2f((currentGun.size.x - currentGun.offset.x) * currentGun.rightDirection.x, 
                                                        (currentGun.size.x - currentGun.offset.y) * currentGun.rightDirection.y);

        Projectile new_projectile = currentGun.ammo_type;
        new_projectile.speed = currentGun.shoot_acceleration;
        new_projectile.position = currentGun.firePoint;
        new_projectile.direction = currentGun.rightDirection;

        projectiles.push_back(new_projectile);
        
        if(!currentGun.automatic)
            gun_fired = true;
    }

    if(!Mouse::isButtonPressed(Mouse::Button::Left)) 
    {
        gun_fired = false;
    }
    ///

    gun_info_text.setString("Ammo:" + std::to_string(currentGun.magazine) + "/" + std::to_string(bullets) + 
                            "\nFireRate Timer:" + std::to_string(currentGun.firerate_timer) +
                            "\nRealodRate Timer:" + std::to_string(currentGun.reloadrate_timer) +
                            "\nReloadTime Timer:" + std::to_string(currentGun.reloadtime_timer));

    /// PROJECTILE CONTROLER
    for(int i = 0; i < projectiles.size(); i++)
    {
        auto &projectile = projectiles[i];
     
        projectile.position += projectile.direction * projectile.speed * deltaTime;
        projectile.timer += deltaTime;

        projectile.bounds = Box{
            projectile.position.x - projectile.size.x * 0.5f,
            projectile.position.y - projectile.size.y * 0.5f,
            projectile.position.x + projectile.size.x * 0.5f,
            projectile.position.y + projectile.size.y * 0.5f
        };
        
        if(projectile.timer >= projectile.life_time)
        {
            projectiles[i] = projectiles.back();
            projectiles.erase(projectiles.end());   
        }
    }
    ///

    /// ENEMYS CONTROLER
    for(auto it = enemys.begin(); it != enemys.end();)
    {
        Enemy &enemy = *it;

        bool dead = false;

        if(enemy.health <= 0)
        {
            dead = true;
            it = enemys.erase(it);
        }

        if(!dead)
        {
            Vector2f enemyToPlayer = player_position - enemy.position;
    
            if(enemyToPlayer.length() > 10.f)
            {
                enemy.direction = enemyToPlayer.normalized();
        
                enemy.position += enemy.direction * (float)enemy.speed * deltaTime;    
                
                enemy.bounds = Box{
                    enemy.position.x - enemy.size.x * 0.5f,
                    enemy.position.y - enemy.size.y * 0.5f,
                    enemy.position.x + enemy.size.x * 0.5f,
                    enemy.position.y + enemy.size.y * 0.5f
                };
            }
            ++it;
        }
    }
    ///

    for(auto &enemyA : enemys)
    {
        for(auto &enemyB : enemys)
        {
            if(&enemyA == &enemy_B) continue;

            if(IntersectBox(enemyA.bounds, enemy_B.bounds))
            {

            }
        }
    }

    /// CHECK PROJECTILE ENEMY COLLISION
    for(auto it = projectiles.begin(); it != projectiles.end();)
    { 
        bool destroyed = false;

        for(auto &enemy : enemys)
        {
            if(IntersectBox(enemy.bounds, it->bounds))
            {
                it = projectiles.erase(it);
                enemy.health -= it->damage;
                destroyed = true;
                break;
            }
        }

        if(!destroyed)
            ++it;
    }
    ///
}

void Draw()
{
    window.setView(camera);

    window.draw(player_rect);

    for(auto &enemy : enemys)
    {
        enemy_B_rect.setPosition(enemy.position);
        window.draw(enemy_B_rect);
    }

    for(auto &projectile : projectiles)
    {
        bullet_circle.setPosition(projectile.position);
        window.draw(bullet_circle);
    } 

    window.draw(gun_rect);

    window.setView(canvas);

    window.draw(gun_info_text);
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
        window.setView(camera);

        sleep(seconds(deltaTime));
    }
}