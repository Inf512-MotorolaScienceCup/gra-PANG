#pragma once

#include <iostream>

#include "Utils.h"

class Game;

class Sprite {
public:
    enum class Type {
        PLAYER,
        BLOCK,
        ENEMY,
        WEAPON,
        LADDER
    } type;

    enum class Direction {
        NONE,
        UP,
        DOWN,
        LEFT,
        RIGHT
    } direction = Direction::NONE;

    enum class State {
        ACTIVE,
        FINISHING,
        FINISHED
    } state;

    Position position;
    bool collision;
    Game* game;

    explicit Sprite(Game* game, const Position& position, Type type)
        : game(game), position(position), type(type), collision(false), state(State::ACTIVE) {
    }
    virtual ~Sprite() {}
    virtual void Draw() = 0;
    virtual void Move(){};
    virtual void Collision(Sprite* sprite) {};
};

bool IsCollision(const Sprite *s1, const Sprite *s2);

class Player : public Sprite {
public:
    Player(Game* game, float x, float y, float width, float height, Color color, Vector2 speed)
        : Sprite(game, Position(x, y, width, height), Type::PLAYER), color(color), speed(speed) {
        spriteSheet = LoadTexture("res/spritesheet.png");
        moveRightRec = { 0.0f, 11 * 64.0f, 64.0f, 64.0f };
        moveLeftRec  = { 0.0f, 9 * 64.0f,  64.0f, 64.0f };
        standRec     = { 0.0f, 2 * 64.0f,  64.0f, 64.0f };
        moveUpRec    = { 0.0f, 8 * 64.0f,  64.0f, 64.0f };
        moveDownRec  = { 0.0f, 10 * 64.0f, 64.0f, 64.0f };
        hurtRec      = { 0.0f, 20 * 64.0f, 64.0f, 64.0f };
  }
    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);

    int weapon = 0;

private:
    Vector2 speed;
    Color color;
    Texture2D spriteSheet;
    Rectangle moveRightRec;
    Rectangle moveLeftRec;
    Rectangle standRec;
    Rectangle moveUpRec;
    Rectangle moveDownRec;
    Rectangle hurtRec;
    int cooldown = 0;
    bool climbing = false;
    int zone = 10;
    bool collision = false;
};

class Block : public Sprite {
public:
    Block(Game* game, float x, float y, float width, float height, Color color)
        : Sprite(game, Position(x, y, width, height), Type::BLOCK), color(color) {
    }
    virtual void Draw();

private:
    Color color;
};

class Enemy : public Sprite {
public:
    enum Kind {
        BALL1,
        BALL2,
        BALL3,
        BALL4
    };
    static Enemy* create(Game* game, float x, float y, Kind kind, int heading) {
        if (!spriteSheet.id)
            //spriteSheet = LoadTexture("res/Ball/Std/firefly.png");
            spriteSheet = LoadTexture("res/Ball/ball_0.png");
        if (!spriteExplode.id)
            spriteExplode = LoadTexture("res/Ball/Explosion/firefly_explosion.png");
        switch (kind) {
        case BALL1:
            return new Enemy(game, x, y, /*radius*/ 20, BALL1, heading);
        case BALL2:
            return new Enemy(game, x, y, /*radius*/ 15, BALL2, heading);
        case BALL3:
            return new Enemy(game, x, y, /*radius*/ 10, BALL3, heading);
        case BALL4:
            return new Enemy(game, x, y, /*radius*/ 5, BALL4, heading);
        default: return 0;
        }
    }
    Enemy(Game* game, float x, float y, float radius, Kind kind, int heading)
        : game(game), Sprite(game, Position(x, y, radius), Type::ENEMY), color(WHITE), sizeX(radius * 2), sizeY(radius * 2), kind(kind) {
        stand = { 0.0f, 0.0f, sizeX, sizeY };
        standExplode = { 0.0f, 0.0f, sizeX + 20, sizeY };
        rad = radius;
        switch (kind) {
        case BALL1:
            speed.x = 7;
            maxSpeedY = 21;
            break;
        case BALL2:
            speed.x = 6;
            maxSpeedY = 19;
            break;
        case BALL3:
            speed.x = 5;
            maxSpeedY = 17;
            break;
        case BALL4:
            speed.x = 4;
            maxSpeedY = 15;
            break;
        }
        speed.x *= heading;
    }
    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);

    void DrawFinish();
    void duality();

    Sprite* checkCollision();

private:
    Game * game;
    static Texture2D spriteSheet;
    static Texture2D spriteExplode;
    Rectangle stand;
    Rectangle standExplode;
    Color color;
    int rad;
    Vector2 speed;
    int cooldown = 0;
    float maxSpeedY;
    float sizeX;
    float sizeY;
    Kind kind;
    int frameNumber = 0;
};

class Weapon : public Sprite {
public:
    Weapon(Game* game, float x, float y, float width, float height, Color color)
        : Sprite(game, Position(x, y, width, height), Type::WEAPON), color(color) {
    }
    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);

private:
    Color color;
    const float speedY = 20;
    int cooldown = 0;
};

class Ladder : public Sprite {
public:
    Ladder(Game* game, float x, float y, float width, float height, Color color)
        : Sprite(game, Position(x, y, width, height), Type::LADDER), color(color) {
    }
    virtual void Draw();

private:
    Color color;
};

