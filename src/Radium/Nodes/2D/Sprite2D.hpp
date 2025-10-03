#pragma once
#include <vector>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Math.hpp>
#include <cstdint>

namespace Radium::Nodes {
    enum class CoordinateOrigin {
        TopLeft,
        Center
    };

    class Sprite2D : public Node2D {
    public:
        Sprite2D();

        static void Register();

        float r = 0;
        float g = 0;
        float b = 0;
        CoordinateOrigin origin;
        Radium::RectangleF sourceRect{0, 0, 0, 0};
        uint32_t textureWidth = 0;
        uint32_t textureHeight = 0;
        float rotation = 0;
        float z = 0;
        uint32_t flags = 0;
        std::string batchTag;

        void OnRender() override;
    };
}