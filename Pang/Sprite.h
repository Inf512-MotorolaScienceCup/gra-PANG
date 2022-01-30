#pragma once

#include <iostream>
#include <ctime>
#include <fstream>

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
        LADDER,
        POWERUP,
        ICE
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

    virtual void Move() {}
    virtual void Collision(Sprite* sprite) {}
    virtual void Save(std::ofstream&) {}
};

bool IsCollision(const Sprite *s1, const Sprite *s2);

class Player : public Sprite {
public:
    Player(Game* game, Position position);
    Player(Game* game, std::ifstream&);
    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);
    virtual void Save(std::ofstream&);
    void Shooting();
    bool checkTime();
    time_t hitBall;

private:
    Vector2 speed;
    Color color = WHITE;
    Texture2D spriteSheet;
    Rectangle moveRightRec;
    Rectangle moveLeftRec;
    Rectangle standRec;
    Rectangle moveUpRec;
    Rectangle hurtRec;
    int cooldown = 0;
    bool climbing = false;
    bool collision;
    //int zone = 10;
    bool onIce = false;
    short int changeColor = 0;
};

class Block : public Sprite {
public:
    enum class Kind {
        WALL,
        PLATFORM_1,
        PLATFORM_2
    };

    Block(Game *game, float x, float y, float width, float height, Kind type);
    Block(Game* game, std::ifstream&);
    virtual void Draw();
    virtual void Collision(Sprite*);
    virtual void Save(std::ofstream&);
    short int health = 2;

private:
    Texture2D* texture;
    Kind kind;
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
    Enemy(Game* game, std::ifstream&);

    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);
    virtual void Save(std::ofstream&);

    void DrawFinish();
    void duality(Sprite*);

    Sprite* checkCollision();

private:
    Game * game;
    static Texture2D spriteSheet[NUM_KINDS];
    static Texture2D spriteExplode[NUM_KINDS];
    Rectangle stand;
    Rectangle standExplode;
    Color color;
    //int rad;
    Vector2 speed;
    int cooldown = 0;
    float maxSpeedY;
    float gravity = 0.3;
    float sizeX;
    float sizeY;
    Kind kind;
    int frameNumber = 0;
    bool collision = false;
};

class Weapon : public Sprite {
public:
    enum Kind {
        WEAPON1,
        WEAPON2,
        WEAPON3,
        WEAPON4
    };

    Weapon(Game* game, float x, float y, Kind kind);
    Weapon(Game* game, std::ifstream&);
    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);
    virtual void Save(std::ofstream&);
    void checkTime();

private:
    Kind kind;
    Texture2D * texture[2] = { nullptr , nullptr };
    Rectangle moveTexture;
    Color color = WHITE;
    int numElements;
    const float speedY = 21;
    int cooldown = 0;
    bool stopMoving;
    int overlap;
    std::time_t lifeTime;
    Sprite* block;
};

class Ladder : public Sprite {
public:
    Ladder(Game *game, float x, float y, int numElements, float distanceToGround);
    Ladder(Game* game, std::ifstream&);
    virtual void Draw();
    virtual void Save(std::ofstream&);
    float distanceToGround;

private:
    Texture2D* texture;
    int numElements;
};

class Powerup : public Sprite {
public:
    enum Kind {
        BOOST,
        DOUBLE,
        HEAL,
        TIME,
        WEAPON
    };

    Powerup(Game *game, float x, float y, Kind kind);
    Powerup(Game* game, std::ifstream&);
    virtual void Draw();
    virtual void Move();
    virtual void Collision(Sprite*);
    virtual void Save(std::ofstream&);

private:
    Texture2D* texture;
    Kind kind;
    float speedY;
};

class Ice : public Sprite {
public:
    Ice(Game *game, float x, float y, float width, float height);
    Ice(Game* game, std::ifstream&);
    virtual void Draw();
    virtual void Save(std::ofstream&);

private:
    Texture2D* texture;
    Rectangle textRec;
};
