#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <cstdlib>

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const std::string ASSET_MIDLINE = "assets/midline.png";
const std::string ASSET_FONT = "assets/super_retro_m54.ttf";
const std::string ASSET_DEAD_SOUND = "assets/dead.wav";
const std::string ASSET_PADDLE_SOUND = "assets/paddle.wav";
sf::Clock deltaClock;
sf::Time deltaTime;
sf::Font font;
sf::SoundBuffer deadBuffer;
sf::SoundBuffer paddleBuffer;
sf::Sound deadSound;
sf::Sound paddleSound;

class Paddle
{
public:
    Paddle(bool right);
    void Draw(sf::RenderWindow& wnd);
    void Update(float dt);
    sf::Vector2f GetPos();
    sf::RectangleShape GetHitbox();
    float GetWidth();
    float GetHeight();
    int GetScore();
    void IncrementScore();
private:
    float w = 10.0f, h = 75.0f;
    int score = 0;
    bool right = false;
    sf::Vector2f pos { 0, (WINDOW_HEIGHT / 2) - (h / 2) };
    sf::RectangleShape shape { sf::Vector2f(w, h) };
    sf::RectangleShape hitbox { sf::Vector2f(w - 2, h) };
};

class MidLine
{
public:
    MidLine();
    void Draw(sf::RenderWindow& wnd);
    sf::Vector2f GetPosition();
private:
    sf::Texture tex;
    sf::Sprite spr;
};

class Ball
{
public:
    void Draw(sf::RenderWindow& wnd);
    void Update(float dt, Paddle& paddle1, Paddle& paddle2, sf::Sound& deadSound, sf::Sound& paddleSound);
    sf::RectangleShape GetHitbox();
private:
    void CheckForCollision(float dt, Paddle& paddle1, Paddle& paddle2, sf::Sound& deadSound, sf::Sound& paddleSound);

    float r = 6.0f;
    float speed = 200.0f;
    bool right = true, up = true;
    sf::Vector2f vel { 0, 0 };
    sf::Vector2f pos { WINDOW_WIDTH / 2 - r, WINDOW_HEIGHT / 2 - r };
    sf::CircleShape shape { r };
    sf::RectangleShape hitbox { sf::Vector2f(r + 1.0f, r + 1.0f) };
};

Paddle::Paddle(bool right)
{
    this->right = right;

    if(this->right)
    {
        pos.x = WINDOW_WIDTH - w*2;
    }
    else
    {
        pos.x = w;
    }

    this->shape.setPosition(this->pos);
}

void Paddle::Draw(sf::RenderWindow& wnd)
{
    wnd.draw(shape);
}

void Paddle::Update(float dt)
{
    shape.setPosition(pos);
    hitbox.setPosition(pos);

    if(this->right)
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            pos.y -= 300.0f * dt;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            pos.y += 300.0f * dt;
        }
    }
    else if(!this->right)
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            pos.y -= 300.0f * dt;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            pos.y += 300.0f * dt;
        }
    }

    if(pos.y > WINDOW_HEIGHT - h)
    {
        pos.y = WINDOW_HEIGHT - h;
    }
    else if(pos.y < 0)
    {
        pos.y = 0;
    }
}

sf::Vector2f Paddle::GetPos()
{
    return pos;
}

sf::RectangleShape Paddle::GetHitbox()
{
    return hitbox;
}

float Paddle::GetWidth()
{
    return w;
}

float Paddle::GetHeight()
{
    return h;
}

int Paddle::GetScore()
{
    return score;
}

void Paddle::IncrementScore()
{
    score++;
}

MidLine::MidLine()
{
    tex.setRepeated(true);
    tex.loadFromFile(ASSET_MIDLINE);
    spr.setTexture(tex);
    spr.setTextureRect(sf::IntRect(WINDOW_WIDTH / 2 - 2, 0, 5, WINDOW_HEIGHT));
    spr.setPosition(WINDOW_WIDTH / 2 - 2, 0);
}

void MidLine::Draw(sf::RenderWindow& wnd)
{
    wnd.draw(spr);
}

sf::Vector2f MidLine::GetPosition()
{
    return spr.getPosition();
}

void Ball::Draw(sf::RenderWindow& wnd)
{
    shape.setPosition(pos);
    wnd.draw(shape);
}

sf::RectangleShape Ball::GetHitbox()
{
    return hitbox;
}

