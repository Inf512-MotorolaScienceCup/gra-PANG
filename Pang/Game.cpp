#include <algorithm>
#include <vector>
#include <iostream>

#include "raylib.h"
#include "raymath.h"

#include "Game.h"
#include "Sprite.h"
#include "Menu.h"

Game::Game()
    : state(State::MOD_MENU),
    mainMenu(this, { "Start", "Load", "Level", "Ranking", "Quit" }, Menu::Type::MAIN_MENU),
    ingameMenu(this, { "Continue", "Save Game", "Back to Menu", "Quit" }, Menu::Type::PAUSE_MENU),
    saveMenu(this, FindSaveFiles(), Menu::Type::PAUSE_MENU),
    overwriteMenu(this, { "Overwrite?", "Cancel" }, Menu::Type::PAUSE_MENU),
    loadMenu(this, FindLoadFiles(), Menu::Type::LOAD_MENU),
    modMenu(this, { "Mod 1", "Mod 2", "Mod 3", "Quit" }, Menu::Type::MAIN_MENU),
    diffLvlMenu(this, { "Easy", "Normal", "Hard", "Quit" }, Menu::Type::MAIN_MENU) {
    InitWindow(screenWidth, screenHeight, "Pang");
    InitAudioDevice();
    SetExitKey(KEY_F10);
    SetTargetFPS(60);
    LoadTextures();
    LoadAudio();
}

Game::~Game() {
    Unspawn();
    Unload();
    CloseWindow();
    CloseAudioDevice();
}

void Game::LoadTextures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        Image image = LoadImage(textureFiles[i]);
        textures[i] = LoadTextureFromImage(image);
        UnloadImage(image);
        SetTextureFilter(textures[i], TEXTURE_FILTER_TRILINEAR);
    }
    font = LoadFontEx("res/font/Organetto-Regular.ttf", 54, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
}

void Game::LoadAudio() {
    for (int i = 0; i < NUM_AUDIO; i++) {
        audio[i] = LoadSound(audioFiles[i]);
    }
    SetSoundVolume(audio[BALL_BOUNCE], 0.1f);
    SetSoundVolume(audio[WALK], 0.5f);
    SetSoundVolume(audio[HARPUN], 0.3f);

    for (int i = 0; i < NUM_MUSIC; i++) {
        music[i] = LoadMusicStream(musicFiles[i]);
        music[i].looping = true;
    }
    SetMusicVolume(music[BACKGROUND_A], 0.1f);
    SetMusicVolume(music[BACKGROUND_B], 0.05f);
    SetMusicVolume(music[BACKGROUND_C], 0.15f);
    SetMusicVolume(music[BACKGROUND_D], 0.15f);
    SetMusicVolume(music[BACKGROUND_E], 0.1f);
}

void Game::Unload() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }
    StopSoundMulti();
    for (int i = 0; i < NUM_AUDIO; i++) {
        UnloadSound(audio[i]);
    }
    for (int i = 0; i < NUM_MUSIC; i++) {
        UnloadMusicStream(music[i]);
    }
    ClearDroppedFiles();

    UnloadFont(font);
}

int Game::MainLoop() {
    LoadUsrData();

    while (!WindowShouldClose() && ingameMenu.selected != "Quit" && mainMenu.selected != "Quit" && diffLvlMenu.selected != "Quit" && modMenu.selected != "Quit") {
        Update();
        CheckCollision();
        Draw();
    }
    SaveUsrData();

    return 0;
}

