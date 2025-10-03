#pragma once
#include <cstdint>
#include <metapp/variant.h>
#include <cmath>

namespace Radium {

    class Vector2f {
    public:
        metapp::Variant x;
        metapp::Variant y;

        static void Register();

        Vector2f(float x, float y) : x({x}), y({y}) {};
        Vector2f() : x({0.0f}), y({0.0f}) {}

        // Vector operations declarations
        Vector2f operator+(const Vector2f& other) const;
        Vector2f& operator+=(const Vector2f& other);

        Vector2f operator-(const Vector2f& other) const;
        Vector2f& operator-=(const Vector2f& other);

        Vector2f operator*(float scalar) const;
        Vector2f& operator*=(float scalar);

        Vector2f operator/(float scalar) const;
        Vector2f& operator/=(float scalar);

        bool operator==(const Vector2f& other) const;
        bool operator!=(const Vector2f& other) const;

        float Length() const;
        float LengthSquared() const;
        float Dot(const Vector2f& other) const;
        Vector2f Normalize() const;
    };

    class Vector2i {
    public:
        metapp::Variant x;
        metapp::Variant y;

        static void Register();

        Vector2i(metapp::Variant x, metapp::Variant y);
        Vector2i() : x({0}), y({0}) {}

        // Vector operations declarations
        Vector2i operator+(const Vector2i& other) const;
        Vector2i& operator+=(const Vector2i& other);

        Vector2i operator-(const Vector2i& other) const;
        Vector2i& operator-=(const Vector2i& other);

        Vector2i operator*(int scalar) const;
        Vector2i& operator*=(int scalar);

        Vector2i operator/(int scalar) const;
        Vector2i& operator/=(int scalar);

        bool operator==(const Vector2i& other) const;
        bool operator!=(const Vector2i& other) const;

        int LengthSquared() const;
        int Dot(const Vector2i& other) const;
    };

    class RectangleF {
    public:
        metapp::Variant x;
        metapp::Variant y;
        metapp::Variant w;
        metapp::Variant h;

        static void Register();

        RectangleF(metapp::Variant x, metapp::Variant y, metapp::Variant w, metapp::Variant h);
        RectangleF() : x(0), y(0), w(0), h(0) {};
    };

}
