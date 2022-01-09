#pragma once

#include <iostream>

#include "Utils.h"
#include "raylib.h"

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
    Game* game;

    Sprite(Game* game, const Position& position, Type type);
    virtual ~Sprite() {}

    virtual void Draw() = 0;

    virtual void Move() {};
    virtual void Collision(Sprite* sprite) {};
};

bool IsCollision(const Sprite *s1, const Sprite *s2);

class Player : public Sprite {
public:
    Player(Game* game, Position position, Color color);
    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);

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
    bool collision;
    int zone = 10;
};

class Block : public Sprite {
public:
  enum class Type {
    WALL,
    PLATFORM_1,
    PLATFORM_2
  };

  Block(Game *game, float x, float y, float width, float height, Block::Type type);
  virtual void Draw();

private:
    Type type;
    Color color;
};

class Enemy : public Sprite {
public:
    enum Kind {
        BALL1,
        BALL2,
        BALL3,
        BALL4,

        NUM_KINDS
    };
    static Enemy* create(Game *game, float x, float y, Kind kind, int heading);

    Enemy(Game *game, float x, float y, float radius, Kind kind, int heading);

    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);

    void DrawFinish();
    void duality();

    Sprite* checkCollision();

private:
    Game * game;
    static Texture2D spriteSheet[NUM_KINDS];
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
    Weapon(Game* game, float x, float y, float width, float height, Color color);
    void init();
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
    Ladder(Game *game, float x, float y, int numElements, float distanceToGround);

    virtual void Draw();
    float distanceToGround;

private:
    Texture2D* texture;
    int numElements;
};
