#pragma once
#include <Rune/Rune.hpp>

namespace Radium {
    constexpr float baseWidth = 640.0f;
    constexpr float baseHeight = 480.0f;

    inline float GetPixelScale() {
        
        float windowWidth = (Rune::windowWidth > 0) ? Rune::windowWidth : baseWidth;
        float windowHeight = (Rune::windowHeight > 0) ? Rune::windowHeight : baseHeight;

        float scaleX = windowWidth / baseWidth;
        float scaleY = windowHeight / baseHeight;

        // Use uniform scale to maintain aspect ratio
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        return scale;
    }
}