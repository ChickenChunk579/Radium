#include <Radium/FontRenderer.hpp>
#include <Radium/DebugRenderer.hpp>
#include <Flux/Flux.hpp>
#include <fstream>
#define STB_TRUETYPE_IMPLEMENTATION
#include <Radium/stb_truetype.h>
#include <earcut.hpp>
using Coord = float;
using N = uint32_t;


std::vector<unsigned char> LoadFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    return std::vector<unsigned char>((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
}

namespace Radium {
    TTFFont::TTFFont(std::string path, float size) {
        fontBuffer = LoadFile(path.c_str());
        this->size = size;

        if (!stbtt_InitFont(&font, fontBuffer.data(), stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0))) {
            Flux::Error("stbtt failed in InitFont");
            exit(1);
        }
    }

    void TTFFont::AddGlyph(char c, Vector2f position) {
        int glyphIndex = stbtt_FindGlyphIndex(&font, c);
        
        if (glyphIndex == 0) {
            Flux::Warn("Glyph not found for character: '{}'", c);
            return;
        }
        
        stbtt_vertex* vertices;
        int numVerts = stbtt_GetGlyphShape(&font, glyphIndex, &vertices);
        
        float scale = stbtt_ScaleForPixelHeight(&font, size);
        Vector2f cursor = {};
        
        for (int i = 0; i < numVerts; ++i) {
            stbtt_vertex v = vertices[i];
            
            // Scale and position the endpoint
            float x_px = v.x * scale + position.x;
            float y_px = v.y * scale + position.y;
            
            switch (v.type) {
                case STBTT_vmove:
                    cursor.x = x_px;
                    cursor.y = y_px;
                    break;
                    
                case STBTT_vline:
                    DebugRenderer::AddLine(cursor, {x_px, y_px}, 1, 1, 1, 2, Nodes::CoordinateOrigin::TopLeft);
                    cursor = {x_px, y_px};
                    break;
                    
                case STBTT_vcurve: {
                    float cx = v.cx * scale + position.x;
                    float cy = v.cy * scale + position.y;
                    
                    DebugRenderer::AddQuadraticBezier(
                        cursor, {cx, cy}, {x_px, y_px},
                        1, 1, 1, 2, 20,
                        Nodes::CoordinateOrigin::TopLeft
                    );
                    cursor = {x_px, y_px};
                    break;
                }
                    
                case STBTT_vcubic: {
                    float cx = v.cx * scale + position.x;
                    float cy = v.cy * scale + position.y;
                    float cx1 = v.cx1 * scale + position.x;
                    float cy1 = v.cy1 * scale + position.y;
                    
                    DebugRenderer::AddCubicBezier(
                        cursor, {cx, cy}, {cx1, cy1}, {x_px, y_px},
                        1, 1, 1, 2, 30,
                        Nodes::CoordinateOrigin::TopLeft
                    );
                    cursor = {x_px, y_px};
                    break;
                }
            }
        }
        
        stbtt_FreeShape(&font, vertices);
    }


    void TTFFont::AddString(std::string s, Vector2f position) {
        Vector2f cursor = position;
        for (auto c : s) {
            int glyphIndex = stbtt_FindGlyphIndex(&font, c);
            
            if (glyphIndex == 0) {
                Flux::Warn("Glyph not found for character: '{}'", c);
                continue;
            }
            
            float scale = stbtt_ScaleForPixelHeight(&font, size);
            
            // Get advance width and left side bearing for spacing
            int advanceWidth, leftSideBearing;
            stbtt_GetGlyphHMetrics(&font, glyphIndex, &advanceWidth, &leftSideBearing);
            

            AddGlyph(c, cursor);

            cursor.x += advanceWidth * scale;
        }
    }
}