void Game::Update() {
    frameCounter++;

    for (int i = 0; i < 4; i++) {
        if (lifeTime[i] != 0)
            CheckTime();
    }

    if (GetKeyPressed() == KEY_ESCAPE) {
        if (state == State::PAUSED) {
            ChangeState(State::ACTIVE);
            PlaySound(audio[MENU_BACK]);
        } else if (state == State::ACTIVE) {
            ChangeState(State::PAUSED);
            PlaySound(audio[MENU_BACK]);
        } else if (state == State::SAVE_MENU) {
            ChangeState(State::PAUSED);
            PlaySound(audio[MENU_BACK]);
        } else if (state == State::LOAD_MENU) {
            ChangeState(State::MAIN_MENU);
            PlaySound(audio[MENU_BACK]);
        } else if (state == State::MAIN_MENU) {
            ChangeState(State::MOD_MENU);
            PlaySound(audio[MENU_BACK]);
        } else if (state == State::DIFFLVL_MENU) {
            ChangeState(State::MOD_MENU);
            PlaySound(audio[MENU_BACK]);
        } else if (state == State::RANKING) {
            ChangeState(State::MAIN_MENU);
            PlaySound(audio[MENU_BACK]);
        }
    }  

    switch (state) {
    case Game::State::MAIN_MENU:
        if (mainMenu.selected == "Start") {
            ChangeState(State::GAME_START);
            mainMenu.selected = "";
        } else if (mainMenu.selected == "Load") {
            ChangeState(State::LOAD_MENU);
            loadMenu.Reload(FindLoadFiles());
            mainMenu.selected = "";
        } else if (mainMenu.selected == "Level") {
            ChangeState(State::LEVEL_SELECTOR);
            mainMenu.selected = "";
        } else if (mainMenu.selected == "Ranking") {
            ChangeState(State::RANKING);
            mainMenu.selected = "";
        }
        mainMenu.Update();
        break;
    case Game::State::MOD_MENU:
        if (modMenu.selected == "Mod 1") {
            ChangeState(State::DIFFLVL_MENU);
            modNum = 1;
            modMenu.selected = "";
        } else if (modMenu.selected == "Mod 2") {
            ChangeState(State::MAIN_MENU);
            modNum = 2;
            level = 1;
            modMenu.selected = "";
        } else if (modMenu.selected == "Mod 3") {
            ChangeState(State::MAIN_MENU);
            modNum = 3;
            level = 16;
            modMenu.selected = "";
        }
        modMenu.Update();
        break;
    case Game::State::DIFFLVL_MENU:
        if (diffLvlMenu.selected == "Easy") {
            level = GetRandomValue(1, 5);
            ChangeState(State::GAME_START);
            diffLvlMenu.selected = "";
        } else if (diffLvlMenu.selected == "Normal") {
            level = GetRandomValue(6, 10);
            ChangeState(State::GAME_START);
            diffLvlMenu.selected = "";
        } else if (diffLvlMenu.selected == "Hard") {
            level = GetRandomValue(11, 15);
            ChangeState(State::GAME_START);
            diffLvlMenu.selected = "";
        }
        diffLvlMenu.Update();
        break;
    case Game::State::LEVEL_SELECTOR:
        if (IsKeyPressed(KEY_ENTER)) {
            PlaySound(audio[MENU_ENTER]);
            ChangeState(State::GAME_START);
        } else if (IsKeyPressed(KEY_ESCAPE)) {
            ChangeState(State::MAIN_MENU);
        } else if (IsKeyPressed(KEY_RIGHT)) {
            if (modNum == 2) {
                if (level < 15) {
                    level += 1;
                    PlaySoundMulti(audio[MENU_SELECT]);
                }
            } else {
                if (level < NUM_LEVELS) {
                    level += 1;
                    PlaySoundMulti(audio[MENU_SELECT]);
                }
            }
        } else if (IsKeyPressed(KEY_LEFT)) {
            if (modNum == 2) {
                if (level > 1) {
                    level -= 1;
                    PlaySoundMulti(audio[MENU_SELECT]);
                }
            } else {
                if (level > 16) {
                    level -= 1;
                    PlaySoundMulti(audio[MENU_SELECT]);
                }
            }
        }
        break;
    case Game::State::SAVE_MENU:
        if (saveMenu.selected == "Save 1") {
            saveNum = 1;
            ChangeState(State::OVERWRITE_MENU);
            saveMenu.selected = "";
        } else if (saveMenu.selected == "Save 2") {
            saveNum = 2;
            ChangeState(State::OVERWRITE_MENU);
            saveMenu.selected = "";
        } else if (saveMenu.selected == "Save 3") {
            saveNum = 3;
            ChangeState(State::OVERWRITE_MENU);
            saveMenu.selected = "";
        } else if (saveMenu.selected == "Save 4") {
            saveNum = 4;
            ChangeState(State::OVERWRITE_MENU);
            saveMenu.selected = "";
        } else if (saveMenu.selected == "Empty slot") {
            if (SaveGame(numFiles + 1))
                ChangeState(State::GAME_SAVED);
            else
                ChangeState(State::ERROR);

            saveMenu.selected = "";
        }
        saveMenu.Update();
        break;
    case Game::State::OVERWRITE_MENU:
        if (overwriteMenu.selected == "Overwrite?") {
            if (SaveGame(saveNum))
                ChangeState(State::GAME_SAVED);
            else
                ChangeState(State::ERROR);
        } else if (overwriteMenu.selected == "Cancel")
            ChangeState(State::SAVE_MENU);

        overwriteMenu.selected = "";
        overwriteMenu.Update();
        break;
    case Game::State::LOAD_MENU:
        if (loadMenu.selected == "Save 1") {
            LoadGame(1);
            ChangeState(State::ACTIVE);
            loadMenu.selected = "";
        } else if (loadMenu.selected == "Save 2") {
            LoadGame(2);
            ChangeState(State::ACTIVE);
            loadMenu.selected = "";
        } else if (loadMenu.selected == "Save 3") {
            LoadGame(3);
            ChangeState(State::ACTIVE);
            loadMenu.selected = "";
        } else if (loadMenu.selected == "Save 4") {
            LoadGame(4);
            ChangeState(State::ACTIVE);
            loadMenu.selected = "";
        } else if (loadMenu.remove == "Save 1") {
            RemoveSave(1);
            loadMenu.remove = "";
        } else if (loadMenu.remove == "Save 2") {
            RemoveSave(2);
            loadMenu.remove = "";
        } else if (loadMenu.remove == "Save 3") {
            RemoveSave(3);
            loadMenu.remove = "";
        } else if (loadMenu.remove == "Save 4") {
            RemoveSave(4);
            loadMenu.remove = "";
        }
        loadMenu.Update();
        break;
    case Game::State::ACTIVE:
        if (lives <= 0) {
            ChangeState(State::GAME_OVER);
            break;
        }
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

        if (laserCooldown > 0)
            laserCooldown--;

        if (backMusic != NUM_MUSIC)
            UpdateMusicStream(music[backMusic]);
        break;
    case Game::State::PAUSED:
        if (ingameMenu.selected == "Save Game") {
            ChangeState(State::SAVE_MENU);
            saveMenu.Reload(FindSaveFiles());
            ingameMenu.selected = "";
        } else if (ingameMenu.selected == "Continue") {
            ChangeState(State::ACTIVE);
            ingameMenu.selected = "";
        } else if (ingameMenu.selected == "Back to Menu") {
            ChangeState(State::GAME_OVER);
            ingameMenu.selected = "";
        }
        ingameMenu.Update();

        endLevelTime = std::time(nullptr) + elapsedLevelTime;
        break;
    case Game::State::LEVEL_FINISHED:
        if (sequenceFrameCounter > frameCounter) return;

        level += 1;
        if (modNum == 2) {
            if (level > 15)
                ChangeState(State::GAME_FINISHED);
            else
                RestartLevel();
        } else {
            if (level > NUM_LEVELS)
                ChangeState(State::GAME_FINISHED);
            else
                RestartLevel();
        }
        break;
    case Game::State::GAME_START:
        if (sequenceFrameCounter > frameCounter) return;

        StartGame();
        break;
    case Game::State::GAME_OVER:
        if (sequenceFrameCounter > frameCounter) return;

        if (backMusic != NUM_MUSIC && IsMusicStreamPlaying(music[backMusic])) {
            StopMusicStream(music[backMusic]);
            backMusic = NUM_MUSIC;
        }
        if (modNum == 2) {
            for (int i = 0; i < 10; i++) {
                if (rankScore[i] < score) {
                    if (i != 9) {
                        for (int j = 9; j > i; j--) {
                            rankScore[j] = rankScore[j - 1];
                        }
                        rankScore[i] = score;
                    } else
                        rankScore[i] = score;
                    break;
                }
            }
        }
        if (modNum == 1)
            ChangeState(State::DIFFLVL_MENU);
        else
            ChangeState(State::MAIN_MENU);
        backTexture = BACKGROUND_ANI2;
        break;
    case Game::State::GAME_FINISHED:
        for (int i = 0; i < 10; i++) {
            if (rankScore[i] < score) {
                if (i != 9) {
                    for (int j = 9; j > i; j--) {
                        rankScore[j] = rankScore[j - 1];
                    }
                    rankScore[i] = score;
                } else 
                    rankScore[i] = score;
            }
        }

        if (sequenceFrameCounter > frameCounter) return;

        ChangeState(State::MAIN_MENU);
        break;
    case Game::State::GAME_SAVED:
        if (sequenceFrameCounter > frameCounter) return;

        saveMenu.Reload(FindSaveFiles());
        ChangeState(State::SAVE_MENU);
        break;
    default:
        break;
    }
}

