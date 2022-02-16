#include <iostream>

#include "raylib.h"
#include "Menu.h"
#include "Game.h"

Menu::Menu(Game* game, std::vector<std::string> items, Type type)
    : game(game), type(type) {
    Reload(items);
}

void Menu::Reload(std::vector<std::string> items) {
    if (graphics.size() > 0)
        graphics.clear();

    float itemHeight = 50;

    if (type == Type::MAIN_MENU) {
        float MENU_X = 70;
        float MENU_Y = game->screenHeight / 2 + 50;

        int i = 0;
        for (auto item : items) {
            graphics.push_back({
                item,{
                    MENU_X,
                    MENU_Y + i++ * itemHeight,
                    MENU_WIDTH,
                    itemHeight
                }
                });
        }
        
    } else {
        float MENU_X = game->screenWidth / 2 - 200;
        float MENU_Y = game->screenHeight / 2 - 100;

        int i = 0;
        for (auto item : items) {
            graphics.push_back({
                item,{
                    MENU_X + i * 80,
                    MENU_Y + i++ * itemHeight,
                    MENU_WIDTH,
                    itemHeight
                }
                });
        }
    }
}

void Menu::Draw() {
    static Color bgColor = Fade(BLACK, 0.8);

    if (type == Type::PAUSE_MENU) {
        DrawTriangle({ 0, 50 }, { game->screenWidth, game->screenHeight - 50 }, { game->screenWidth, 50 }, GRAY);
        DrawRectangle(0, 0, game->screenWidth, 50, GRAY);
        DrawRectangle(0, 0, game->screenWidth, game->screenHeight, bgColor);
        

        // Draw selected item
        const MenuItem& item = graphics[position];
        DrawRectangleRounded({ item.rec.x - 40, item.rec.y + item.rec.height / 2, 25, 5 }, 1, 1, WHITE);
        DrawRectangleRounded({ item.rec.x + item.name.size() * 30, item.rec.y + item.rec.height / 2, 25, 5 }, 1, 1, WHITE);
        DrawTextEx(game->font, item.name.c_str(), { item.rec.x, item.rec.y }, 60, 0, PINK);

        for (const auto& g : graphics) {
            if (item.name.c_str() != g.name.c_str())
                DrawTextEx(game->font, g.name.c_str(), { g.rec.x, g.rec.y }, 54, 0, WHITE);
        }
        
        DrawTexture(game->textures[BALL_MENU], 1000, 40, WHITE);
    } else {
        // Draw selected item
        const MenuItem& item = graphics[position];
        DrawRectangleRounded({ item.rec.x - 40, item.rec.y + item.rec.height / 2, 25, 5 }, 1, 1, WHITE);
        DrawTextEx(game->font, item.name.c_str(), { item.rec.x, item.rec.y }, 60, 0, PINK);

        for (const auto& g : graphics) {
            if (g.name.c_str() != item.name.c_str())
                DrawTextEx(game->font, g.name.c_str(), { g.rec.x, g.rec.y }, 54, 0, WHITE);
        }
    }


}

void Menu::Update() {
    if (IsKeyPressed(KEY_UP)) {
        position = (position <= 0) ? position = graphics.size() - 1 : position - 1;
        PlaySound(game->audio[MENU_SELECT]);
    } else if (IsKeyPressed(KEY_DOWN)) {
        position = (position >= graphics.size() - 1) ? position = 0 : position + 1;
        PlaySound(game->audio[MENU_SELECT]);
    } else if (IsKeyPressed(KEY_ENTER)) {
        selected = graphics[position].name;
        PlaySound(game->audio[MENU_ENTER]);
    }
}
