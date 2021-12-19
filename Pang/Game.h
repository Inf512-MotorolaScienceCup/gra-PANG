#pragma once

#include <vector>
#include "raylib.h"

class Sprite;

class Game {
public:
	const float screenWidth = 800;
	const float screenHeight = 450;
	const float wallThickness = 20;
	const float ballRadius = 40;

	std::vector<Sprite*> sprites;

	Game();
	~Game();
	void Spawn();
	void DrawSprites();
	void MoveSprites();
	static bool isCollision(const Sprite* s1, const Sprite* s2);
	Rectangle getPlayerPosition();
	void AddScore(int);
	int mainLoop();

	int score = 0;
};

