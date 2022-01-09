#pragma once

#include "raylib.h"

struct Position {
    enum class Type {
        RECTANGLE,
        CIRCLE,
        // LINE
    } type;

    Vector2 center;
    float radius;
    Rectangle rectangle;
    Rectangle hbRectangle;
    Position(float x, float y, float width, float height)
        : type(Type::RECTANGLE), rectangle{x, y, width, height}, hbRectangle{0, 0, width, height} {}
    Position(float x, float y, float width, float height, float hbOffsetX, float hbOffsetY, float hbWidth, float hbHeight)
        : type(Type::RECTANGLE), rectangle{x, y, width, height}, hbRectangle{hbOffsetX, hbOffsetY, hbWidth, hbHeight} {}
    Position(float x, float y, float radius) : type(Type::CIRCLE), center{x, y}, radius{radius} {}
    Rectangle hitBox() const {
        return {rectangle.x + hbRectangle.x, rectangle.y + hbRectangle.y, hbRectangle.width, hbRectangle.height};
    }
};
