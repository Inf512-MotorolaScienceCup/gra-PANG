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
    void Draw();
    void Update();

private:
    std::vector<MenuItem> graphics;
    Game* game;
    int position = 0;
    Rectangle bgRec;
};

class MainMenu {
    struct MenuItem {
        std::string name;
        Rectangle rec;
    };

public:
    MainMenu(Game* game, std::vector<std::string> items);
    void Draw();
    void Update();

private:
    std::vector<MenuItem> graphics;
    Game* game;
    int position = 0;
    Rectangle bgRec;
};