void Game::ChangeState(State newState) {
    if (state == newState) return;

    switch (newState) {
    case State::ACTIVE:
        if (state == State::PAUSED && !endAnim) {
            endAnim = true;
            newAnim = newState;
            return;
        }
        break;
    case State::PAUSED:
        if (state == State::SAVE_MENU && !endAnim) {
            endAnim = true;
            newAnim = newState;
            return;
        } else if (state == State::ACTIVE) {
            for (int i = 0; i < NUM_AUDIO; i++) {
                if (IsSoundPlaying(audio[i]))
                    StopSound(audio[i]);
            }
        }
        ingameMenu.animVec = { screenWidth, screenHeight - 50 };
        break;
    case State::MOD_MENU:
        modMenu.animVec = { 200, 0 };
        break;
    case State::DIFFLVL_MENU:
        diffLvlMenu.animVec = { 200, 0 };
        break;
    case State::MAIN_MENU:
        mainMenu.animVec = { 200, 0 };
        if ((state == State::LOAD_MENU || state == State::RANKING || state == State::LEVEL_SELECTOR) && !endAnim) {
            endAnim = true;
            newAnim = newState;
            return;
        }
        break;
    case State::SAVE_MENU:
        if (state == State::PAUSED && !endAnim) {
            endAnim = true;
            newAnim = newState;
            return;
        }
        saveMenu.animVec = { screenWidth, screenHeight - 50 };
        break;
    case State::LOAD_MENU:
        loadMenu.animVec = { screenWidth, 0 };
        animVec = { 600, 0 };
        break;
    case State::LEVEL_SELECTOR:
        if (modNum == 2)
            level = 1;
        else
            level = 16;
        animVec = { screenWidth - 300, 0 };
        break;
    case State::RANKING:
        animVec = { screenWidth - 300, 0 };
        break;
    case State::GAME_OVER:
        Unspawn();
        for (int i = 0; i < NUM_AUDIO; i++) {
            if (IsSoundPlaying(audio[i]))
                StopSound(audio[i]);
        }
        frameCounter = 0;
        sequenceFrameCounter = 4 * 60;
        animVec = { 0, 0 };
        PlaySound(audio[GAME_OVER]);
        break;
    case State::LEVEL_FINISHED:
        Unspawn();
        for (int i = 0; i < NUM_AUDIO; i++) {
            if (IsSoundPlaying(audio[i]))
                StopSound(audio[i]);
        }
        timeBonus = elapsedLevelTime * 25;
        score += timeBonus;
        frameCounter = 0;
        sequenceFrameCounter = 4 * 60;
        animVec = { 0, 0 };
        PlaySound(audio[LEVEL_COMPLETED]);
        break;
    case State::GAME_START:
        frameCounter = 0;
        sequenceFrameCounter = 60;
        animVec = { screenWidth * 2, 0 };
        PlaySound(audio[LEVEL_START]);
        break;
    case State::GAME_FINISHED:
        for (int i = 0; i < NUM_AUDIO; i++) {
            if (IsSoundPlaying(audio[i]))
                StopSound(audio[i]);
        }
        Unspawn();
        frameCounter = 0;
        sequenceFrameCounter = 4 * 60;
        animVec = { 0, 0 };
        //PlaySound(audio[GAME_COMPLETED]);
        break;
    case State::GAME_SAVED:
        frameCounter = 0;
        sequenceFrameCounter = 2 * 60;
        animVec = { 0, 0 };
        break;
    case State::ERROR:
        frameCounter = 0;
        sequenceFrameCounter = 2 * 60;
        PlaySound(audio[ERROR]);
        break;
    }
    state = newState;
}

