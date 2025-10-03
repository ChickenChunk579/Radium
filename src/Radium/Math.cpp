#include <Radium/Math.hpp>

namespace Radium {
    Vector2i::Vector2i(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Vector2f::Vector2f(float x, float y) {
        this->x = x;
        this->y = y;
    }

    RectangleF::RectangleF(float x, float y, float w, float h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
};