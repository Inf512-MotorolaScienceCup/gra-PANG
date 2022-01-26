#include <algorithm>
#include <vector>
#include <iostream>

#include "raylib.h"
#include "raymath.h"

#include "Game.h"
#include "Sprite.h"
#include "Menu.h"

void Game::DrawSprites() {
    for (size_t i = 0; i < sprites.size(); i++) {
        sprites[i]->Draw();
    }
}

void Game::MoveSprites() {
    for (size_t i = 0; i < sprites.size(); i++) {
        sprites[i]->Move();
    }
    auto removed = remove_if(sprites.begin(), sprites.end(), [](Sprite* sprite) { return sprite->state == Sprite::State::FINISHED; });
    std::for_each(removed, sprites.end(), [](Sprite* sprite) { std::cout << "REMOVE:" << (int)sprite->type << " ptr:"<< sprite << std::endl; });
    sprites.erase(removed, sprites.end());
}

void Game::CheckCollision() {
    // Collision Player<->Block
    for (auto& block : GetSprites(Sprite::Type::BLOCK)) {
        if (IsCollision(player, block)) {
            // std::cout << "PLAYER collided with BLOCK:" << std::endl;
            player->Collision(block);
        }
    }

    // Collision Player<->Enemy
    for (auto& enemy : GetSprites(Sprite::Type::ENEMY)) {
        if (IsCollision(player, enemy)) {
        // std::cout << "PLAYER collided with ENEMY:" << std::endl;
            if (!player->hitBall)
                player->Collision(enemy);
        }
    }

    // Collision Player<->Ladder
    for (auto& ladder : GetSprites(Sprite::Type::LADDER)) {
        if (IsCollision(player, ladder)) {
            // std::cout << "PLAYER collided with LADDER:" << std::endl;
            player->Collision(ladder);
        }
    }

    // Collision Player<->Powerup
    for (auto& powerup : GetSprites(Sprite::Type::POWERUP)) {
        if (IsCollision(player, powerup)) {
            // std::cout << "PLAYER collided with LADDER:" << std::endl;
            player->Collision(powerup);
            powerup->Collision(player);
        }
    }

    // Collision Player<->Ice
    for (auto& ice : GetSprites(Sprite::Type::ICE)) {
        if (IsCollision(player, ice)) {
            // std::cout << "PLAYER collided with LADDER:" << std::endl;
            player->Collision(ice);
        }
    }

    // Collision Weapon<->Enemy
    for (auto& weapon : GetSprites(Sprite::Type::WEAPON)) {
        for (auto& enemy : GetSprites(Sprite::Type::ENEMY)) {
            if (IsCollision(weapon, enemy)) {
                std::cout << "WEAPON collided with ENEMY:" << std::endl;
                weapon->Collision(enemy);
                enemy->Collision(weapon);
                auto& v = spriteMap[Sprite::Type::ENEMY];
                size_t numEnemies = 0;
                for_each(v.begin(), v.end(), [&](Sprite* sprite) { if (sprite->type == Sprite::Type::ENEMY && sprite->state == Sprite::State::ACTIVE) numEnemies += 1; });
                // std::cout << "Enemy Left:" << spriteMap[Sprite::Type::ENEMY].size() << " / " << numEnemies << std::endl;
                if (numEnemies <= 0) {
                    ChangeState(State::LEVEL_FINISHED);
                }
            }
        }
    }

    // Collision Weapon<->Block
    for (auto& weapon : GetSprites(Sprite::Type::WEAPON)) {
        for (auto& block : GetSprites(Sprite::Type::BLOCK)) {
            if (IsCollision(weapon, block)) {
                block->Collision(weapon);
                weapon->Collision(block);
                break;
            }
        }
    }

    // Collision Enemy<->Block
    for (auto& enemy : GetSprites(Sprite::Type::ENEMY)) {
        for (auto& block : GetSprites(Sprite::Type::BLOCK)) {
            if (IsCollision(enemy, block)) {
                enemy->Collision(block);
            }
        }
    }

    // Collision Powerup<->Block
    for (auto& powerup : GetSprites(Sprite::Type::POWERUP)) {
        for (auto& block : GetSprites(Sprite::Type::BLOCK)) {
            if (IsCollision(powerup, block)) {
                powerup->Collision(block);
            }
        }
    }
}

void Game::AddEnemy(float x, float y, Enemy::Kind kind, int heading) {
    Sprite* s = Enemy::create(this, x, y, kind, heading);
    spriteMap[Sprite::Type::ENEMY].push_back(s);
    sprites.push_back(s);
}