void Game::CheckCollision() {
    // Collision Player<->Block
    for (auto& block : GetSprites(Sprite::Type::BLOCK)) {
        if (IsCollision(player, block)) {
            player->Collision(block);
        }
    }

    // Collision Player<->Enemy
    for (auto& enemy : GetSprites(Sprite::Type::ENEMY)) {
        if (IsCollision(player, enemy)) {
            if (!player->frameCounter)
                player->Collision(enemy);
        }
    }

    // Collision Player<->Ladder
    for (auto& ladder : GetSprites(Sprite::Type::LADDER)) {
        if (IsCollision(player, ladder)) {
            player->Collision(ladder);
        }
    }

    // Collision Player<->Powerup
    for (auto& powerup : GetSprites(Sprite::Type::POWERUP)) {
        if (IsCollision(player, powerup)) {
            player->Collision(powerup);
            powerup->Collision(player);
        }
    }

    // Collision Player<->Ice
    for (auto& ice : GetSprites(Sprite::Type::ICE)) {
        if (IsCollision(player, ice)) {
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

void Game::MoveSprites() {
    for (size_t i = 0; i < sprites.size(); i++) {
        sprites[i]->Move();
    }
    auto removed = remove_if(sprites.begin(), sprites.end(), [](Sprite* sprite) { return sprite->state == Sprite::State::FINISHED; });
    std::for_each(removed, sprites.end(), [](Sprite* sprite) { std::cout << "REMOVE:" << (int)sprite->type << " ptr:" << sprite << std::endl; });
    sprites.erase(removed, sprites.end());
}

void Game::Spawn() {
    Spawn(new Block(this, 0, 0, screenWidth, wallThickness, Block::Kind::WALL));
    Spawn(new Block(this, 0, panelHeight + wallThickness, screenWidth, wallThickness, Block::Kind::WALL));
    Spawn(new Block(this, 0, screenHeight - wallThickness, screenWidth, wallThickness, Block::Kind::WALL));
    Spawn(new Block(this, 0, 0, wallThickness, screenHeight, Block::Kind::WALL));
    Spawn(new Block(this, screenWidth - wallThickness, 0, wallThickness, screenHeight, Block::Kind::WALL));

    weaponType = 1;
    shootingLeft = 0;
    speedBoost = 1;

    SpawnLevel();

    if (backMusic != NUM_MUSIC)
        PlayMusicStream(music[backMusic]);

    endLevelTime = std::time(nullptr) + levelTime;
}

void Game::Spawn(Sprite* sprite) {
    sprites.push_back(sprite);
    spriteMap[sprite->type].push_back(sprite);
}

void Game::SpawnLevel() {
    switch (level) {
    case 1:
        for (int x = 16; x > 0; x--) {
            Spawn(new Block(this, 20, 40 + x * 40, (16 - x) * 40, 40, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, 620 + x * 40, 40 + x * 40, (16 - x) * 40, 40, Block::Kind::PLATFORM_1));
        }

        for (int x = 0; x < 15; x++) {
            if (x < 9) {
                Spawn(new Block(this, 460 + x * 40, 260, 40, wallThickness, Block::Kind::PLATFORM_2));
            }
            Spawn(new Block(this, 340 + x * 40, 380, 40, wallThickness, Block::Kind::PLATFORM_2));
        }
        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        Spawn(Enemy::create(this, 620, 300, Enemy::Kind::BALL3, 1));
        Spawn(Enemy::create(this, 620, 160, Enemy::Kind::BALL3, -1));
        backMusic = BACKGROUND_C;
        backTexture = BACKGROUND1;
        levelTime = 160;
        break;
    case 2:
        Spawn(new Block(this, 20, 80, 500, 450, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 760, 80, 500, 450, Block::Kind::PLATFORM_1));

        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 4; x++) {
                Spawn(new Block(this, 520 + x * 60, 510 - y * 150, 60, wallThickness, Block::Kind::PLATFORM_2));
            }
            Spawn(Enemy::create(this, 520 + 120, 510 - y * 150 - 50, Enemy::Kind::BALL3, -1));
        }

        Spawn(new Powerup(this, 650, 140, Powerup::Kind::HEAL));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_A;
        backTexture = BACKGROUND2;
        levelTime = 160;
        break;


    case 3:
        Spawn(new Block(this, 200, 400, 150, wallThickness, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 450, 500, 150, wallThickness, Block::Kind::PLATFORM_2));

        // y value must always be 5-10 points lower than block value
        // distanceToGround must be properly calculated
        Spawn(new Ladder(this, 200, 395, 4, 25));
        Spawn(new Powerup(this, 280, 400 - 64, Powerup::Kind::HEAL));

        Spawn(Enemy::create(this, 400, 200, Enemy::Kind::BALL1, 1));

        player = new Player(this, { 200,400 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backTexture = BACKGROUND_ANI;
        backMusic = BACKGROUND_A;
        levelTime = 60;
        break;
    case 4:
        Spawn(new Block(this, 20, 550, 220, wallThickness, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 240, 550, 60, wallThickness, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 300, 550, 680, wallThickness, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 980, 550, 60, wallThickness, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 1040, 550, 220, wallThickness, Block::Kind::PLATFORM_1));
        Spawn(Enemy::create(this, 900, 200, Enemy::Kind::BALL1, -1));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_D;
        backTexture = BACKGROUND4;
        levelTime = 160;
        break;
    case 5:
        for (int x = 1; x < 10; x++) {
            Spawn(new Block(this, x * (1260 / 10), 80, 20, 540, Block::Kind::PLATFORM_1));
            if (x % 2 == 0) {
                Spawn(Enemy::create(this, x * (1260 / 10) + 50, 120, Enemy::Kind::BALL4, 1));
            }
        }    //                       x     y                          wymiary 64-64                
        player = new Player(this, { 420,screenHeight - wallThickness - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_B;
        backTexture = BACKGROUND8;
        levelTime = 160;
        break;
    case 6:

        Spawn(new Block(this, 20, 550, 180, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 430, 380, 80, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 940, 280, 80, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 740, 480, 220, 20, Block::Kind::PLATFORM_2));

        Spawn(Enemy::create(this, 1000, 200, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 100, 500, Enemy::Kind::BALL2, 1));

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_C;
        backTexture = BACKGROUND10;
        levelTime = 160;
        break;
    case 7:

        Spawn(new Block(this, 20, 660, 400, 40, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 860, 660, 400, 40, Block::Kind::PLATFORM_1));

        Spawn(new Block(this, 430, 380, 80, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 940, 280, 80, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 720, 480, 220, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 520, 330, 120, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 60, 230, 120, 20, Block::Kind::PLATFORM_1));

        Spawn(Enemy::create(this, 1000, 200, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 100, 500, Enemy::Kind::BALL2, 1));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_B;
        backTexture = BACKGROUND11;
        levelTime = 160;
        break;
    case 8:
        for (int x = 0; x < 15; x++) {
            if (!(x == 6 || x == 7)) {
                Spawn(new Block(this, 20 + x * 40, 140 + x * 20, 40, wallThickness, Block::Kind::PLATFORM_1));
                Spawn(new Block(this, 1220 - x * 40, 140 + x * 20, 40, wallThickness, Block::Kind::PLATFORM_1));
            }
        }
        Spawn(Enemy::create(this, 600, 150, Enemy::Kind::BALL1, 1));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_C;
        backTexture = BACKGROUND5;
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

        Spawn(new Powerup(this, 460, 500 - 64, Powerup::Kind::HEAL));

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);


        backMusic = BACKGROUND_C;
        backTexture = BACKGROUND7;
        levelTime = 160;
        break;
    case 10:
        for (int x = 1; x < 11; x++) {
            Spawn(Enemy::create(this, x * 60 + 500, 80 + x * 60, Enemy::Kind::BALL4, 1));
        }
        Spawn(new Powerup(this, 800, 720 - 10 - 64, Powerup::Kind::HEAL));

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backTexture = BACKGROUND6;
        backMusic = BACKGROUND_D;
        levelTime = 160;
        break;
    case 11:
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

        Spawn(Enemy::create(this, 900, 130, Enemy::Kind::BALL1, -1));

        Spawn(new Powerup(this, 290, 340, Powerup::Kind::HEAL));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_A;
        backTexture = BACKGROUND3;
        levelTime = 160;
        break;
    case 12:
        Spawn(new Block(this, 500, 500, 150, wallThickness, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 200, 400, 150, wallThickness, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 700, 400, 150, wallThickness, Block::Kind::PLATFORM_2));

        Spawn(new Ladder(this, 200, 395, 4, 25));

        Spawn(Enemy::create(this, 400, 200, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 500, 400, Enemy::Kind::BALL1, 1));


        player = new Player(this, { 200,400 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        Spawn(new Powerup(this, 1050, 720 - 20 - 64, Powerup::Kind::HEAL));
        backMusic = BACKGROUND_A;
        backTexture = BACKGROUND_ANI;
        levelTime = 60;
        break;
    case 13:
        Spawn(new Block(this, 20, 560, 1240, 20, Block::Kind::PLATFORM_1));

        Spawn(new Ladder(this, 300, 555, 2, 55));
        Spawn(new Ladder(this, 600, 555, 2, 55));
        Spawn(new Ladder(this, 900, 555, 2, 55));
        Spawn(Enemy::create(this, 80, 500, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 500, 200, Enemy::Kind::BALL1, -1));
        Spawn(new Powerup(this, 760, 300 - 64, Powerup::Kind::HEAL));

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND9;
        levelTime = 160;
        break;
    case 14:
        Spawn(new Block(this, 20, 500, 200, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 220, 500, 80, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 300, 500, 300, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 600, 500, 80, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 680, 500, 300, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 980, 500, 80, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 1060, 500, 200, 20, Block::Kind::PLATFORM_1));

        Spawn(Enemy::create(this, 90, 500, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 500, 200, Enemy::Kind::BALL1, 1));

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND10;
        levelTime = 160;
        break;
    case 15:
        for (int x = 0; x < 14; x++) {
            Spawn(new Block(this, x * 80 + 80, 200, 80, 20, Block::Kind::PLATFORM_2));
        }

        Spawn(new Block(this, 630, 80, 20, 120, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 320, 80, 20, 120, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 940, 80, 20, 120, Block::Kind::PLATFORM_1));

        Spawn(new Block(this, 20, 200, 60, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 1200, 200, 60, 20, Block::Kind::PLATFORM_1));

        Spawn(new Block(this, 480, 600, 320, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 560, 500, 160, 20, Block::Kind::PLATFORM_1));

        Spawn(Enemy::create(this, 150, 120, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 390, 120, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 770, 120, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 1120, 120, Enemy::Kind::BALL1, 1));

        Spawn(new Ladder(this, 605, 495, 3, 60));

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND12;
        levelTime = 160;
        break;
    case 16:
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

        Spawn(new Ice(this, 600, 390, 500, 10));

        Spawn(Enemy::create(this, 590, 120, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 1020, 120, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 660, 120, Enemy::Kind::BALL1, 1));

        Spawn(new Powerup(this, 1045, 240 - 64, Powerup::Kind::HEAL));
        Spawn(new Powerup(this, 705, 240 - 64, Powerup::Kind::HEAL));

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND14;
        levelTime = 160;
        break;
    case 17:
        for (int x = 0; x <= 62; x++) {
            Spawn(new Block(this, x * 80, 200, 80, 20, Block::Kind::PLATFORM_2));
        }

        Spawn(new Block(this, 320, 80, 20, 120, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 630, 80, 20, 120, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 940, 80, 20, 120, Block::Kind::PLATFORM_1));

        for (int x = 1; x < 3; x++) {
            for (int i = 1; i < 3; i++) {
                Spawn(Enemy::create(this, 40 + 40 * i, 80 + x * 35, Enemy::Kind::BALL3, 1));
                Spawn(Enemy::create(this, 360 + 40 * i, 80 + x * 35, Enemy::Kind::BALL3, 1));
                Spawn(Enemy::create(this, 670 + 40 * i, 80 + x * 35, Enemy::Kind::BALL3, 1));
                Spawn(Enemy::create(this, 980 + 40 * i, 80 + x * 35, Enemy::Kind::BALL3, 1));
            }
        }
        for (int i = 1; i <= 4; i++) {
            Spawn(new Powerup(this, (i * 313) - 150, 120, Powerup::Kind::HEAL));
        }

        player = new Player(this, { 200,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_D;
        backTexture = BACKGROUND13;
        levelTime = 160;
        break;
    case 18:
        Spawn(new Block(this, 20, 580, 100, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 120, 480, 20, 120, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 20, 460, 120, 20, Block::Kind::PLATFORM_1));

        Spawn(new Block(this, 140, 460, 100, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 240, 360, 20, 120, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 20, 340, 240, 20, Block::Kind::PLATFORM_1));

        Spawn(new Block(this, 260, 340, 100, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 360, 240, 20, 120, Block::Kind::PLATFORM_1));

        Spawn(new Block(this, 20, 220, 360, 20, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 380, 220, 120, 20, Block::Kind::PLATFORM_2));
        Spawn(new Block(this, 480, 80, 20, 140, Block::Kind::PLATFORM_1));

        Spawn(Enemy::create(this, 50, 530, Enemy::Kind::BALL2, -1));
        Spawn(Enemy::create(this, 50, 390, Enemy::Kind::BALL2, -1));
        Spawn(Enemy::create(this, 50, 290, Enemy::Kind::BALL2, -1));
        Spawn(Enemy::create(this, 50, 135, Enemy::Kind::BALL2, 1));
        Spawn(Enemy::create(this, 350, 120, Enemy::Kind::BALL2, -1));

        Spawn(new Block(this, 640, 420, 620, 20, Block::Kind::PLATFORM_1));

        Spawn(new Ladder(this, 640, 415, 4, 50));
        Spawn(new Powerup(this, 940, 420 - 64, Powerup::Kind::HEAL));

        Spawn(new Ice(this, 640, 410, 620, 10));
        Spawn(new Ice(this, 20, 690, 220, 10));


        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND15;
        levelTime = 160;
        break;
    case 19:
        for (int x = 1; x < 8; x++) {
            Spawn(Enemy::create(this, x * 60 + 200, 100 + x * 50, Enemy::Kind::BALL3, 1));
        }

        Spawn(new Block(this, 200, 500, 800, 20, Block::Kind::PLATFORM_1));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        Spawn(new Powerup(this, 620, 700 - 64, Powerup::Kind::HEAL));

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND18;
        levelTime = 160;
        break;
    case 20:
        for (int x = 0; x < 4; x++) {
            Spawn(new Block(this, 540 - x * 100, 280 + x * 100, 100, 20, Block::Kind::PLATFORM_1));
            Spawn(new Block(this, (1240 - (520 - x * 100)) - 80, 280 + x * 100, 100, 20, Block::Kind::PLATFORM_1));

            Spawn(new Ice(this, 540 - x * 100, 270 + (x * 100), 100, 10));
            Spawn(new Ice(this, (1240 - (520 - x * 100)) - 80, 270 + x * 100, 100, 10));

            if (x != 3) {
                Spawn(new Block(this, (1240 - (520 - x * 100)) - 0, 300 + x * 100, 20, 100, Block::Kind::PLATFORM_1));
                Spawn(new Block(this, 540 - x * 100, 300 + x * 100, 20, 100, Block::Kind::PLATFORM_1));
            }
            if (x <= 2) {
                Spawn(new Block(this, 320 - x * 100, 80 + x * 100, 20, 100, Block::Kind::PLATFORM_2));
                if (x == 2 || x == 1)
                    Spawn(new Block(this, 220 - x * 100, 160 + x * 100, 100, 20, Block::Kind::PLATFORM_2));
                else 
                    Spawn(new Block(this, 220 - x * 100, 160 + x * 100, 100, 20, Block::Kind::PLATFORM_1));

                Spawn(new Block(this, 1260 - (320 - x * 100), 80 + x * 100, 20, 100, Block::Kind::PLATFORM_2));


                if (x == 2 || x == 1)
                    Spawn(new Block(this, 1180 - (220 - x * 100), 160 + x * 100, 100, 20, Block::Kind::PLATFORM_2));
                else
                    Spawn(new Block(this, 1180 - (220 - x * 100), 160 + x * 100, 100, 20, Block::Kind::PLATFORM_1));
                

            }
        }
        Spawn(new Ladder(this, 603, 275, 6, 50));

        Spawn(new Block(this, 120, 80, 20, 200, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 220, 80, 20, 80, Block::Kind::PLATFORM_1));

        Spawn(new Block(this, 1140, 80, 20, 200, Block::Kind::PLATFORM_1));
        Spawn(new Block(this, 1040, 80, 20, 80, Block::Kind::PLATFORM_1));


        Spawn(Enemy::create(this, 55, 140, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 140, 140, Enemy::Kind::BALL2, -1));
        Spawn(Enemy::create(this, 280, 130, Enemy::Kind::BALL2, -1));

        Spawn(Enemy::create(this, 1200, 140, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 1080, 140, Enemy::Kind::BALL2, -1));
        Spawn(Enemy::create(this, 980, 140, Enemy::Kind::BALL2, -1));

        Spawn(Enemy::create(this, 620, 140, Enemy::Kind::BALL1, -1));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        Spawn(new Powerup(this, 620, 280 - 64, Powerup::Kind::HEAL));

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND16;
        levelTime = 160;
        break;
    case 21:

        Spawn(Enemy::create(this, 890, 140, Enemy::Kind::BALL1, 1));
        Spawn(Enemy::create(this, 280, 240, Enemy::Kind::BALL1, -1));
        Spawn(Enemy::create(this, 980, 440, Enemy::Kind::BALL1, -1));
        Spawn(Enemy::create(this, 920, 410, Enemy::Kind::BALL1, -1));

        player = new Player(this, { 600,720 - 20 - 64, 64, 64, 12, 10, 40, 54 });
        sprites.push_back(player);

        Spawn(new Ice(this, wallThickness, screenHeight - wallThickness - 10, screenWidth - 2 * wallThickness, 10));

        Spawn(new Powerup(this, 620, 280 - 64, Powerup::Kind::HEAL));

        backMusic = BACKGROUND_E;
        backTexture = BACKGROUND17;
        levelTime = 160;
        break;

    }
}

void Game::Unspawn() {
    std::for_each(begin(sprites), end(sprites), [](Sprite *s) {
        delete s;
    });
    sprites.clear();
    spriteMap.clear();
}

void Game::StartGame() {
    score = 0;
    lives = 5;
    RestartLevel();
}

void Game::RestartLevel() {
    Unspawn();
    Spawn();
    ChangeState(State::ACTIVE);
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
        case State::MOD_MENU:
            modMenu.Draw();
            break;
        case State::DIFFLVL_MENU:
            diffLvlMenu.Draw();
            break;
        case State::PAUSED:
            ingameMenu.Draw();
            break;
        case State::LEVEL_FINISHED:
            //DrawSequence("Great Job!");
            DrawEndLevel();
            break;
        case State::GAME_START:
            DrawGameStart();
            break;
        case State::GAME_OVER:
            //DrawSequence("Game Over!");
            DrawGameOver();
            break;
        case State::GAME_FINISHED:
            DrawEndGame();
            break;
        case State::LEVEL_SELECTOR:
            DrawLevelSelector();
            DrawIcons();
            break;
        case State::RANKING:
            DrawRanking();
            DrawIcons();
            break;
        case State::SAVE_MENU:
            saveMenu.Draw();
            break;
        case State::OVERWRITE_MENU:
            overwriteMenu.Draw();
            break;
        case State::GAME_SAVED:
            //DrawSequence("Game Saved");
            DrawGameSaved();
            break;
        case State::LOAD_MENU:
            loadMenu.Draw();
            DrawLoadMenu();
            break;
        case State::ERROR:
            //DrawSequence("Unable to save file");
            break;
    }

    EndDrawing();
}

void Game::DrawSprites() {
    for (size_t i = 0; i < sprites.size(); i++) {
        sprites[i]->Draw();
    }
}

void Game::DrawBackground() {
    if (textures[backTexture].width > 2560) {
        DrawTextureRec(textures[backTexture], backRec, { 0, 0 }, WHITE);
        if (backFrame++ % 8 == 0) {
            backRec.x = (backRec.x + 1280 < textures[backTexture].width) ? backRec.x + 1280 : backRec.x = 0;
            backFrame = 1;
        }
    } else {
        DrawTexture(textures[backTexture], 0, 0, WHITE);
    }
}

void Game::DrawPanel() {
    float y = wallThickness + 5;

    DrawRectangleRec({wallThickness, wallThickness, screenWidth - 2 * wallThickness, panelHeight}, ColorAlpha(BLACK, 0.6f));
    //DrawFPS(wallThickness + 10, y);
    DrawTextEx(font, TextFormat("Level: %i", level), { 40, y }, 35, 0, WHITE);
    DrawTextEx(font, TextFormat("Score: %03i", score), { 200, y }, 35, 0, WHITE);
    DrawTextEx(font, TextFormat("Lives: %i", lives), { 970, y }, 35, 0, WHITE);
    DrawTextEx(font, TextFormat("Time: %03i", elapsedLevelTime), { 1120, y }, 35, 0, WHITE);
    
    if (speedBoost != 1) {
        DrawTexture(textures[HUD_BOOST], 400, y, WHITE);
        if (lifeTime[0]) {
            std::time_t now = std::time(nullptr);
            DrawTextEx(font, TextFormat(": %i s", timeLeft[0]), { 425, y }, 35, 0, WHITE);
        }
    }
    if (multiWeapon != 0) {
        DrawTexture(textures[HUD_DOUBLE], 490, y, WHITE);
        if (lifeTime[1]) {
            std::time_t now = std::time(nullptr);
            DrawTextEx(font, TextFormat(": %i s", timeLeft[1]), { 515, y }, 35, 0, WHITE);
        }
    }
    if (stopTime) {
        DrawTexture(textures[POWERUP_TIME], 740, y, WHITE);
        if (lifeTime[2]) {
            std::time_t now = std::time(nullptr);
            DrawTextEx(font, TextFormat(": %i s", timeLeft[2]), { 775, y }, 35, 0, WHITE);
        }
    }
    switch (weaponType) {
    case 2:
        hudWeapons.x = 0;
        DrawTextureRec(textures[HUD_WEAPONS], hudWeapons, { 620, y }, WHITE);
        break;
    case 3:
        hudWeapons.x = hudWeapons.width;
        DrawTextureRec(textures[HUD_WEAPONS], hudWeapons, { 620, y }, WHITE);
        break;
    case 4:
        hudWeapons.x = 2 * hudWeapons.width;
        DrawTextureRec(textures[HUD_WEAPONS], hudWeapons, { 620, y }, WHITE);
        DrawTextEx(font, TextFormat("x%i", shootingLeft), { 590, y }, 35, 0, WHITE);
        break;
    default:
        break;
    }
}

void Game::DrawGameSaved() {
    if (sequenceFrameCounter > frameCounter + sequenceFrameCounter - 30)
        animVec.y += 50;
    else if (sequenceFrameCounter < frameCounter + 30)
        animVec.y -= 50;
    DrawRectanglePro({ screenWidth / 2, animVec.y, 1500, 1500 }, { 0, 0 }, 225, GRAY);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.6f));
    DrawTextEx(font, "Game saved", { 485, animVec.y - 1240 }, 66, 0, WHITE);
}

void Game::DrawGameStart() {
    animVec.x -= 42;

    DrawRectangle(-screenWidth + animVec.x, 0, screenWidth, screenHeight, GRAY);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.6f));
}

void Game::DrawEndLevel() {
    if (sequenceFrameCounter > frameCounter + sequenceFrameCounter - 30)
        animVec.y += 50;
    else if (sequenceFrameCounter < frameCounter + 30)
        animVec.y -= 50;
    DrawRectanglePro({ screenWidth / 2, animVec.y, 1500, 1500 }, { 0, 0 }, 225, GRAY);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.6f));
    DrawTextEx(font, "Great job!", { 497, animVec.y - 1240 }, 66, 0, RED);
    DrawTextEx(font, TextFormat("Time bonus: %d", timeBonus), { 480, animVec.y - 1140 }, 50, 0, WHITE);
    DrawTextEx(font, TextFormat("Your score: %d", score), { 475, animVec.y - 1100 }, 50, 0, WHITE);
}

void Game::DrawGameOver() {
    if (sequenceFrameCounter > frameCounter + sequenceFrameCounter - 30)
        animVec.y += 50;
    else if (sequenceFrameCounter < frameCounter + 30)
        animVec.y -= 50;
    DrawRectanglePro({ screenWidth / 2, animVec.y, 1500, 1500}, { 0, 0 }, 225, GRAY);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.6f));
    DrawTextEx(font, "Game over", { 497, animVec.y - 1240 }, 66, 0, RED);
    DrawTextEx(font, TextFormat("Your score: %d", score), { 475, animVec.y - 1120 }, 50, 0, WHITE);
}

