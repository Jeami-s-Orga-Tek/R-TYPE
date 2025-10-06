/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Utils
*/

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cstdint>

namespace Engine {
    namespace Utils {
        // typedef struct Vec2_s {
        //     float x;
        //     float y;
        // } Vec2;

        class Vec2
        {
            public:
                Vec2() = default;
                Vec2(float x, float y) : x(x), y(y) {}

                Vec2 operator+(Vec2 const& v) {
                    return (Vec2(x + v.x, y + v.y));
                }

                Vec2 operator+=(Vec2 const& v) {
                    x += v.x;
                    y += v.y;
                    return (*this);
                }

                Vec2 operator-(Vec2 const& v) {
                    return (Vec2(x - v.x, y - v.y));
                }

                Vec2 operator-=(Vec2 const& v) {
                    x -= v.x;
                    y -= v.y;
                    return (*this);
                }

                Vec2 operator*(float f) const {
                    return (Vec2(this->x * f, this->y * f));
                }

                float x = 0.0f;
                float y = 0.0f;
        };

        class Vec2Int
        {
            public:
                Vec2Int() = default;
                Vec2Int(int x, int y) : x(x), y(y) {}

                Vec2Int operator+(Vec2Int const& v) {
                    return (Vec2Int(x + v.x, y + v.y));
                }

                Vec2Int operator+=(Vec2Int const& v) {
                    x += v.x;
                    y += v.y;
                    return (*this);
                }

                Vec2Int operator-(Vec2Int const& v) {
                    return (Vec2Int(x - v.x, y - v.y));
                }

                Vec2Int operator-=(Vec2Int const& v) {
                    x -= v.x;
                    y -= v.y;
                    return (*this);
                }

                Vec2Int operator*(int f) const {
                    return (Vec2Int(this->x * f, this->y * f));
                }

                int x = 0;
                int y = 0;
        };

        class Vec2UInt
        {
            public:
                Vec2UInt() = default;
                Vec2UInt(unsigned int x, unsigned int y) : x(x), y(y) {}

                Vec2UInt operator+(Vec2UInt const& v) {
                    return (Vec2UInt(x + v.x, y + v.y));
                }

                Vec2UInt operator+=(Vec2UInt const& v) {
                    x += v.x;
                    y += v.y;
                    return (*this);
                }

                Vec2UInt operator-(Vec2UInt const& v) {
                    return (Vec2UInt(x - v.x, y - v.y));
                }

                Vec2UInt operator-=(Vec2UInt const& v) {
                    x -= v.x;
                    y -= v.y;
                    return (*this);
                }

                Vec2Int operator*(unsigned int f) const {
                    return (Vec2Int(this->x * f, this->y * f));
                }

                unsigned int x = 0;
                unsigned int y = 0;
        };

        class Rect
        {
            public:
                Rect() {}
                Rect(float x, float y, float width, float height)
                    : x(x), y(y), width(width), height(height) {}

                bool contains(float px, float py) const {
                    return (px >= x && px <= x + width && py >= y && py <= y + height);
                }

                bool contains(const Vec2& point) const {
                    return contains(point.x, point.y);
                }

                bool intersects(const Rect& other) const {
                    return !(x + width < other.x || other.x + other.width < x ||
                             y + height < other.y || other.y + other.height < y);
                }

                float x = 0.0f;
                float y = 0.0f;
                float width = 0.0f;
                float height = 0.0f;
        };

        class Color
        {
            public:
                Color() {};
                Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
                    this->r = r;
                    this->g = g;
                    this->b = b;
                }

                Color operator*(float f) const {
                    return (Color(this->r * f, this->g * f, this->b * f));
                }

                std::uint8_t r = 0;
                std::uint8_t g = 0;
                std::uint8_t b = 0;
        };

        
    };
};

#endif /* !UTILS_HPP_ */
