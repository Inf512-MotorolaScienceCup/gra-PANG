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
    Menu(Game* game, std::vector<std::string> items);
    void Reload(Game* game, std::vector<std::string> items);
    void Draw();
    void Update();

    std::string selected;

  private:
    std::vector<MenuItem> graphics;
    Game* game;
    int position = 0;
    Rectangle bgRec;
};