void Game::DrawEndGame() {
    if (sequenceFrameCounter > frameCounter + sequenceFrameCounter - 30)
        animVec.y += 50;
    else if (sequenceFrameCounter < frameCounter + 30)
        animVec.y -= 50;
    DrawRectanglePro({ screenWidth / 2, animVec.y, 1500, 1500 }, { 0, 0 }, 225, GRAY);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.6f));
    DrawTextEx(font, "Game finished - Superb job!", { 300, animVec.y - 1240 }, 66, 0, RED);
    DrawTextEx(font, TextFormat("Time bonus: %d", timeBonus), { 490, animVec.y - 1140 }, 50, 0, WHITE);
    DrawTextEx(font, TextFormat("Your score: %d", score), { 475, animVec.y - 1100 }, 50, 0, WHITE);
    DrawTextEx(font, TextFormat("Highest score: %d", rankScore[0]), { 475, animVec.y - 1060 }, 50, 0, WHITE);
}

void Game::DrawLoadMenu() {
    if (endAnim) {
        animVec.x += 50;
        animColor = 0;
    } else if (animVec.x > 50) {
        animVec.x -= 50;
        animColor = 0;
    } else {
        animVec.x = 0;
        animColor = (animColor < 1) ? animColor + 0.05f : animColor = 1;
    }
    int pos = loadMenu.GetPosition();
    time_t modTime;
    if (FileExists(TextFormat("saves/s%d.psf", pos + 1))) {
        modTime = GetFileModTime(TextFormat("saves/s%d.psf", pos + 1));
        char buf[100];
        errno_t e = ctime_s(buf, 100, &modTime);
        DrawRectangle(680 + animVec.x, 330, 550, 150, DARKGRAY);
        DrawTextEx(font, TextFormat("Date: %s", buf), { 690 + animVec.x, 350 }, 40, 0, Fade(WHITE, animColor));
    }
}