void Game::AddWeapon(float x, float y, int type) {
    switch (type) {
    case 1:
        weapon = new Weapon(this, x, y, Weapon::Kind::WEAPON1);
        spriteMap[Sprite::Type::WEAPON].push_back(weapon);
        sprites.push_back(weapon);
        break;
    case 2:
        weapon = new Weapon(this, x, y, Weapon::Kind::WEAPON2);
        spriteMap[Sprite::Type::WEAPON].push_back(weapon);
        sprites.push_back(weapon);
        break;
    case 3:
        weapon = new Weapon(this, x, y, Weapon::Kind::WEAPON3);
        spriteMap[Sprite::Type::WEAPON].push_back(weapon);
        sprites.push_back(weapon);
        break;
    case 4:
        weapon = new Weapon(this, x, y, Weapon::Kind::WEAPON4);
        spriteMap[Sprite::Type::WEAPON].push_back(weapon);
        sprites.push_back(weapon);
        break;
    }
}

void Game::AddPowerup(float x, float y) {
    int chance = GetRandomValue(1, 3);
    if (chance == 1) {
        int kindNum;
        if (weaponType == 2)
            kindNum = GetRandomValue(2, 4);
        else
            kindNum = GetRandomValue(1, 4);
            
        Sprite* s;
        switch (kindNum) {
        case 1:
            s = new Powerup(this, x, y, Powerup::Kind::DOUBLE);
            spriteMap[Sprite::Type::POWERUP].push_back(s);
            sprites.push_back(s);
            break;
        case 2:
            s = new Powerup(this, x, y, Powerup::Kind::BOOST);
            spriteMap[Sprite::Type::POWERUP].push_back(s);
            sprites.push_back(s);
            break;
        case 3:
            s = new Powerup(this, x, y, Powerup::Kind::TIME);
            spriteMap[Sprite::Type::POWERUP].push_back(s);
            sprites.push_back(s);
            break;
        case 4:
            s = new Powerup(this, x, y, Powerup::Kind::WEAPON);
            spriteMap[Sprite::Type::POWERUP].push_back(s);
            sprites.push_back(s);
            break;
        }
    }
}

void Game::PickAction(Powerup::Kind kind) {
    switch (kind) {
    case Powerup::Kind::BOOST:
        speedBoost = 2;
        timeLeft[0] = std::time(nullptr);
        break;
    case Powerup::Kind::DOUBLE:
        if (weaponType == 4)
            shootingLeft *= 2;
        else {
            multiWeapon = 1;
            timeLeft[1] = std::time(nullptr);
        }
        break;
    case Powerup::Kind::HEAL:
        lives++;
        break;
    case Powerup::Kind::TIME:
        stopTime = true;
        timeLeft[2] = std::time(nullptr);
        break;
    case Powerup::Kind::WEAPON:
        int change = GetRandomValue(2, 4);
        if (change == weaponType)
            weaponType = 1;
        else
            weaponType = change;

        if (weaponType == 4)
            shootingLeft = 5;
        break;
    }
}

void Game::CheckTime() {
    std::time_t now = std::time(nullptr);
    for (int i = 0; i < 4; i++) {
        if (timeLeft[i] + 5 <= now) {
            switch (i) {
            case 0:
                speedBoost = 1;
                timeLeft[i] = 0;
                break;
            case 1:
                multiWeapon = 0;
                timeLeft[i] = 0;
                break;
            case 2:
                stopTime = false;
                timeLeft[i] = 0;
                break;
            case 3:
                break;
            default:
                break;
            }
        }
    }
}

void Game::AddScore(int score) {
    this->score += score;
}

Game::Game()
    : state(State::MAIN_MENU),
      mainMenu(this, {"Start", "Load", "Level", "Quit"}),
      ingameMenu(this, {"Continue", "Restart", "Back to menu", "Quit"}) {
    InitWindow(screenWidth, screenHeight, "Pang");
    SetExitKey(KEY_F10);
    SetTargetFPS(60);
    LoadTextures();
}

Game::~Game() {
    Unspawn();
    CloseWindow(); // Close window and OpenGL context
}

void Game::Spawn(Sprite* sprite) {
    sprites.push_back(sprite);
    spriteMap[sprite->type].push_back(sprite);
}

