/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Utils
*/

#ifndef UTILS_HPP_
#define UTILS_HPP_

namespace Engine {
    namespace Utils {
        // typedef struct Vec2_s {
        //     float x;
        //     float y;
        // } Vec2;

        class Vec2
        {
            public:
                Vec2() {};
                Vec2(float x, float y) {
                    this->x = x;
                    this->y = y;
                };

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
