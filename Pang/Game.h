#pragma once

#include <vector>
#include <map>

#include "raylib.h"

#include "Menu.h"
#include "Sprite.h"

class Game {
public:
    const float screenWidth = 1280;
    const float screenHeight = 720;
    const float wallThickness = 70;
    const float ballRadius = 40;

    Game();
    ~Game();

    void Spawn();
    void Draw();
    void Update();
    void DrawSprites();
    void MoveSprites();
    void CheckCollision();

    Rectangle getPlayerPosition();

    void AddSprite(float, float, Enemy::Kind, int);
    void AddScore(int);
    int MainLoop();
    void OnMenu(std::string name);

    std::vector<Sprite*> sprites;
    std::map<Sprite::Type, std::vector<Sprite*>> spriteMap;
    Player* player;
    Weapon* weapon;

    int score = 0;
    bool pause = false;
    bool quitSelected = false;
    int frameCounter = 0;
    Menu menu;
};