void Game::SpawnLevel() {
    switch (level) {
        case 1:
            Spawn(new Block(this, 200, 400, 150, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 450, 500, 150, wallThickness, Block::Kind::PLATFORM_2));

            // y value must always be 5-10 points lower than block value
            // distanceToGround must be properly calculated
            Spawn(new Ladder(this, 200, 395, 4, 25));
            Spawn(new Powerup(this, 360, 600, Powerup::Kind::DOUBLE));
            
            Spawn(new Ice(this, 700, 690, 150, 10));

            Spawn(Enemy::create(this, 400, 200, Enemy::Kind::BALL1, 1));
            //Spawn(Enemy::create(this, 500, 400, Enemy::Kind::BALL1, 1));

            backTexture = BACKGROUND_ANI;
            levelTime = 60;
            break;
        case 2:
            Spawn(new Block(this, 100, 500, 150, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 500, 500, 150, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 200, 400, 150, wallThickness, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 700, 400, 150, wallThickness, Block::Kind::PLATFORM_2));

            Spawn(new Ladder(this, 200, 400, 4, 25));

            Spawn(Enemy::create(this, 400, 200, Enemy::Kind::BALL1, 1));
            Spawn(Enemy::create(this, 500, 400, Enemy::Kind::BALL1, 1));

            levelTime = 60;
            break;
        case 3:
            for (int x = 32; x > 0; x--) {
                Spawn(new Block(this, 20, 60 + x * 20, (32 - x) * 20, wallThickness, Block::Kind::PLATFORM_1));
                Spawn(new Block(this, 20 + 620 + x * 20, 60 + x * 20, (32 - x) * 20, wallThickness, Block::Kind::PLATFORM_1));
            }
            //  Spawn(new Block(this, 23 * 20, 260, 380, wallThickness, Block::Kind::PLATFORM_2));
            //  Spawn(new Block(this, 16 * 20, 400, 660, wallThickness, Block::Kind::PLATFORM_2));

            for (int x = 0; x < 33; x++) {
                if (x < 19) {
                    Spawn(new Block(this, 460 + x * 20, 260, 20, wallThickness, Block::Kind::PLATFORM_2));
                }
                Spawn(new Block(this, 320 + x * 20, 400, 20, wallThickness, Block::Kind::PLATFORM_2));
            }
            Spawn(Enemy::create(this, 620, 300, Enemy::Kind::BALL3, 1));
            Spawn(Enemy::create(this, 620, 160, Enemy::Kind::BALL3, -1));
            backTexture = BACKGROUND1;
            levelTime = 160;
            break;
        case 4:
            Spawn(new Block(this, 20, 80, 500, 450, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 760, 80, 500, 450, Block::Kind::PLATFORM_1));

            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 4; x++) {
                    Spawn(new Block(this, 520 + x * 60, 510 - y * 150, 60, wallThickness, Block::Kind::PLATFORM_2));
                }
                Spawn(Enemy::create(this, 520 + 120, 510 - y * 150 - 50, Enemy::Kind::BALL3, -1));
            }
            backTexture = BACKGROUND2;
            levelTime = 160;
            break;
        case 5:
            Spawn(new Block(this, 20, 550, 1050, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 135, 300, 1145, wallThickness, Block::Kind::PLATFORM_1));

            for (int x = 0; x < 4; x++) {
                Spawn(new Block(this, 200 + x * 270, 400, 270, wallThickness, Block::Kind::PLATFORM_2));
                if (x % 2 == 0) {
                    Spawn(Enemy::create(this, 250 + x * 275, 350, Enemy::Kind::BALL3, 1));
                }
                if (x % 2 == 1) {
                    Spawn(Enemy::create(this, 250 + x * 275, 350, Enemy::Kind::BALL3, -1));
                }
            }

            for (int x = 0; x < 4; x++) {
                Spawn(new Block(this, 200 + x * (1080 / 4), 300, 20, 120, Block::Kind::PLATFORM_1));
            }

            Spawn(new Block(this, 740, 80, 20, 160, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 800, 240, 20, 80, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 760, 220, 60, 20, Block::Kind::PLATFORM_1));
            Spawn(new Ladder(this, 1010, 545, 2, 55));
            Spawn(new Ladder(this, 135, 290, 4, 65));

            Spawn(Enemy::create(this, 900, 100, Enemy::Kind::BALL1, -1));

            backTexture = BACKGROUND3;
            levelTime = 160;
            break;
        case 6:
            Spawn(new Block(this, 20, 550, 220, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 240, 550, 60, wallThickness, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 300, 550, 680, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 980, 550, 60, wallThickness, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 1040, 550, 220, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(Enemy::create(this, 900, 100, Enemy::Kind::BALL1, -1));

            backTexture = BACKGROUND4;
            levelTime = 160;
            break;
        case 7:
            for (int x = 0; x < 15; x++) {
                Spawn(new Block(this, 20 + x * 40, 140 + x * 20, 40, wallThickness, Block::Kind::PLATFORM_1));
                Spawn(new Block(this, 1220 - x * 40, 140 + x * 20, 40, wallThickness, Block::Kind::PLATFORM_1));
            }
            Spawn(Enemy::create(this, 600, 150, Enemy::Kind::BALL1, 1));

            backTexture = BACKGROUND5;
            levelTime = 160;
            break;
        case 8:
            for (int x = 1; x < 11; x++) {
                Spawn(Enemy::create(this, x * 60 + 500, 80 + x * 60, Enemy::Kind::BALL4, 1));
            }

            backTexture = BACKGROUND6;
            levelTime = 160;
            break;
        case 9:

            Spawn(new Block(this, 20, 500, 1000, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 1020, 500, 240, wallThickness, Block::Kind::PLATFORM_2));

            Spawn(new Block(this, 260, 300, 1000, wallThickness, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 20, 300, 240, wallThickness, Block::Kind::PLATFORM_2));

            Spawn(new Ladder(this, 200, 295, 3, 55));
            Spawn(new Ladder(this, 1020, 495, 3, 55));

            Spawn(Enemy::create(this, 600, 150, Enemy::Kind::BALL2, 1));
            Spawn(Enemy::create(this, 600, 350, Enemy::Kind::BALL2, -1));
            Spawn(Enemy::create(this, 600, 550, Enemy::Kind::BALL2, 1));

            backTexture = BACKGROUND7;
            levelTime = 160;
            break;
        case 10:
            for (int x = 0; x < 14; x++) {
                Spawn(new Block(this, x * (1240 / 10), 80, 20, 540, Block::Kind::PLATFORM_1));
                if (x % 2 == 0) {
                    Spawn(Enemy::create(this, x * (1240 / 10) + 30, 120, Enemy::Kind::BALL4, 1));
                }
            }

            backTexture = BACKGROUND8;
            levelTime = 160;
            break;
        case 11:
            Spawn(new Block(this, 20, 560, 1240, 20, Block::Kind::PLATFORM_1));

            Spawn(new Ladder(this, 300, 555, 2, 55));
            Spawn(new Ladder(this, 600, 555, 2, 55));
            Spawn(new Ladder(this, 900, 555, 2, 55));
            Spawn(Enemy::create(this, 50, 500, Enemy::Kind::BALL1, 1));
            Spawn(Enemy::create(this, 500, 200, Enemy::Kind::BALL1, 1));
            backTexture = BACKGROUND9;
            levelTime = 160;
            break;
        case 12:
            Spawn(new Block(this, 20, 500, 200, 20, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 220, 500, 80, 20, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 300, 500, 300, 20, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 600, 500, 80, 20, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 680, 500, 300, 20, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 980, 500, 80, 20, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 1060, 500, 200, 20, Block::Kind::PLATFORM_1));

            Spawn(Enemy::create(this, 50, 500, Enemy::Kind::BALL1, 1));
            Spawn(Enemy::create(this, 500, 200, Enemy::Kind::BALL1, 1));

            backTexture = BACKGROUND10;
            levelTime = 160;
            break;
        case 13:
            Spawn(new Block(this, 500, 400, 720, 20, Block::Kind::PLATFORM_1));

            Spawn(new Block(this, 540, 240, 140, 20, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 680, 240, 80, 20, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 760, 240, 260, 20, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 1020, 240, 80, 20, Block::Kind::PLATFORM_2));
            Spawn(new Block(this, 1100, 240, 140, 20, Block::Kind::PLATFORM_1));

            Spawn(new Block(this, 540, 80, 20, 160, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 880, 80, 20, 160, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 1220, 80, 20, 160, Block::Kind::PLATFORM_1));

            Spawn(new Ladder(this, 505, 395, 4, 20));

            Spawn(Enemy::create(this, 590, 120, Enemy::Kind::BALL1, 1));
            Spawn(Enemy::create(this, 1020, 120, Enemy::Kind::BALL1, 1));

            backTexture = BACKGROUND11;
            levelTime = 160;
            break;
    }//1240, 20
}

