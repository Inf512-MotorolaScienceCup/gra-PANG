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
        float MENU_X = game->screenWidth / 2 - 50 * items.size();
        float MENU_Y = game->screenHeight / 2 - 25 * items.size();

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
    static Color bgColor = Fade(BLACK, 0.8f);

    if (type == Type::PAUSE_MENU) {
        Vector2 animSpeed = { 123, 67 };
        if (!game->endAnim) {
            if (animVec.x - animSpeed.x > 0 && animVec.y - animSpeed.y > 0) {
                animVec.x -= animSpeed.x;
                animVec.y -= animSpeed.y;
            }
            else
                animVec = { 0, 0 };
        }
        else {
            if (animVec.x < game->screenWidth && animVec.y < game->screenHeight - 50) {
                animVec.x += animSpeed.x;
                animVec.y += animSpeed.y;
            }
            else {
                animVec = { game->screenWidth, game->screenHeight - 50 };
                game->ChangeState(game->newAnim);
                game->endAnim = false;
            }
        }
        DrawTriangle({ 0 - animVec.x, 50 - animVec.y }, { game->screenWidth - animVec.x, game->screenHeight - 50 - animVec.y }, { game->screenWidth - animVec.x, 50 - animVec.y }, GRAY);
        DrawRectangle(0 - animVec.x, 0 - animVec.y, game->screenWidth, 50, GRAY);
        DrawRectangle(0, 0, game->screenWidth, game->screenHeight, bgColor);

        // Draw selected item
        const MenuItem& item = graphics[position];
        Color recColor;
        Color selColor;
        if (animVec.y != 0) {
            recColor = Fade(WHITE, 100 / animVec.y);
            selColor = Fade(PINK, 100 / animVec.y);
        }
        else {
            recColor = WHITE;
            selColor = PINK;
        }
        short int textSpeed = 3;
        DrawRectangleRounded({ item.rec.x - 40 - animVec.x / textSpeed, item.rec.y + item.rec.height / 2, 25, 5 }, 1, 1, recColor);
        DrawRectangleRounded({ item.rec.x + item.name.size() * 30 - animVec.x / textSpeed, item.rec.y + item.rec.height / 2, 25, 5 }, 1, 1, recColor);
        DrawTextEx(game->font, item.name.c_str(), { item.rec.x - animVec.x / textSpeed, item.rec.y }, 60, 0, selColor);

        short int i = 2;
        for (const auto& g : graphics) {
            Color color;
            if (animVec.y != 0)
                color = Fade(WHITE, 50 / (animVec.y * i));
            else
                color = WHITE;

            if (item.name.c_str() != g.name.c_str())
                DrawTextEx(game->font, g.name.c_str(), { g.rec.x - animVec.x * i / textSpeed, g.rec.y }, 54, 0, color);

            i++;
        }
    } else if (type == Type::LOAD_MENU) {

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