void Ball::CheckForCollision(float dt, Paddle& paddle1, Paddle& paddle2, sf::Sound& deadSound, sf::Sound& paddleSound)
{
    if(hitbox.getGlobalBounds().intersects(paddle1.GetHitbox().getGlobalBounds()))
    {
        right = false;
        speed += 10.0f;
        paddleSound.play();
    }

    if(hitbox.getGlobalBounds().intersects(paddle2.GetHitbox().getGlobalBounds()))
    {
        right = true;
        speed += 10.0f;
        paddleSound.play();
    }

    if(right)
    {
        vel.x = 1 * speed * dt;
    }
    else
    {
        vel.x = -1 * speed * dt;
    }

    // Top and bottom
    if(pos.y < r)
    {
        vel.y = 1 * speed * dt;
        up = false;
        paddleSound.play();
    }
    else if(pos.y > WINDOW_HEIGHT - r*2)
    {
        vel.y = -1 * speed * dt;
        up = true;
        paddleSound.play();
    }
    else if(up)
    {
        vel.y = -1 * speed * dt;
    }

    // Right and left
    if(pos.x > WINDOW_WIDTH - r || pos.x < r)
    {
        deadSound.play();
        if(pos.x > WINDOW_WIDTH - r)
        {
            paddle2.IncrementScore();
        }
        else
        {
            paddle1.IncrementScore();
        }

        pos.x = WINDOW_WIDTH / 2 - r;
        pos.y = WINDOW_HEIGHT / 2 - r;

        // Randomly decide beginning side
        int random = (rand() % (4 - 1 + 1)) + 1;
        std::cout << random << std::endl;
        if(random % 2 == 0)
        {
            vel.x = 1 * speed * dt;
            right = true;
        }
        else
        {
            vel.x = -1 * speed * dt;
            right = false;
        }
    }
}

void Ball::Update(float dt, Paddle& paddle1, Paddle& paddle2, sf::Sound& deadSound, sf::Sound& paddleSound)
{
    hitbox.setPosition(pos);
    CheckForCollision(dt, paddle1, paddle2, deadSound, paddleSound);

    pos += vel;
}

// true for right, false for left
Paddle paddle1 { true };
Paddle paddle2 { false };
Ball ball { };
MidLine midLine { };
sf::Text scoreL;
sf::Text scoreR;


void Draw(sf::RenderWindow& wnd)
{
    wnd.clear();
    scoreR.setString(std::to_string(paddle1.GetScore()));
    scoreL.setString(std::to_string(paddle2.GetScore()));
    wnd.draw(scoreR);
    wnd.draw(scoreL);
    paddle1.Draw(wnd);
    paddle2.Draw(wnd);
    midLine.Draw(wnd);
    ball.Draw(wnd);

    wnd.display();
}

void Update(float dt)
{
    paddle1.Update(dt);
    paddle2.Update(dt);
    ball.Update(dt, paddle1, paddle2, deadSound, paddleSound);
}

void GameLoop(sf::RenderWindow& wnd)
{

    while(wnd.isOpen())
    {
        sf::Event ev;

        while(wnd.pollEvent(ev))
        {
            if(ev.type == sf::Event::Closed)
            {
                wnd.close();
            }
        }

        Update(deltaTime.asSeconds());
        Draw(wnd);

        // update delta time
        deltaTime = deltaClock.restart();
    }
}

int main()
{
    font.loadFromFile(ASSET_FONT);

    scoreL.setFont(font);
    scoreR.setFont(font);
    scoreL.setCharacterSize(24);
    scoreR.setCharacterSize(24);
    scoreL.setPosition(sf::Vector2f(midLine.GetPosition().x - 72, WINDOW_HEIGHT - 36));
    scoreR.setPosition(sf::Vector2f(midLine.GetPosition().x + 48, WINDOW_HEIGHT - 36));
    scoreL.setString("0");
    scoreR.setString("0");
    paddleBuffer.loadFromFile(ASSET_PADDLE_SOUND);
    deadBuffer.loadFromFile(ASSET_DEAD_SOUND);
    paddleSound.setBuffer(paddleBuffer);
    deadSound.setBuffer(deadBuffer);

    // Seed rng
    srand(time(0));

    sf::RenderWindow wnd(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pong - https://github.com/arencoskun");
    wnd.setFramerateLimit(120);

    GameLoop(wnd);

    return 0;
}