void Game::Spawn() {
    Spawn(new Block(this, 0, 0, screenWidth, wallThickness, Block::Kind::WALL));
    Spawn(new Block(this, 0, panelHeight + wallThickness, screenWidth, wallThickness, Block::Kind::WALL));
    Spawn(new Block(this, 0, screenHeight - wallThickness, screenWidth, wallThickness, Block::Kind::WALL));
    Spawn(new Block(this, 0, 0, wallThickness, screenHeight, Block::Kind::WALL));
    Spawn(new Block(this, screenWidth - wallThickness, 0, wallThickness, screenHeight, Block::Kind::WALL));

    SpawnLevel();

    // weapon = new Weapon(this, 0, 0, 20, 0, PURPLE);
    // sprites.push_back(weapon);

    player = new Player(this, {400, screenHeight - wallThickness - 64, 64, 64, 12, 10, 40, 54});
    sprites.push_back(player);

    weaponType = 2;
    shootingLeft = 0;
    speedBoost = 1;

    endLevelTime = std::time(nullptr) + levelTime;
}

void Game::Draw() {
    // Draw
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawBackground();

    if (state == State::ACTIVE) {
        DrawPanel();
        DrawSprites();
    }

    switch (state) {
        case State::ACTIVE:
            break;
        case State::MAIN_MENU:
            mainMenu.Draw();
            break;
        case State::PAUSED:
            ingameMenu.Draw();
            break;
        case State::LEVEL_FINISHED:
            DrawSequence("Great Job!");
            break;
        case State::GAME_OVER:
            DrawSequence("Game Over!");
            break;
        case State::GAME_FINISHED:
            DrawSequence("You Won!");
            break;
        case State::LEVEL_SELECTOR:
            DrawLevelSelector();
    }

    EndDrawing();
}

