#pragma once

#include <map>
#include <vector>
#include <ctime>

#include "raylib.h"

#include "Menu.h"
#include "Sprite.h"

enum TextureTypes {
    BACKGROUND_ANI,
    //BACKGROUND1,
    //BACKGROUND2,
    //BACKGROUND3,
    //BACKGROUND4,
    //BACKGROUND5,
    //BACKGROUND6,
    //BACKGROUND7,
    //BACKGROUND8,
    //BACKGROUND9,
    //BACKGROUND10,
    //BACKGROUND11,
    //BACKGROUND12,
    //BACKGROUND13,
    //BACKGROUND14,
    //BACKGROUND15,
    //BACKGROUND16,
    //BACKGROUND17,
    //BACKGROUND18,
    //BACKGROUND19,
    PLAYER,
    LADDER,
    WALL,
    PLATFORM_1,
    PLATFORM_2,
    EXPLOSION,
    BALL_1,
    BALL_2,
    BALL_3,
    BALL_4,
    LINE_STRAIGHT,
    LINE_WIGGLED,
    WEAPON_ENDING,
    WEAPON_SHOT,
    WEAPON_MINE,
    MINE_EXPLOSION,
    POWERUP_TIME,
    POWERUP_BOOST,
    POWERUP_WEAPON,
    POWERUP_DOUBLE,
    POWERUP_HEAL,

    NUM_TEXTURES
};

class Game {
public:
    enum class State {
        MAIN_MENU,
        LEVEL_SELECTOR,
        PLAYER_DIED,
        ACTIVE,
        PAUSED,
        LEVEL_FINISHED,
        GAME_OVER,
        GAME_FINISHED
    } state;

    const char *textureFiles[NUM_TEXTURES] = {"res/Background/forestHD.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              //"res/Background/.png",
                                              "res/spritesheet.png",
                                              "res/ladder.png",
                                              "res/wall.png",
                                              "res/platform1.png",
                                              "res/platform2.png",
                                              "res/Ball/Explosion/enemyexplosion1.png",
                                              "res/Ball/Ball_1.png",
                                              "res/Ball/Ball_2.png",
                                              "res/Ball/Ball_3.png",
                                              "res/Ball/Ball_4.png",
                                              "res/hook/line_straight.png",
                                              "res/hook/line_wiggled.png",
                                              "res/hook/ending2.png",
                                              "res/hook/shot.png",
                                              "res/hook/mine.png",
                                              "res/hook/explosion.png",
                                              "res/powerups/clock.png",
                                              "res/powerups/boost.png",
                                              "res/powerups/anchor.png",
                                              "res/powerups/double.png",
                                              "res/powerups/heart.png"
                                              };
    Texture2D textures[NUM_TEXTURES];

    const float screenWidth = 1280;
    const float screenHeight = 720;
    const float wallThickness = 20;
    const float panelHeight = 40;
    const int NUM_LEVELS = 20;

    Game();
    ~Game();

    void Spawn(Sprite *sprite);
    void Spawn();
    void SpawnLevel();
    void Unspawn();
    void Draw();
    void Update();
    void DrawSprites();
    void DrawBackground();
    void DrawPanel();
    void MoveSprites();
    void CheckCollision();
    void DrawSequence(const char* message);
    void ChangeState(State newState);
    void RestartLevel();
    void StartGame(int newLevel = 1);
    void DrawLevelSelector();

    void AddEnemy(float, float, Enemy::Kind, int);
    void AddWeapon(float, float, int);
    void AddScore(int);
    void AddPowerup(float, float);
    void PickAction(Powerup::Kind);
    void CheckTime();
    int MainLoop();
    void LoadTextures();

    std::vector<Sprite *> sprites;
    std::map<Sprite::Type, std::vector<Sprite *>> spriteMap;
    Player *player;
    Weapon *weapon;

    TextureTypes backTexture;
    Rectangle backRec = { 0, 0, 1280, 720 };
    short int backFrame = 0;

    int level = 1;
    int lives = 3;
    int score = 0;

    int frameCounter = 0;
    int sequenceFrameCounter;
    Menu mainMenu;
    Menu ingameMenu;

    std::time_t endLevelTime;
    int levelTime;
    int elapsedLevelTime;

    short int weaponType = 0;
    int shootingLeft;

    std::time_t timeLeft[4];
    bool stopTime;
    short int speedBoost;
    short int multiWeapon = 0;
};

