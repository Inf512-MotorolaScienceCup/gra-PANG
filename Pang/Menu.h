#pragma once

#include <string>
#include <vector>

#include "raylib.h"

class Game;

class Menu {
    struct MenuItem {
        std::string name;
        Rectangle rec;
    };

public:
    enum class Type{
        MAIN_MENU,
        PAUSE_MENU,
        LOAD_MENU
    } type;

    Menu(Game* game, std::vector<std::string> items, Type type);
    void Reload(std::vector<std::string> items);
    void Draw();
    void Update();
    int GetPosition();

    std::string selected;
    Vector2 animVec;

  private:
    std::vector<MenuItem> graphics;
    Game* game;
    int position = 0;
    const float MENU_WIDTH = 300;
    float MENU_HEIGHT;
};
