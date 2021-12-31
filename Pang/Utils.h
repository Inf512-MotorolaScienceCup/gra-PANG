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
    Position(float x, float y, float width, float height)
        : type(Type::RECTANGLE), rectangle{ x, y, width, height } {
    }
    Position(float x, float y, float radius)
        : type(Type::CIRCLE), center{ x, y }, radius{ radius } {
    }
};