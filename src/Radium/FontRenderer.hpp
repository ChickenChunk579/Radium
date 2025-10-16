#pragma once

#include <stdint.h>
#include <string>
#include <Radium/Math.hpp>
#include <Radium/stb_truetype.h>

namespace Radium {
    class TTFFont {
    private:
        stbtt_fontinfo font;
        std::vector<unsigned char> fontBuffer;
    public:
        float size = 32;

        TTFFont(std::string path, float size);

        void AddGlyph(char c, Vector2f position);
        void AddString(std::string s, Vector2f position);
    };
}