void Game::Update() {
    frameCounter++;

    if (state == State::ACTIVE && lives <= 0) {
        ChangeState(State::GAME_OVER);
    } else if (state == State::GAME_OVER) {
        if (sequenceFrameCounter > frameCounter) return;

        ChangeState(State::MAIN_MENU);
    } else if (state == State::LEVEL_FINISHED) {
        if (sequenceFrameCounter > frameCounter) return;

        level += 1;
        if (level > NUM_LEVELS)
            ChangeState(State::GAME_FINISHED);
        else
            RestartLevel();
    } else if (state == State::GAME_FINISHED) {
        if (sequenceFrameCounter > frameCounter) return;

        ChangeState(State::MAIN_MENU);
    } else if (state == State::LEVEL_SELECTOR) {
        if (IsKeyPressed(KEY_ENTER)) {
            RestartLevel();
        } else if (IsKeyPressed(KEY_ESCAPE)) {
            ChangeState(State::MAIN_MENU);
        } else if (IsKeyPressed(KEY_RIGHT)) {
            if (level < NUM_LEVELS) level += 1;
        } else if (IsKeyPressed(KEY_LEFT)) {
            if (level > 1) level -= 1;
        }
    }

    if (ingameMenu.selected == "Restart") {
        RestartLevel();
        ingameMenu.selected = "";
    } else if (ingameMenu.selected == "Continue") {
        ChangeState(State::ACTIVE);
        ingameMenu.selected = "";
    } else if (ingameMenu.selected == "Back to menu") {
        ChangeState(State::GAME_OVER);
        ingameMenu.selected = "";
    } else if (mainMenu.selected == "Start") {
        StartGame(level);
        mainMenu.selected = "";
    } else if (mainMenu.selected == "Load") {

        mainMenu.selected = "";
    } else if (mainMenu.selected == "Level") {
        ChangeState(State::LEVEL_SELECTOR);
        mainMenu.selected = "";
    }

    if (GetKeyPressed() == KEY_ESCAPE) {
        if (state == State::PAUSED)
            ChangeState(State::ACTIVE);
        else if (state == State::ACTIVE)
            ChangeState(State::PAUSED);
    }

    if (state == State::PAUSED) {
        ingameMenu.Update();
    } else if (state == State::ACTIVE) {
        if (endLevelTime > 0) {
            std::time_t now = std::time(nullptr);
            elapsedLevelTime = endLevelTime - now;
            if (elapsedLevelTime <= 0) {
                elapsedLevelTime = 0;
                endLevelTime = 0;
                ChangeState(State::GAME_OVER);
            }
        }
        MoveSprites();
        for (int i = 0; i < 4; i++) {
            if (timeLeft != 0) {
                CheckTime();
            }
        }
    } else if (state == State::MAIN_MENU) {
        mainMenu.Update();
    }
}

