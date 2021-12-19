#pragma once

#include <iostream>

#include "Utils.h"
#include "Game.h"

class Sprite {
public:
	enum class Type {
		PLAYER,
		BLOCK,
		BALL,
		WEAPON
	} type;
	enum class State {
		ACTIVE,
		FINISHING,
		FINISHED
	} state;

	Position position;
	bool collision;
	Game* game;

	virtual void draw() = 0;
	virtual void move() {};
	virtual State getState() { return state; }
	explicit Sprite(Game* game, const Position& position, Type type)
		: game(game), position(position), type(type), collision(false), state(State::ACTIVE) {
	}

};

class Player : public Sprite {
public:
	Player(Game* game, float x, float y, float width, float height, Color color, float speed)
		: Sprite(game, Position(x, y, width, height), Type::PLAYER), color(color), speed(speed) {
		spriteSheet = LoadTexture("res/spritesheet.png");
		moveRightRec = { 0.0f, 11 * 64.0f, 64.0f, 64.0f };
		moveLeftRec = { 0.0f, 9 * 64.0f, 64.0f, 64.0f };
		standRec = { 0.0f, 2 * 64.0f, 64.0f, 64.0f };
	}
	virtual void draw();
	virtual void move();
	Sprite* checkCollision();

private:
	Color color;
	float speed;
	Texture2D spriteSheet;
	Rectangle moveRightRec;
	Rectangle moveLeftRec;
	Rectangle standRec;
	int cooldown = 0;
};

class Block : public Sprite {
public:
	Block(Game* game, float x, float y, float width, float height, Color color)
		: Sprite(game, Position(x, y, width, height), Type::BLOCK), color(color) {
	}
	virtual void draw();

private:
	Color color;
};

class Ball : public Sprite {
public:
	enum Kind {
		BALL1,
		BALL2,
		BALL3,
		BALL4
	};
	static Ball* create(Game* game, float x, float y, Kind kind, int heading) {
		if (!spriteSheet.id)
			spriteSheet = LoadTexture("res/Ball/Std/firefly.png");
		if (!spriteExplode.id)
			spriteExplode = LoadTexture("res/Ball/Explosion/firefly_explosion.png");
		switch (kind) {
		case BALL1:
			return new Ball(game, x, y, /*radius*/ 20, BALL1, heading);
		case BALL2:
			return new Ball(game, x, y, /*radius*/ 15, BALL2, heading);
		case BALL3:
			return new Ball(game, x, y, /*radius*/ 10, BALL3, heading);
		case BALL4:
			return new Ball(game, x, y, /*radius*/ 5, BALL4, heading);
		default: return 0;
		}
	}
	Ball(Game* game, float x, float y, float radius, Kind kind, int heading)
		: game(game), Sprite(game, Position(x, y, radius), Type::BALL), color(WHITE), sizeX(80.0f), sizeY(95.0f), kind(kind) {
		stand = { 0.0f, 0.0f, sizeX, sizeY };
		standExplode = { 0.0f, 0.0f, sizeX + 20, sizeY };
		speedX *= heading;
	}
	virtual void draw();
	virtual void move();

	void drawFinish();
	void duality();

	Sprite* checkCollision();

private:
	Game * game;
	static Texture2D spriteSheet;
	static Texture2D spriteExplode;
	Rectangle stand;
	Rectangle standExplode;
	Color color;
	float speedX = 5;
	float speedY = -15;
	int cooldown = 0;
	const float maxSpeedY = 20;
	float sizeX;
	float sizeY;
	Kind kind;
};

class Weapon : public Sprite {
public:
	Weapon(Game* game, float x, float y, float width, float height, Color color)
		: Sprite(game, Position(x, y, width, height), Type::WEAPON), color(color) {
		init();
	}
	void init();
	virtual void draw();
	virtual void move();

	Sprite* checkCollision();

private:
	Color color;
	bool active = false;
	const float speedY = 20;
	int cooldown = 0;
};


