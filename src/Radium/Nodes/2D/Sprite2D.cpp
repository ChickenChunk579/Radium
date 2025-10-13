#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/PixelScaleUtil.hpp>
#include <Radium/Application.hpp>
#include <Rune/SpriteBatch.hpp>
#include <spdlog/spdlog.h>

namespace Radium::Nodes {

Sprite2D::Sprite2D() {
}

void Sprite2D::Register() {
    CLASSDB_REGISTER_SUBCLASS(Sprite2D, Node2D);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, float, r);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, float, g);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, float, b);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, Radium::RectangleF, sourceRect);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, uint32_t, textureWidth);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, uint32_t, textureHeight);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, float, z);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, uint32_t, flags);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, std::string, batchTag);
    CLASSDB_DECLARE_PROPERTY(Sprite2D, CoordinateOrigin, origin);
}

void Sprite2D::OnRender() {
    Node2D::OnRender();
    
    Rune::SpriteBatch* batch = nullptr;
    batch = Radium::SpriteBatchRegistry::Get(batchTag);
    
    if (!batch) {
        spdlog::error("Sprite2D: No sprite batch with tag '{}'", batchTag);
        return;
    }
    
    if (!batch->started) {
        batch->Begin();
    }
    
    if (sourceRect.w > 0 && sourceRect.h > 0) {
        // Apply camera offset to world position
        Radium::Vector2f screenPos = globalPosition;
        if (Radium::currentApplication) {
            screenPos = screenPos - Radium::currentApplication->GetCamera()->offset;
        }
        
        float drawX = (screenPos.x * Radium::GetPixelScale());
        float drawY = (-screenPos.y * Radium::GetPixelScale());
        
        if (origin == CoordinateOrigin::Center) {
            drawX += Rune::windowWidth / 2.0f;
            drawY += Rune::windowHeight / 2.0f;
        }
        
        // Calculate display size - if size is set, use it, otherwise use sourceRect dimensions
        uint32_t displayW = (size.x > 0) ? (uint32_t)(size.x * Radium::GetPixelScale()) : (uint32_t)(sourceRect.w * Radium::GetPixelScale());
        uint32_t displayH = (size.y > 0) ? (uint32_t)(size.y * Radium::GetPixelScale()) : (uint32_t)(sourceRect.h * Radium::GetPixelScale());
        
        batch->DrawImageRect(
            drawX, drawY,
            displayW, displayH,  // Display size (can be scaled)
            r, g, b,
            (uint32_t)sourceRect.x, (uint32_t)sourceRect.y,  // Source position in texture
            (uint32_t)sourceRect.w, (uint32_t)sourceRect.h,  // Source size in texture (actual UV dimensions)
            textureWidth, textureHeight,
            globalRotation, z, flags
        );
    }
}

}