#include <Radium/Math.hpp>
#include <Radium/Nodes/LuaScript.hpp>
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

    void Vector2f::Register() {
        CLASSDB_REGISTER(Vector2f);
        CLASSDB_DECLARE_PROPERTY(Vector2f, float, x);
        CLASSDB_DECLARE_PROPERTY(Vector2f, float, y);

        LUA_CONSTRUCTOR("Vector2f", [](lua_State* L) -> int {
            float x = Nodes::get_float_arg(L, 1);
            float y = Nodes::get_float_arg(L, 2);
            return Radium::Nodes::classdb_lua_wrap(L, new Vector2f(x, y));
        });

        LUA_FUNC("Radium::Vector2f::Length", [](lua_State* L) -> int {
            Vector2f* instance = (Vector2f*)lua_touserdata(L, lua_upvalueindex(1));
            lua_pushnumber(L, instance->Length());
            return 1;
        });

        LUA_FUNC("Radium::Vector2f::LengthSquared", [](lua_State* L) -> int {
            Vector2f* instance = (Vector2f*)lua_touserdata(L, lua_upvalueindex(1));
            lua_pushnumber(L, instance->LengthSquared());
            return 1;
        });

        LUA_FUNC("Radium::Vector2f::Normalize", [](lua_State* L) -> int {
            Vector2f* instance = (Vector2f*)lua_touserdata(L, lua_upvalueindex(1));
            Vector2f result = instance->Normalize();
            return Radium::Nodes::classdb_lua_wrap(L, new Vector2f(result));
        });

        LUA_FUNC("Radium::Vector2f::Dot", [](lua_State* L) -> int {
            Vector2f* instance = (Vector2f*)lua_touserdata(L, lua_upvalueindex(1));
            Vector2f* other = (Vector2f*)lua_touserdata(L, 1);
            lua_pushnumber(L, instance->Dot(*other));
            return 1;
        });

    }

    void Vector2i::Register() {
        CLASSDB_REGISTER(Vector2i);
        CLASSDB_DECLARE_PROPERTY(Vector2i, int, x);
        CLASSDB_DECLARE_PROPERTY(Vector2i, int, y);

        LUA_CONSTRUCTOR("Vector2i", [](lua_State* L) -> int {
            int x = Nodes::get_float_arg(L, 1);
            int y = Nodes::get_float_arg(L, 2);
            return Radium::Nodes::classdb_lua_wrap(L, new Vector2i(x, y));
        });

        LUA_FUNC("Radium::Vector2i::LengthSquared", [](lua_State* L) -> int {
            Vector2i* instance = (Vector2i*)lua_touserdata(L, lua_upvalueindex(1));
            lua_pushinteger(L, instance->LengthSquared());
            return 1;
        });

        LUA_FUNC("Radium::Vector2i::Dot", [](lua_State* L) -> int {
            Vector2i* instance = (Vector2i*)lua_touserdata(L, lua_upvalueindex(1));
            Vector2i* other = (Vector2i*)lua_touserdata(L, 1);
            lua_pushinteger(L, instance->Dot(*other));
            return 1;
        });

    }

    void RectangleF::Register() {
        CLASSDB_REGISTER(RectangleF);
        CLASSDB_DECLARE_PROPERTY(RectangleF, float, x);
        CLASSDB_DECLARE_PROPERTY(RectangleF, float, y);
        CLASSDB_DECLARE_PROPERTY(RectangleF, float, w);
        CLASSDB_DECLARE_PROPERTY(RectangleF, float, h);

        LUA_CONSTRUCTOR("RectangleF", [](lua_State* L) -> int {
            float x = Nodes::get_float_arg(L, 1);
            float y = Nodes::get_float_arg(L, 2);
            float w = Nodes::get_float_arg(L, 3);
            float h = Nodes::get_float_arg(L, 4);

            return Radium::Nodes::classdb_lua_wrap(L, new RectangleF(x, y, w, h));
        });
    }

    Vector2f::Vector2f() : x(0), y(0) {}
    Vector2i::Vector2i() : x(0), y(0) {}
    RectangleF::RectangleF() : x(0), y(0), w(0), h(0) {}
};