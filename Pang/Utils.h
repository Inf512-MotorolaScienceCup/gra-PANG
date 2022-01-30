#pragma once

#include "raylib.h"
#include <fstream>

template <typename T>
void Write(std::ofstream& os, T* data) {
    os.write((const char*)data, sizeof(T));
}

template <typename T>
void Read(std::ifstream& is, T* data) {
    char buffer[20];
    is.read(buffer, sizeof(T));
    *data = *(T*)buffer;
}

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
    Position(std::ifstream& input) {
        int t;
        Read(input, &t);
        type = static_cast<Position::Type>(t);

        if (type == Type::CIRCLE) {
            Read(input, &center.x);
            Read(input, &center.y);
            Read(input, &radius);
        }
        else {
            Read(input, &rectangle.x);
            Read(input, &rectangle.y);
            Read(input, &rectangle.width);
            Read(input, &rectangle.height);
            Read(input, &hbRectangle.x);
            Read(input, &hbRectangle.y);
            Read(input, &hbRectangle.width);
            Read(input, &hbRectangle.height);
        
            //printf("Read x:%f y:%f x:%f y:%f\n", rectangle.x, rectangle.y, hbRectangle.x, hbRectangle.y);
        }
    }

    Rectangle hitBox() const {
        return {rectangle.x + hbRectangle.x, rectangle.y + hbRectangle.y, hbRectangle.width, hbRectangle.height};
    }

    void Save(std::ofstream& output) {
        int t = static_cast<int>(type);
        Write(output, &t);

        if (type == Type::CIRCLE) {
            Write(output, &center.x);
            Write(output, &center.y);
            Write(output, &radius);
        } else {
            Write(output, &rectangle.x);
            Write(output, &rectangle.y);
            Write(output, &rectangle.width);
            Write(output, &rectangle.height);            
            Write(output, &hbRectangle.x);
            Write(output, &hbRectangle.y);
            Write(output, &hbRectangle.width);
            Write(output, &hbRectangle.height);

            //printf("Write x:%f y:%f x:%f y:%f\n", rectangle.x, rectangle.y, hbRectangle.x, hbRectangle.y);
        }
    }
};
