#include "stdafx.h"

#include <algorithm>
#include <vector>
#include <iostream>

#include "raylib.h"
#include "raymath.h"

#include "Game.h"
#include "Sprite.h"

void Game::DrawSprites()
{
	for (size_t i = 0; i < sprites.size(); i++) {
		sprites[i]->draw();
	}
}

void Game::MoveSprites()
{
	for (size_t i = 0; i < sprites.size(); i++)
	{
		sprites[i]->move();
	}
	auto removed = remove_if(sprites.begin(), sprites.end(), [](Sprite* sprite) { return sprite->getState() == Sprite::State::FINISHED; });
	sprites.erase(removed, sprites.end());
}

bool Game::isCollision(const Sprite* s1, const Sprite* s2)
{
	bool collision = false;
	if (s1->position.type == Position::Type::RECTANGLE) {
		if (s2->position.type == Position::Type::RECTANGLE) {
			collision = CheckCollisionRecs(s1->position.rectangle, s2->position.rectangle);
		}
		else {
			collision = CheckCollisionCircleRec(s2->position.center, s2->position.radius, s1->position.rectangle);
		}
	}
	else {
		if (s2->position.type == Position::Type::RECTANGLE) {
			collision = CheckCollisionCircleRec(s1->position.center, s1->position.radius, s2->position.rectangle);
		}
		else {
			collision = CheckCollisionCircles(s1->position.center, s1->position.radius, s2->position.center, s2->position.radius);
		}
	}
	return collision;
}

Rectangle Game::getPlayerPosition()
{
	for (auto sprite : sprites) {
		if (sprite->type == Sprite::Type::PLAYER) {
			return sprite->position.rectangle;
		}
	}
	return { 0, 0, 0, 0 };
}

void Game::AddScore(int score) {
	this->score += score;
}

Game::Game() {

}

Game::~Game() {
	std::for_each(begin(sprites), end(sprites), [](Sprite* s) { delete s; });
}

void Game::Spawn() {
	sprites.push_back(new Block(this, 0, 0, screenWidth, wallThickness, LIGHTGRAY));
	sprites.push_back(new Block(this, 0, screenHeight - wallThickness, screenWidth, wallThickness, RED));
	sprites.push_back(new Block(this, 0, 0, wallThickness, screenHeight, GRAY));
	sprites.push_back(new Block(this, screenWidth - wallThickness, 0, wallThickness, screenHeight, YELLOW));
	sprites.push_back(new Player(this, 400, screenHeight - wallThickness - 64, 64, 64, BLACK, 0));
	sprites.push_back(Ball::create(this, 300, 200, Ball::Kind::BALL1, 1));
	sprites.push_back(new Weapon(this, 0, 0, 20, 0, PURPLE));
}

int Game::mainLoop() {
	// Initialization
	//---------------------------------------------------------

	InitWindow(screenWidth, screenHeight, "Pang");

	Spawn();

	bool pause = 0;
	int framesCounter = 0;

	SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

									// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update
		//-----------------------------------------------------
		if (IsKeyPressed(KEY_ENTER)) pause = !pause;

		if (!pause)
		{
			MoveSprites();
		}
		else framesCounter++;
		//-----------------------------------------------------

		// Draw
		BeginDrawing();

		ClearBackground(RAYWHITE);

		//DrawText("PRESS SPACE to PAUSE BALL MOVEMENT", 10, GetScreenHeight() - 25, 20, LIGHTGRAY);

		DrawSprites();

		// On pause, we draw a blinking message
		if (pause && ((framesCounter / 30) % 2)) DrawText("PAUSED", 350, 200, 30, GRAY);

		DrawFPS(10, 10);

		EndDrawing();
	}

	// De-Initialization
	//---------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//----------------------------------------------------------

	return 0;
}