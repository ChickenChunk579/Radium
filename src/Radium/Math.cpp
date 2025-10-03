#include "Math.hpp"

namespace Radium {

    void Vector2f::Register() {
        // Possibly register with metapp if needed
    }

    Vector2f Vector2f::operator+(const Vector2f& other) const {
        return Vector2f(
            x.get<float>() + other.x.get<float>(),
            y.get<float>() + other.y.get<float>()
        );
    }

    Vector2f& Vector2f::operator+=(const Vector2f& other) {
        x = x.get<float>() + other.x.get<float>();
        y = y.get<float>() + other.y.get<float>();
        return *this;
    }

    Vector2f Vector2f::operator-(const Vector2f& other) const {
        return Vector2f(
            x.get<float>() - other.x.get<float>(),
            y.get<float>() - other.y.get<float>()
        );
    }

    Vector2f& Vector2f::operator-=(const Vector2f& other) {
        x = x.get<float>() - other.x.get<float>();
        y = y.get<float>() - other.y.get<float>();
        return *this;
    }

    Vector2f Vector2f::operator*(float scalar) const {
        return Vector2f(
            x.get<float>() * scalar,
            y.get<float>() * scalar
        );
    }

    Vector2f& Vector2f::operator*=(float scalar) {
        x = x.get<float>() * scalar;
        y = y.get<float>() * scalar;
        return *this;
    }

    Vector2f Vector2f::operator/(float scalar) const {
        return Vector2f(
            x.get<float>() / scalar,
            y.get<float>() / scalar
        );
    }

    Vector2f& Vector2f::operator/=(float scalar) {
        x = x.get<float>() / scalar;
        y = y.get<float>() / scalar;
        return *this;
    }

    bool Vector2f::operator==(const Vector2f& other) const {
        return x.get<float>() == other.x.get<float>() &&
               y.get<float>() == other.y.get<float>();
    }

    bool Vector2f::operator!=(const Vector2f& other) const {
        return !(*this == other);
    }

    float Vector2f::Length() const {
        float xf = x.get<float>();
        float yf = y.get<float>();
        return std::sqrt(xf * xf + yf * yf);
    }

    float Vector2f::LengthSquared() const {
        float xf = x.get<float>();
        float yf = y.get<float>();
        return xf * xf + yf * yf;
    }

    float Vector2f::Dot(const Vector2f& other) const {
        return x.get<float>() * other.x.get<float>() +
               y.get<float>() * other.y.get<float>();
    }

    Vector2f Vector2f::Normalize() const {
        float len = Length();
        if (len == 0) return Vector2f(0.0f, 0.0f);
        return Vector2f(x.get<float>() / len, y.get<float>() / len);
    }

    // ---------------- Vector2i ----------------

    void Vector2i::Register() {
        // Register logic here if needed
    }

    Vector2i::Vector2i(metapp::Variant x, metapp::Variant y) : x(x), y(y) {}

    Vector2i Vector2i::operator+(const Vector2i& other) const {
        return Vector2i(
            x.get<int>() + other.x.get<int>(),
            y.get<int>() + other.y.get<int>()
        );
    }

    Vector2i& Vector2i::operator+=(const Vector2i& other) {
        x = x.get<int>() + other.x.get<int>();
        y = y.get<int>() + other.y.get<int>();
        return *this;
    }

    Vector2i Vector2i::operator-(const Vector2i& other) const {
        return Vector2i(
            x.get<int>() - other.x.get<int>(),
            y.get<int>() - other.y.get<int>()
        );
    }

    Vector2i& Vector2i::operator-=(const Vector2i& other) {
        x = x.get<int>() - other.x.get<int>();
        y = y.get<int>() - other.y.get<int>();
        return *this;
    }

    Vector2i Vector2i::operator*(int scalar) const {
        return Vector2i(
            x.get<int>() * scalar,
            y.get<int>() * scalar
        );
    }

    Vector2i& Vector2i::operator*=(int scalar) {
        x = x.get<int>() * scalar;
        y = y.get<int>() * scalar;
        return *this;
    }

    Vector2i Vector2i::operator/(int scalar) const {
        return Vector2i(
            x.get<int>() / scalar,
            y.get<int>() / scalar
        );
    }

    Vector2i& Vector2i::operator/=(int scalar) {
        x = x.get<int>() / scalar;
        y = y.get<int>() / scalar;
        return *this;
    }

    bool Vector2i::operator==(const Vector2i& other) const {
        return x.get<int>() == other.x.get<int>() &&
               y.get<int>() == other.y.get<int>();
    }

    bool Vector2i::operator!=(const Vector2i& other) const {
        return !(*this == other);
    }

    int Vector2i::LengthSquared() const {
        int xf = x.get<int>();
        int yf = y.get<int>();
        return xf * xf + yf * yf;
    }

    int Vector2i::Dot(const Vector2i& other) const {
        return x.get<int>() * other.x.get<int>() +
               y.get<int>() * other.y.get<int>();
    }

    // ---------------- RectangleF ----------------

    void RectangleF::Register() {
        
    }

    RectangleF::RectangleF(metapp::Variant x, metapp::Variant y, metapp::Variant w, metapp::Variant h)
        : x(x), y(y), w(w), h(h) {}

}
