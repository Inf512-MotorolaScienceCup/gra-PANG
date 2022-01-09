#pragma once

#include <map>
#include <vector>
#include <ctime>

#include "raylib.h"

#include "Menu.h"
#include "Sprite.h"

enum TextureTypes {
    BACKGROUND,
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

  const char *textureFiles[NUM_TEXTURES] = {"res/Background/forest.gif",
                                            "res/spritesheet.png",
                                            "res/ladder.png",
                                            "res/wall.png",
                                            "res/platform1.png",
                                            "res/platform2.png",
                                            "res/Ball/Explosion/firefly_explosion.png",
                                            "res/Ball/Ball_1.png",
                                            "res/Ball/Ball_2.png",
                                            "res/Ball/Ball_3.png",
                                            "res/Ball/Ball_4.png"};
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
  void DrawPanel();
  void MoveSprites();
  void CheckCollision();
  void DrawSequence(const char* message);
  void ChangeState(State newState);
  void RestartLevel();
  void StartGame(int newLevel = 1);
  void DrawLevelSelector();

  Rectangle getPlayerPosition();

  void AddEnemy(float, float, Enemy::Kind, int);
  void AddWeapon(float, float);
  void AddScore(int);
  int MainLoop();
  void LoadTextures();

  std::vector<Sprite *> sprites;
  std::map<Sprite::Type, std::vector<Sprite *>> spriteMap;
  Player *player;
  Weapon *weapon;

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
};

