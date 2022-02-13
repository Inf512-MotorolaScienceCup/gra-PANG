#pragma once

#include <map>
#include <vector>
#include <ctime>
#include <fstream>

#include "raylib.h"

#include "Menu.h"
#include "Sprite.h"

enum TextureTypes {
    BACKGROUND_ANI,
    BACKGROUND1,
    BACKGROUND2,
    BACKGROUND3,
    BACKGROUND4,
    BACKGROUND5,
    BACKGROUND6,
    BACKGROUND7,
    BACKGROUND8,
    BACKGROUND9,
    BACKGROUND10,
    BACKGROUND11,
    BACKGROUND12,
    BACKGROUND13,
    BACKGROUND14,
    BACKGROUND15,
    BACKGROUND16,
    BACKGROUND17,
    BACKGROUND18,
    BACKGROUND19,
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
    HUD_WEAPONS,
    HUD_BOOST,
    HUD_DOUBLE,
    ICE,

    NUM_TEXTURES
};

enum AudioTypes {
    BALL_BOUNCE,
    GAME_OVER,
    HEALTH_LOSE,
    LASER,
    LEVEL_START,
    LEVEL_COMPLETED,
    POWERUP,
    WALL_DESTRACTION,
    BALL_BREAKING,
    MENU_SELECT,
    MENU_ENTER,
    WALK,
    HARPUN,

    NUM_AUDIO
};

enum MusicTypes {
    BACKGROUND_A,
    BACKGROUND_B,
    BACKGROUND_C,
    BACKGROUND_D,
    BACKGROUND_E,

    NUM_MUSIC
};

class Game {
public:
    enum class State {
        MAIN_MENU,
        MOD_MENU,
        DIFFLVL_MENU,
        LEVEL_SELECTOR,
        SAVE_MENU,
        OVERWRITE_MENU,
        LOAD_MENU,
        ACTIVE,
        PAUSED,
        LEVEL_FINISHED,
        GAME_OVER,
        GAME_FINISHED,
        GAME_SAVED,
        ERROR
    } state;

    const char *textureFiles[NUM_TEXTURES] = {"res/Background/forestHD.png",
                                              "res/Background/bagno1.png",
                                              "res/Background/jungla1.png",
                                              "res/Background/jungla2.png",
                                              "res/Background/jungla3.png",
                                              "res/Background/las1.png",
                                              "res/Background/las2.png",
                                              "res/Background/las3.png",
                                              "res/Background/morze1.png",
                                              "res/Background/pustynia1.png",
                                              "res/Background/pustynia2.png",
                                              "res/Background/pustynia3.png",
                                              "res/Background/pustynia4.png",
                                              "res/Background/gory.png",
                                              "res/Background/miasto1.png",
                                              "res/Background/zima1.png",
                                              "res/Background/zima2.png",
                                              "res/Background/zima3.png",
                                              "res/Background/zima4.png",
                                              "res/Background/zima5.png",
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
                                              "res/weapons/line_straight.png",
                                              "res/weapons/line_wiggled.png",
                                              "res/weapons/ending.png",
                                              "res/weapons/shot.png",
                                              "res/weapons/mine.png",
                                              "res/weapons/explosion.png",
                                              "res/powerups/clock.png",
                                              "res/powerups/boost.png",
                                              "res/powerups/anchor.png",
                                              "res/powerups/double.png",
                                              "res/powerups/heart.png",
                                              "res/hud/weapons.png",
                                              "res/hud/boost.png",
                                              "res/hud/double.png",
                                              "res/ice.png"
                                              };
    Texture2D textures[NUM_TEXTURES];

    const char *audioFiles[NUM_AUDIO] = { "res/audio/bounce.mp3",
                                          "res/audio/game_over.mp3",
                                          "res/audio/health_lose.mp3",
                                          "res/audio/laser.mp3",
                                          "res/audio/level_start.mp3",
                                          "res/audio/level_completed.mp3",
                                          "res/audio/powerup.mp3",
                                          "res/audio/wall_destruction.mp3",
                                          "res/audio/ball-breaking.mp3",
                                          "res/audio/menu_select.mp3",
                                          "res/audio/menu_enter.mp3",
                                          "res/audio/walk.mp3",
                                          "res/audio/harpun.mp3"
    };
    Sound audio[NUM_AUDIO];

    const char *musicFiles[NUM_MUSIC] = {
        "res/audio/background1.mp3",
        "res/audio/background2.mp3",
        "res/audio/background3.mp3",
        "res/audio/background4.mp3",
        "res/audio/background5.mp3"
    };
    Music music[NUM_MUSIC];

    const float screenWidth = 1280;
    const float screenHeight = 720;
    const float wallThickness = 20;
    const float panelHeight = 40;
    const int NUM_LEVELS = 21;

    Game();
    ~Game();

    void LoadTextures();
    void LoadAudio();
    void Unload();

    int MainLoop();
    void Update();
    void ChangeState(State newState);
    void CheckCollision();
    void MoveSprites();

    void Spawn();
    void Spawn(Sprite *sprite);
    void SpawnLevel();
    void Unspawn();
    void StartGame(int newLevel = 1);
    void RestartLevel();

    void Draw();
    void DrawSprites();
    void DrawBackground();
    void DrawPanel();
    void DrawSequence(const char* message);
    void DrawEndLevel();
    void DrawGameOver();
    void DrawEndGame();
    void DrawLevelSelector();

    void WriteGameData(std::ofstream&);
    void ReadGameData(std::ifstream&);
    bool SaveGame(int);
    void LoadGame(int);
    void SaveUsrData();
    void LoadUsrData();
    std::vector<std::string> FindLoadFiles();
    std::vector<std::string> FindSaveFiles();

    void AddEnemy(float, float, Enemy::Kind, int);
    void AddWeapon(float, float, int);
    void AddScore(int);
    void AddPowerup(float, float);
    void PickAction(Powerup::Kind);
    void CheckTime();

    std::vector<Sprite*> GetSprites(Sprite::Type type);

    short int numFiles;

    std::vector<Sprite *> sprites;
    std::map<Sprite::Type, std::vector<Sprite *>> spriteMap;
    Player *player;
    Weapon *weapon;

    TextureTypes backTexture;
    Rectangle backRec = { 0, 0, 1280, 720 };
    short int backFrame = 0;

    MusicTypes backMusic = NUM_MUSIC;

    Rectangle hudWeapons = { 0, 0, 32, 32 };

    int frameCounter = 0;
    int sequenceFrameCounter;
    Menu mainMenu;
    Menu ingameMenu;
    Menu saveMenu;
    Menu overwriteMenu;
    Menu loadMenu;
    Menu modMenu;
    Menu diffLvlMenu;

    short int modNum;
    short int saveNum;

    int level = 1;
    int lives;
    int score;
    int highScore;

    std::time_t endLevelTime;
    int levelTime;
    int elapsedLevelTime;

    short int weaponType = 1;
    int shootingLeft;

    std::time_t timeLeft[4];
    bool stopTime;
    short int speedBoost;
    short int multiWeapon = 0;
};