void Game::DrawLevelSelector() {
    if (endAnim) {
        animVec.x += 50;
        animColor = 0;
        if (animVec.x >= screenWidth) {
            ChangeState(State::MAIN_MENU);
            endAnim = false;
        }
    } else if (animVec.x > 50) {
        animVec.x -= 50;
        animColor = 0;
    } else {
        animVec.x = 0;
        animColor = (animColor < 1) ? animColor + 0.05f : animColor = 1;
    }
    DrawRectangle(animVec.x, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.8f));
    DrawTextEx(font, "Select level", { 460 + animVec.x, 75 }, 66, 0, Fade(PINK, 50 / (animVec.x + 1)));
    if (modNum == 2) {
        float xOffset = 210;
        float y = 300;
        float width = 40;
        float height = 40;
        float space = 10;
        for (int i = 0; i < 15; i++) {
            float x = xOffset + i * (width + space) + 50 * (i / 5);
            Color color = GREEN;
            if (i > 4) {
                if (i < 10)
                    color = YELLOW;
                else
                    color = RED;
            }
            DrawRectangleRec({ x + animVec.x, y, width, height }, color);
            DrawTextEx(font, TextFormat("%02i", i + 1), { x + 6 + animVec.x, y + 5}, 30, 0, BLACK);
        }
        DrawRectangleRounded({ xOffset + (level - 1) * (width + space) - 10 + 50 * ((level - 1) / 5) + animVec.x, y - 10, width + 20, height + 20 }, 0.2f, 8, ColorAlpha(ORANGE, 0.6));
        DrawTextEx(font, "Easy", { xOffset + 70 + animVec.x, y + 100 }, 50, 0, Fade(GREEN, 50 / (animVec.x + 1)));
        DrawTextEx(font, "Normal", { xOffset + 340 + animVec.x, y + 100 }, 50, 0, Fade(YELLOW, 50 / (animVec.x + 1)));
        DrawTextEx(font, "Hard", { xOffset + 670 + animVec.x, y + 100 }, 50, 0, Fade(RED, 50 / (animVec.x + 1)));
    } else {
        float xOffset = 450;
        float y = 300;
        float width = 40;
        float height = 40;
        float space = 20;
        for (int i = 15; i < NUM_LEVELS; i++) {
            float x = xOffset + (i - 15) * (width + space);
            Color color = GOLD;
            DrawRectangleRec({ x + animVec.x, y, width, height }, color);
            DrawTextEx(font, TextFormat("%02i", i - 14), { x + 6 + animVec.x, y + 5}, 30, 0, BLACK);
        }
        DrawRectangleRounded({ xOffset + (level - 16) * (width + space) - 10 + animVec.x, y - 10, width + 20, height + 20 }, 0.2f, 8, ColorAlpha(RED, 0.6));
        DrawTextEx(font, "Special levels", { 470 + animVec.x, y + 100 }, 50, 0, Fade(GOLD, 50 / (animVec.x + 1)));
    }
}

