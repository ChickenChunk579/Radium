#pragma once

#include <Radium/Nodes/ClassDB.hpp>
#include <functional>

namespace Radium {

    /**
     * @brief Represents a 2D vector with floating-point precision.
     */
    class Vector2f : public Nodes::Object {
    public:
        float x; ///< X component of the vector
        float y; ///< Y component of the vector

        /**
         * @brief Registers this class with the ClassDB.
         */
        static void Register();

        /**
         * @brief Constructs a Vector2f with specified components.
         * @param x X component.
         * @param y Y component.
         */
        Vector2f(float x, float y);

        /**
         * @brief Default constructor. Initializes both components to 0.
         */
        Vector2f();

        float GetX();
        void SetX(float val);
    
        float GetY();
        void SetY(float val);

        // Vector arithmetic operations

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

        /**
         * @brief Calculates the length (magnitude) of the vector.
         */
        float Length() const;

        /**
         * @brief Calculates the squared length of the vector.
         * More efficient than Length() if you just need comparison.
         */
        float LengthSquared() const;

        /**
         * @brief Calculates the dot product with another vector.
         * @param other The other vector.
         */
        float Dot(const Vector2f& other) const;

        /**
         * @brief Returns a normalized (unit length) copy of this vector.
         */
        Vector2f Normalize() const;
    };

    /**
     * @brief Represents a 2D vector with integer precision.
     */
    class Vector2i : public Nodes::Object {
    public:
        int x; ///< X component
        int y; ///< Y component

        /**
         * @brief Registers this class with the ClassDB.
         */
        static void Register();

        /**
         * @brief Constructs a Vector2i with specified components.
         * @param x X component.
         * @param y Y component.
         */
        Vector2i(int x, int y);

        /**
         * @brief Default constructor. Initializes components to 0.
         */
        Vector2i();

        int GetX();
        void SetX(int val);

        int GetY();
        void SetY(int val);

        // Vector arithmetic operations

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

        /**
         * @brief Returns the squared length of the vector.
         */
        int LengthSquared() const;

        /**
         * @brief Calculates the dot product with another vector.
         * @param other The other vector.
         */
        int Dot(const Vector2i& other) const;
    };

    /**
     * @brief Represents an axis-aligned rectangle with floating-point precision.
     */
    class RectangleF : public Nodes::Object {
    public:
        float x; ///< X position (top-left)
        float y; ///< Y position (top-left)
        float w; ///< Width of the rectangle
        float h; ///< Height of the rectangle

        /**
         * @brief Registers this class with the ClassDB.
         */
        static void Register();

        /**
         * @brief Constructs a RectangleF with specified values.
         * @param x X coordinate of the top-left corner.
         * @param y Y coordinate of the top-left corner.
         * @param w Width of the rectangle.
         * @param h Height of the rectangle.
         */
        RectangleF(float x, float y, float w, float h);

        /**
         * @brief Default constructor. Initializes all fields to 0.
         */
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

    /**
     * @brief Checks for Axis-Aligned Bounding Box (AABB) collision between two rectangles.
     * 
     * @param rect1 First rectangle.
     * @param rect2 Second rectangle.
     * @return true if rectangles intersect, false otherwise.
     */
    bool AABB(RectangleF rect1, RectangleF rect2);    
}

namespace std {
    template <>
    struct hash<Radium::Vector2i> {
        std::size_t operator()(const Radium::Vector2i& v) const noexcept {
            std::size_t h1 = std::hash<int>{}(v.x);
            std::size_t h2 = std::hash<int>{}(v.y);
            return h1 ^ (h2 << 1); // Combine hashes
        }
    };
}
