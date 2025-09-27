#pragma once

#include <Radium/Math.hpp>
#include <iostream>

namespace Radium::Components {
    struct FullDrawSprite {
        std::string batchTag;
        float r, g, b;
        float rotation;
    };

    struct TileDrawSprite {
        std::string batchTag;
        int tileX;
        int tileY;
        int tilesPerRow;
        int tilesPerColumn;
        float r, g, b;
        float rotation;
    };
}