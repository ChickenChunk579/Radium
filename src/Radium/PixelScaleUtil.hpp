#pragma once
#include <Rune/Rune.hpp>
#include <Rune/Viewport.hpp>

namespace Radium {
    /// @brief The base width that the game will be scaled down to
    constexpr float baseWidth = 640.0f;
    
    /// @brief The base height that the game will be scaled down to
    constexpr float baseHeight = 480.0f;

    /// @brief Get a scale factor based on baseWidth and height 
    inline float GetPixelScale() {
        
        float windowWidth = (Rune::windowWidth > 0) ? Rune::windowWidth : baseWidth;
        float windowHeight = (Rune::windowHeight > 0) ? Rune::windowHeight : baseHeight;
        if (Rune::currentViewport != nullptr) {
            windowWidth = Rune::currentViewport->width;
            windowHeight = Rune::currentViewport->height;
        }

        float scaleX = windowWidth / baseWidth;
        float scaleY = windowHeight / baseHeight;

        // Use uniform scale to maintain aspect ratio
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        return scale;
    }
}