void Game::DrawRanking() {
    if (endAnim) {
        animVec.x += 50;
        animColor = 0;
        if (animVec.x >= screenWidth) {
            ChangeState(State::MAIN_MENU);
            endAnim = false;
        }
    } else if (animVec.x > 50) {
        animVec.x -= 50;
        animColor = 0;
    } else {
        animVec.x = 0;
        animColor = (animColor < 1) ? animColor + 0.05f : animColor = 1;
    }
    DrawRectangle(animVec.x, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.8f));
    DrawTextEx(font, "Ranking", { 500 + animVec.x, 75 }, 66, 0, Fade(PINK, 50 / (animVec.x + 1)));
    for (int i = 0; i < 10; i++) {
        int x = (i == 9) ? x = 480 : x = 490;
        if (rankScore[i] != 0)
            DrawTextEx(font, TextFormat("%i. %03i", i + 1, rankScore[i]), { x + animVec.x, 150.0f + i * 50 }, 60, 0, Fade(WHITE, animColor));
    }
}

void Game::DrawIcons() {
    if (state != State::MOD_MENU) {
        float x = (state == State::RANKING) ? x = 70 : x = 250;
        DrawTexture(textures[ESC_ICON], x, screenHeight - 45, WHITE);
        DrawTextEx(font, "Back", { x + 40, screenHeight - 50 }, 40, 0, WHITE);
    }
    if (state != State::RANKING) {
        DrawTexture(textures[ENTER_ICON], 70, screenHeight - 50, WHITE);
        DrawTextEx(font, "Select", { 120, screenHeight - 50 }, 40, 0, WHITE);
    }
    if (state == State::LOAD_MENU) {
        DrawTexture(textures[BACKSPACE_ICON], 390, screenHeight - 45, WHITE);
        DrawTextEx(font, "Delete", { 443, screenHeight - 50 }, 40, 0, WHITE);
    }
}

void Game::WriteGameData(std::ofstream& s) {
    Write(s, &level);
    Write(s, &lives);
    Write(s, &score);
    Write(s, &weaponType);
    Write(s, &previousWeapon);

    Write(s, &speedBoost);
    Write(s, &multiWeapon);
    Write(s, &stopTime);

    Write(s, &backTexture);
    Write(s, &backMusic);

    Write(s, &levelTime);
    Write(s, &elapsedLevelTime);
    for (int i = 0; i < 3; i++)
        Write(s, &timeLeft[i]);

    size_t numSprites = sprites.size();
    Write(s, &numSprites);

    for (auto& sprite : sprites) {
        sprite->Save(s);
    }
}

void Game::ReadGameData(std::ifstream& s) {
    Read(s, &level);
    Read(s, &lives);
    Read(s, &score);
    Read(s, &weaponType);
    Read(s, &previousWeapon);

    Read(s, &speedBoost);
    Read(s, &multiWeapon);
    Read(s, &stopTime);

    Read(s, &backTexture);
    Read(s, &backMusic);

    Read(s, &levelTime);
    Read(s, &elapsedLevelTime);
    std::time_t now = std::time(nullptr);
    endLevelTime = now + elapsedLevelTime;

    for (int i = 0; i < 3; i++) {
        Read(s, &timeLeft[i]);
        lifeTime[i] = now + timeLeft[i];
    }

    size_t numSprites = 0;
    Read(s, &numSprites);

    for (int i = 0; i < numSprites; i++) {
        int type;
        Read(s, &type);
        switch (static_cast<Sprite::Type>(type)) {
        case Sprite::Type::PLAYER:
            player = new Player(this, s);
            sprites.push_back(player);
            break;
        case Sprite::Type::BLOCK:
            Spawn(new Block(this, s));
            break;
        case Sprite::Type::ENEMY:
            Spawn(new Enemy(this, s));
            break;
        case Sprite::Type::POWERUP:
            Spawn(new Powerup(this, s));
            break;
        case Sprite::Type::ICE:
            Spawn(new Ice(this, s));
            break;
        case Sprite::Type::LADDER:
            Spawn(new Ladder(this, s));
            break;
        case Sprite::Type::WEAPON:
            Spawn(new Weapon(this, s));
            break;
        }
    }
}