int Game::MainLoop() {
    while (!WindowShouldClose() && ingameMenu.selected != "Quit" && mainMenu.selected != "Quit") {
        Update();
        CheckCollision();
        Draw();
    }

    return 0;
}

void Game::LoadTextures() {
  for (int i = 0; i < NUM_TEXTURES; i++) {
      Image image = LoadImage(textureFiles[i]);
      textures[i] = LoadTextureFromImage(image);
      UnloadImage(image);
  }
}

void Game::Unspawn() {
    std::for_each(begin(sprites), end(sprites), [](Sprite *s) {
        delete s;
    });
    sprites.clear();
    spriteMap.clear();
}

void Game::DrawBackground() {
    if (textures[backTexture].width > 2560) {
        DrawTextureRec(textures[backTexture], backRec, { 0, 0 }, WHITE);
        if (backFrame++ % 8 == 0) {
            backRec.x += 1398;
            backFrame = 1;
        }
    } else {
        DrawTexture(textures[backTexture], 0, 0, WHITE);
    }
}

void Game::DrawPanel() {
    float y = wallThickness + 10;

    DrawRectangleRec({wallThickness, wallThickness, screenWidth - 2 * wallThickness, panelHeight}, ColorAlpha(BLACK, 0.6));
    //DrawFPS(wallThickness + 10, y);
    DrawText(TextFormat("Level: %i", level), 40, y, 20, GREEN);
    DrawText(TextFormat("Score: %03i", score), 245, y, 20, GREEN);
    DrawText(TextFormat("Lives: %i", lives), 900, y, 20, GREEN);
    DrawText(TextFormat("Time: %03i", elapsedLevelTime), 1150, y, 20, GREEN);
    if (multiWeapon != 0)
        DrawTexture(textures[HUD_DOUBLE], 450, y - 5, WHITE);
    if (speedBoost != 1)
        DrawTexture(textures[HUD_BOOST], 500, y - 5, WHITE);
    switch (weaponType) {
    case 2:
        hudWeapons.x = 0;
        DrawTextureRec(textures[HUD_WEAPONS], hudWeapons, { 600, y - 5 }, WHITE);
        break;
    case 3:
        hudWeapons.x = hudWeapons.width;
        DrawTextureRec(textures[HUD_WEAPONS], hudWeapons, { 600, y - 5}, WHITE);
        break;
    case 4:
        hudWeapons.x = 2 * hudWeapons.width;
        DrawTextureRec(textures[HUD_WEAPONS], hudWeapons, { 600, y - 5 }, WHITE);
        DrawText(TextFormat("x%i", shootingLeft), 565, y, 20, GREEN);
        break;
    default:
        break;
    }
}

void Game::DrawSequence(const char* message) {
    DrawRectangleRec({0, 0, screenWidth, screenHeight}, ColorAlpha(BLACK, 0.6));
    DrawText(message, 400, 400, 40, GREEN);
}

void Game::ChangeState(State newState) {
    if (state == newState) return;

    switch (newState) {
        case State::GAME_OVER:
            Unspawn();
            sequenceFrameCounter = frameCounter + 2 * 60;
            break;
        case State::LEVEL_FINISHED:
            Unspawn();
            sequenceFrameCounter = frameCounter + 2 * 60;
            break;
        case State::GAME_FINISHED:
            Unspawn();
            sequenceFrameCounter = frameCounter + 2 * 60;
            break;
    }
    state = newState;
}

void Game::RestartLevel() {
    Unspawn();
    Spawn();
    lives = 5;
    ChangeState(State::ACTIVE);
}

void Game::StartGame(int newLevel) {
    level = newLevel;
    score = 0;
    RestartLevel();
}

void Game::DrawLevelSelector() {
    float xOffset = 150;
    float y = 200;
    float width = 40;
    float height = 40;
    float space = 10;
    for (int i = 0; i < NUM_LEVELS; i++) {
        float x = xOffset + i * (width + space);
        Color color = GREEN;
        if (i > 4) {
            if (i < 10)
                color = YELLOW;
            else
                color = RED;
        }
        DrawRectangleRec({x, y, width, height}, color);
        DrawText(TextFormat("%02i", i + 1), x, y, 30, BLACK);
    }
    DrawRectangleRounded({xOffset + (level - 1) * (width + space) - 10, y - 10, width + 20, height + 20}, 0.2, 8, ColorAlpha(ORANGE, 0.6));
}

std::vector<Sprite*> Game::GetSprites(Sprite::Type type) {
    if (spriteMap.find(type) == spriteMap.end())
        return {};
    return spriteMap[type];
}