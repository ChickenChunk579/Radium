#include <Radium/Math.hpp>
#include <cmath>
#include <spdlog/spdlog.h>

namespace Radium
{
    Vector2i::Vector2i(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    Vector2f::Vector2f(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    RectangleF::RectangleF(float x, float y, float w, float h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    float Vector2f::GetX()
    {
        return x;
    }

    void Vector2f::SetX(float val)
    {
        x = val;
    }

    float Vector2f::GetY()
    {
        return y;
    }

    void Vector2f::SetY(float val)
    {
        y = val;
    }

    int Vector2i::GetX()
    {
        return x;
    }

    void Vector2i::SetX(int val)
    {
        x = val;
    }

    int Vector2i::GetY()
    {
        return y;
    }

    void Vector2i::SetY(int val)
    {
        y = val;
    }

    float RectangleF::GetX()
    {
        return x;
    }

    void RectangleF::SetX(float val)
    {
        x = val;
    }

    float RectangleF::GetY()
    {
        return y;
    }

    void RectangleF::SetY(float val)
    {
        y = val;
    }

    float RectangleF::GetW()
    {
        return w;
    }

    void RectangleF::SetW(float val)
    {
        w = val;
    }

    float RectangleF::GetH()
    {
        return h;
    }

    void RectangleF::SetH(float val)
    {
        h = val;
    }

    Vector2f Vector2f::operator+(const Vector2f &other) const
    {
        return Vector2f(
            x + other.x,
            y + other.y);
    }

    Vector2f &Vector2f::operator+=(const Vector2f &other)
    {
        x = x + other.x;
        y = y + other.y;
        return *this;
    }

    Vector2f Vector2f::operator-(const Vector2f &other) const
    {
        return Vector2f(
            x - other.x,
            y - other.y);
    }

    Vector2f &Vector2f::operator-=(const Vector2f &other)
    {
        x = x - other.x;
        y = y - other.y;
        return *this;
    }

    Vector2f Vector2f::operator*(float scalar) const
    {
        return Vector2f(
            x * scalar,
            y * scalar);
    }

    Vector2f &Vector2f::operator*=(float scalar)
    {
        x = x * scalar;
        y = y * scalar;
        return *this;
    }

    Vector2f Vector2f::operator/(float scalar) const
    {
        return Vector2f(
            x / scalar,
            y / scalar);
    }

    Vector2f &Vector2f::operator/=(float scalar)
    {
        x = x / scalar;
        y = y / scalar;
        return *this;
    }

    bool Vector2f::operator==(const Vector2f &other) const
    {
        return x == other.x &&
               y == other.y;
    }

    bool Vector2f::operator!=(const Vector2f &other) const
    {
        return !(*this == other);
    }

    float Vector2f::Length() const
    {
        float xf = x;
        float yf = y;
        return std::sqrt(xf * xf + yf * yf);
    }

    float Vector2f::LengthSquared() const
    {
        float xf = x;
        float yf = y;
        return xf * xf + yf * yf;
    }

    float Vector2f::Dot(const Vector2f &other) const
    {
        return x * other.x +
               y * other.y;
    }

    Vector2f Vector2f::Normalize() const
    {
        float len = Length();
        if (len == 0)
            return Vector2f(0.0f, 0.0f);
        return Vector2f(x / len, y / len);
    }

    Vector2i Vector2i::operator+(const Vector2i &other) const
    {
        return Vector2i(
            x + other.x,
            y + other.y);
    }

    Vector2i &Vector2i::operator+=(const Vector2i &other)
    {
        x = x + other.x;
        y = y + other.y;
        return *this;
    }

    Vector2i Vector2i::operator-(const Vector2i &other) const
    {
        return Vector2i(
            x - other.x,
            y - other.y);
    }

    Vector2i &Vector2i::operator-=(const Vector2i &other)
    {
        x = x - other.x;
        y = y - other.y;
        return *this;
    }

    Vector2i Vector2i::operator*(int scalar) const
    {
        return Vector2i(
            x * scalar,
            y * scalar);
    }

    Vector2i &Vector2i::operator*=(int scalar)
    {
        x = x * scalar;
        y = y * scalar;
        return *this;
    }

    Vector2i Vector2i::operator/(int scalar) const
    {
        return Vector2i(
            x / scalar,
            y / scalar);
    }

    Vector2i &Vector2i::operator/=(int scalar)
    {
        x = x / scalar;
        y = y / scalar;
        return *this;
    }

    bool Vector2i::operator==(const Vector2i &other) const
    {
        return x == other.x &&
               y == other.y;
    }

    bool Vector2i::operator!=(const Vector2i &other) const
    {
        return !(*this == other);
    }

    int Vector2i::LengthSquared() const
    {
        int xf = x;
        int yf = y;
        return xf * xf + yf * yf;
    }

    int Vector2i::Dot(const Vector2i &other) const
    {
        return x * other.x +
               y * other.y;
    }

    bool AABB(RectangleF a, RectangleF b)
    {
        return (
            a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
    }
};