bool Game::SaveGame(int fileNum) {
    if (!DirectoryExists("saves")) {
        std::system("mkdir saves");
    }
    std::ofstream saveFile(TextFormat("saves/s%d.psf", fileNum), std::ios_base::binary);
    if (saveFile.is_open()) {
        WriteGameData(saveFile);

        saveFile.close();

        std::cout << "Save file created";
        return true;
    } else {
        std::cout << "Unable to open file";
        return false;
    }
}

void Game::LoadGame(int fileNum) {
    if (!DirectoryExists("saves")) {
        std::cout << "No saves";
    } else {
        std::ifstream loadFile(TextFormat("saves/s%d.psf", fileNum), std::ios_base::binary);
        if (loadFile.is_open()) {
            ReadGameData(loadFile);

            loadFile.close();
            std::cout << "Load complete\n";
            PlaySound(audio[LEVEL_START]);
            if (backMusic != NUM_MUSIC)
                PlayMusicStream(music[backMusic]);
        } else {
            std::cout << "Unable to open file";
        }
    }
}

void Game::SaveUsrData() {
    if (!DirectoryExists("saves")) {
        std::system("mkdir saves");
    }
    std::ofstream saveFile("saves/s0.psf", std::ios_base::binary);
    if (saveFile.is_open()) {
        for (int i = 0; i < 10; i++) {
            Write(saveFile, &rankScore[i]);
        }
        saveFile.close();
    }
}

void Game::LoadUsrData() {
    if (DirectoryExists("saves")) {
        std::ifstream loadFile("saves/s0.psf", std::ios_base::binary);
        if (loadFile.is_open()) {
            for (int i = 0; i < 10; i++) {
                Read(loadFile, &rankScore[i]);
                if (rankScore[i] < 0)
                    rankScore[i] = 0;
            }
            loadFile.close();
        }
    }
}

void Game::RemoveSave(int i) {
    if (FileExists(TextFormat("saves/s%d.psf", i))) {
        system("cd");
        system(TextFormat("del saves\\s%d.psf", i));
        if (FileExists(TextFormat("saves/s%d.psf", i)))
            system(TextFormat("rm saves/s%d.psf", i));
        else
            printf("Success");
    }
    loadMenu.Reload(FindLoadFiles());
}

std::vector<std::string> Game::FindLoadFiles() {
    std::vector<std::string> fileNames;

    for (int i = 1; i < 5; i++) {
        if (FileExists(TextFormat("saves/s%d.psf", i)))
            fileNames.push_back(TextFormat("Save %d", i));
    }
    if (fileNames.size() == 0)
        fileNames.push_back("No saves");

    return fileNames;
}

std::vector<std::string> Game::FindSaveFiles() {
    std::vector<std::string> fileNames;
    numFiles = 0;

    for (int i = 1; i < 5; i++) {
        if (FileExists(TextFormat("saves/s%d.psf", i))) {
            fileNames.push_back(TextFormat("Save %d", i));
            numFiles++;
        } else
            fileNames.push_back("Empty slot");
    }
    return fileNames;
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
        PlaySound(audio[HARPUN]);
        break;
    case 2:
        if (laserCooldown)
            return;
        laserCooldown = 15;
        weapon = new Weapon(this, x, y, Weapon::Kind::WEAPON2);
        spriteMap[Sprite::Type::WEAPON].push_back(weapon);
        sprites.push_back(weapon);
        PlaySound(audio[LASER]);
        break;
    case 3:
        weapon = new Weapon(this, x, y, Weapon::Kind::WEAPON3);
        spriteMap[Sprite::Type::WEAPON].push_back(weapon);
        sprites.push_back(weapon);
        PlaySound(audio[HARPUN]);
        break;
    case 4:
        weapon = new Weapon(this, x, y, Weapon::Kind::WEAPON4);
        spriteMap[Sprite::Type::WEAPON].push_back(weapon);
        sprites.push_back(weapon);
        PlaySound(audio[MINE]);
        break;
    }
}

void Game::AddScore(int score) {
    this->score += score;
}

void Game::AddPowerup(float x, float y) {
    int chance = GetRandomValue(1, 4);
    if (chance == 1) {
        int kindNum;
        if (modNum != 3) {
            if (weaponType == 2)
                kindNum = GetRandomValue(2, 4);
            else
                kindNum = GetRandomValue(1, 4);
        } else {
            if (weaponType == 2)
                kindNum = GetRandomValue(2, 5);
            else
                kindNum = GetRandomValue(1, 5);
        }
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
        case 5:
            s = new Powerup(this, x, y, Powerup::Kind::SCORE);
            spriteMap[Sprite::Type::POWERUP].push_back(s);
            sprites.push_back(s);
        }
    }
}

void Game::PickAction(Powerup::Kind kind) {
    PlaySound(audio[POWERUP]);
    switch (kind) {
    case Powerup::Kind::BOOST:
        speedBoost = 2;
        timeLeft[0] = 5;
        lifeTime[0] = std::time(nullptr) + timeLeft[0];
        break;
    case Powerup::Kind::DOUBLE:
        if (weaponType == 4)
            shootingLeft *= 2;
        else {
            multiWeapon = 1;
            timeLeft[1] = 5;
            lifeTime[1] = std::time(nullptr) + timeLeft[1];
        }
        break;
    case Powerup::Kind::HEAL:
        lives++;
        break;
    case Powerup::Kind::TIME:
        stopTime = true;
        timeLeft[2] = 5;
        lifeTime[2] = std::time(nullptr) + timeLeft[2];
        break;
    case Powerup::Kind::SCORE:
        AddScore(1000);
        break;
    case Powerup::Kind::WEAPON:
        int change = GetRandomValue(2, 4);
        if (change == weaponType)
            weaponType = 1;
        else if (change == 4) {
            previousWeapon = weaponType;
            weaponType = change;
            shootingLeft = 5;
            break;
        } else
            weaponType = change;

        shootingLeft = 0;
        break;
    }
}

void Game::CheckTime() {
    std::time_t now = std::time(nullptr);
    if (state == State::PAUSED || state == State::SAVE_MENU) {
        for (int i = 0; i < 3; i++)
            lifeTime[i] = now + timeLeft[i];
    } else {
        for (int i = 0; i < 3; i++) {
            timeLeft[i] = lifeTime[i] - now;
            if (timeLeft[i] <= 0) {
                timeLeft[i] = 0;
                lifeTime[i] = 0;
                switch (i) {
                case 0:
                    speedBoost = 1;
                    break;
                case 1:
                    multiWeapon = 0;
                    break;
                case 2:
                    stopTime = false;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

std::vector<Sprite*> Game::GetSprites(Sprite::Type type) {
    if (spriteMap.find(type) == spriteMap.end())
        return {};
    return spriteMap[type];
}
