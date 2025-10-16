#pragma once
#include <Rune/GeometryRenderer.hpp>
#include <Radium/Math.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>

namespace Radium::DebugRenderer {
    extern std::vector<float> vertices;
    extern Rune::GeometryRenderer* renderer;

    void Setup();

    void AddQuadraticBezier(Vector2f p0, Vector2f p1, Vector2f p2, float r, float g, float b, float thickness, int segments, Radium::Nodes::CoordinateOrigin origin);

    void AddCubicBezier(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, float r, float g, float b, float thickness, int segments, Radium::Nodes::CoordinateOrigin origin);

    void AddLine(Vector2f start, Vector2f end, float r, float g, float b, float thickness = 10, Radium::Nodes::CoordinateOrigin origin = Radium::Nodes::CoordinateOrigin::TopLeft);

    void AddRect(RectangleF rect, float r, float g, float b, float thickness = 10, Radium::Nodes::CoordinateOrigin origin = Radium::Nodes::CoordinateOrigin::TopLeft, float rotation = 0);

    void AddPoint(Vector2f pos, float r, float g, float b, float radius = 10, float resolution = 10, Radium::Nodes::CoordinateOrigin origin = Radium::Nodes::CoordinateOrigin::TopLeft);
    
    void AddCircleOutline(Vector2f pos, float r, float g, float b, float radius = 10, float resolution = 10, Radium::Nodes::CoordinateOrigin origin = Radium::Nodes::CoordinateOrigin::TopLeft);

    void AddGlyph(char c, Vector2f pos, float r, float g, float b, float thickness = 5);

    void AddString(std::string s, Vector2f pos, float r, float g, float b, float thickness = 5);

    void Draw();
}