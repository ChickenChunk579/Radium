#pragma once

#include <Radium/Nodes/ClassDB.hpp>

namespace Radium {
    class Vector2f : public Nodes::Object {
    public:
        float x;
        float y;

        static void Register();

        Vector2f(float x, float y);
        Vector2f();

        float GetX();
        void SetX(float val);
    
        float GetY();
        void SetY(float val);


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

    class Vector2i : public Nodes::Object {
    public:
        int x;
        int y;

        static void Register();

        Vector2i(int x, int y);
        Vector2i();

        int GetX();
        void SetX(int val);

        int GetY();
        void SetY(int val);


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

    class RectangleF : public Nodes::Object {
    public:
        float x;
        float y;
        float w;
        float h;

        static void Register();

        RectangleF(float x, float y, float w, float h);
        RectangleF();

        float GetX();
        void SetX(float val);

        float GetY();
        void SetY(float val);


        float GetW();
        void SetW(float val);

        float GetH();
        void SetH(float val);
    };
    bool AABB(RectangleF rect1, RectangleF rect2);
};