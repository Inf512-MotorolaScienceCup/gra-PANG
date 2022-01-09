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
    // Collision Player<->Enemy
    for (auto& enemy : spriteMap[Sprite::Type::ENEMY]) {
        if (IsCollision(player, enemy)) {
        // std::cout << "PLAYER collided with ENEMY:" << std::endl;
            player->Collision(enemy);
        }
    }

    // Collision Player<->Block
    for (auto& block : spriteMap[Sprite::Type::BLOCK]) {
        if (IsCollision(player, block)) {
        // std::cout << "PLAYER collided with BLOCK:" << std::endl;
            player->Collision(block);
        }
    }

    // Collision Player<->Ladder
    for (auto& ladder : spriteMap[Sprite::Type::LADDER]) {
        if (IsCollision(player, ladder)) {
            // std::cout << "PLAYER collided with LADDER:" << std::endl;
            player->Collision(ladder);
        }
    }

    // Collision Weapon<->Enemy
    for (auto& weapon : spriteMap[Sprite::Type::WEAPON]) {
        for (auto& enemy : spriteMap[Sprite::Type::ENEMY]) {
            if (IsCollision(weapon, enemy)) {
                std::cout << "WEAPON collided with ENEMY:" << std::endl;
                // FIXME: Now implemented in Enemy::checkCollision
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
    for (auto& weapon : spriteMap[Sprite::Type::WEAPON]) {
        for (auto& block : spriteMap[Sprite::Type::BLOCK]) {
            if (IsCollision(weapon, block)) {
                // std::cout << "WEAPON collided with BLOCK:" << std::endl;
                weapon->Collision(block);
                break;
            }
        }
    }

    // Collision Enemy<->Block
    for (auto& enemy : spriteMap[Sprite::Type::ENEMY]) {
        for (auto& block : spriteMap[Sprite::Type::BLOCK]) {
        if (IsCollision(enemy, block)) {
            // std::cout << "ENEMY collided with BLOCK:" << std::endl;
            // FIXME: Now implemented in Enemy::checkCollision
            enemy->Collision(block);
        }
        }
    }
}

Rectangle Game::getPlayerPosition() {
    for (auto& sprite : sprites) {
        if (sprite->type == Sprite::Type::PLAYER) {
            return sprite->position.rectangle;
        }
    }
    return { 0, 0, 0, 0 };
}

void Game::AddEnemy(float x, float y, Enemy::Kind kind, int heading) {
    Sprite* s = Enemy::create(this, x, y, kind, heading);
    spriteMap[Sprite::Type::ENEMY].push_back(s);
    sprites.push_back(s);
}

void Game::AddWeapon(float x, float y) {
    //auto w = spriteMap.find(Sprite::Type::WEAPON);
    //if (w != spriteMap.end() && !w->second.empty()) return;
    weapon = new Weapon(this, x, y, 20, 0, PURPLE);
    spriteMap[Sprite::Type::WEAPON].push_back(weapon);
    sprites.push_back(weapon);
}

void Game::AddScore(int score) {
    this->score += score;
}

Game::Game()
    : state(State::MAIN_MENU),
      mainMenu(this, {"Start", "Load", "Level", "Quit"}),
      ingameMenu(this, {"Continue", "Break", "Restart", "Quit"}) {
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
            Spawn(new Block(this, 500, 500, 150, wallThickness, Block::Type::PLATFORM_1));
            Spawn(new Block(this, 200, 400, 150, wallThickness, Block::Type::PLATFORM_2));

            // y value must always be 5-10 points lower than block value
            // distanceToGround must be properly calculated
            Spawn(new Ladder(this, 200, 395, 4, 25));

            Spawn(Enemy::create(this, 400, 200, Enemy::Kind::BALL1, 1));
            //Spawn(Enemy::create(this, 500, 400, Enemy::Kind::BALL1, 1));

            levelTime = 30;
            break;
        case 2:
            Spawn(new Block(this, 100, 500, 150, wallThickness, Block::Type::PLATFORM_1));
            Spawn(new Block(this, 500, 500, 150, wallThickness, Block::Type::PLATFORM_1));
            Spawn(new Block(this, 200, 400, 150, wallThickness, Block::Type::PLATFORM_2));
            Spawn(new Block(this, 700, 400, 150, wallThickness, Block::Type::PLATFORM_2));

            Spawn(new Ladder(this, 200, 400, 4, 25));

            Spawn(Enemy::create(this, 400, 200, Enemy::Kind::BALL1, 1));
            Spawn(Enemy::create(this, 500, 400, Enemy::Kind::BALL1, 1));

            levelTime = 30;
            break;
    }
}

void Game::Spawn() {
    Spawn(new Block(this, 0, 0, screenWidth, wallThickness, Block::Type::WALL));
    Spawn(new Block(this, 0, panelHeight + wallThickness, screenWidth, wallThickness, Block::Type::WALL));
    Spawn(new Block(this, 0, screenHeight - wallThickness, screenWidth, wallThickness, Block::Type::WALL));
    Spawn(new Block(this, 0, 0, wallThickness, screenHeight, Block::Type::WALL));
    Spawn(new Block(this, screenWidth - wallThickness, 0, wallThickness, screenHeight, Block::Type::WALL));

    SpawnLevel();

    // weapon = new Weapon(this, 0, 0, 20, 0, PURPLE);
    // sprites.push_back(weapon);

    player = new Player(this, {400, screenHeight - wallThickness - 64, 64, 64, 12, 10, 40, 54}, BLACK);
    sprites.push_back(player);

    endLevelTime = std::time(nullptr) + levelTime;
}

void Game::Draw() {
    // Draw
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawTexture(textures[BACKGROUND], 0, 0, WHITE);

    if (state == State::ACTIVE || state == State::PLAYER_DIED) {
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
        case State::PLAYER_DIED:
            DrawSequence("You are Dead!");
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

    if (state == State::PLAYER_DIED) {
        if (sequenceFrameCounter > frameCounter) return;

        if (lives > 0)
            RestartLevel();
        else
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
    } else if (mainMenu.selected == "Start") {
        StartGame();
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
                ChangeState(State::PLAYER_DIED);
            }
        }
        MoveSprites();
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

void Game::DrawPanel() {
    float y = wallThickness + 10;

    DrawRectangleRec({wallThickness, wallThickness, screenWidth - 2 * wallThickness, panelHeight}, ColorAlpha(BLACK, 0.6));
    DrawFPS(wallThickness + 10, y);
    DrawText(TextFormat("Level: %i", level), 300, y, 20, GREEN);
    DrawText(TextFormat("Score: %03i", score), 560, y, 20, GREEN);
    DrawText(TextFormat("Lives: %i", lives), 900, y, 20, GREEN);
    DrawText(TextFormat("Time: %03i", elapsedLevelTime), 1150, y, 20, GREEN);
}

void Game::DrawSequence(const char* message) {
    DrawRectangleRec({0, 0, screenWidth, screenHeight}, ColorAlpha(BLACK, 0.6));
    DrawText(message, 400, 400, 40, GREEN);
}

void Game::ChangeState(State newState) {
    if (state == newState) return;

    switch (newState) {
        case State::PLAYER_DIED:
            player->state = Sprite::State::FINISHING;
            lives -= 1;
            sequenceFrameCounter = frameCounter + 2 * 60;
            break;
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
    ChangeState(State::ACTIVE);
}

void Game::StartGame(int newLevel) {
    level = newLevel;
    score = 0;
    lives = 3;
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

// void Game::ChangeState() {
//     std::time_t now = std::time(nullptr);
//     if (endLevelTime - now <= 0)
//         return;
// }
