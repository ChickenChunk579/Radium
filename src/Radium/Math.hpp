#pragma once

namespace Radium {
    class Vector2f {
    public:
        float x;
        float y;

        Vector2f(float x, float y);
    };

    class Vector2i {
    public:
        int x;
        int y;

        Vector2i(int x, int y);
    };

    class RectangleF {
    public:
        float x;
        float y;
        float w;
        float h;

        RectangleF(float x, float y, float w, float h);
    };
};