#include <iostream>

#include "raylib.h"
#include "Menu.h"
#include "Game.h"

const float MENU_WIDTH = 300;
const float MENU_HEIGHT = 200;

Menu::Menu(Game* game, std::vector<std::string> items)
    : game(game) {
    Reload(items);
}

void Menu::Reload(std::vector<std::string> items) {
    if (graphics.size() > 0)
        graphics.clear();

    bgRec.x = (game->screenWidth - MENU_WIDTH) / 2;
    bgRec.y = (game->screenHeight - MENU_HEIGHT) / 2;
    bgRec.width = MENU_WIDTH;
    bgRec.height = MENU_HEIGHT;

    float xMargin = 0.1 * MENU_HEIGHT;
    float yMargin = 0.1 * MENU_WIDTH;
    float itemHeight = (MENU_HEIGHT - 2 * yMargin) / items.size();

    int i = 0;
    for (auto item : items) {
        graphics.push_back({
            item,{
                bgRec.x + xMargin,
                bgRec.y + yMargin + i++ * (itemHeight + 2),
                MENU_WIDTH - 2 * xMargin,
                itemHeight
            }
            });
    }
}

void Menu::Draw() {
    static Color bgColor = Fade(BLACK, 0.8);
    static Color menuBgColor = Fade(GREEN, 0.8);

    DrawRectangle(0, 0, game->screenWidth, game->screenHeight, bgColor);
    DrawRectangleRounded(bgRec, 0.2, 8, menuBgColor);

    int i = 0;
    for (const auto& g : graphics) {
        DrawRectangleRounded(g.rec, 0.2, 8, menuBgColor);
        DrawText(g.name.c_str(), g.rec.x + 125 - 4 * g.name.size(), g.rec.y - 10 + g.rec.height / 2, 20, BLACK);
    }

  // Draw selected item
    const MenuItem& item = graphics[position];
    DrawRectangleRounded(item.rec, 0.2, 8, BLACK);
    DrawText(item.name.c_str(), item.rec.x + 127 - 4 * item.name.size(), item.rec.y - 10 + item.rec.height / 2, 20, menuBgColor);
}

void Menu::Update() {
    if (IsKeyPressed(KEY_UP)) {
        position = (position <= 0) ? position = 0 : position - 1;
        PlaySound(game->audio[MENU_SELECT]);
    } else if (IsKeyPressed(KEY_DOWN)) {
        position = (position >= graphics.size() - 1) ? position = graphics.size() - 1 : position + 1;
        PlaySound(game->audio[MENU_SELECT]);
    } else if (IsKeyPressed(KEY_ENTER)) {
        selected = graphics[position].name;
        PlaySound(game->audio[MENU_ENTER]);
    }
}
