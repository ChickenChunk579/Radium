#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/PixelScaleUtil.hpp>
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
        CLASSDB_DECLARE_PROPERTY(Sprite2D, uint, textureWidth);
        CLASSDB_DECLARE_PROPERTY(Sprite2D, uint, textureHeight);
        CLASSDB_DECLARE_PROPERTY(Sprite2D, float, rotation);
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
            float drawX = (globalPosition.x * Radium::GetPixelScale());
            float drawY = (-globalPosition.y * Radium::GetPixelScale());

            if (origin == CoordinateOrigin::Center) {
                drawX += Rune::windowWidth / 2.0f;
                drawY += Rune::windowHeight / 2.0f;
            }

            batch->DrawImageRect(
                drawX, drawY,
                (uint)sourceRect.w * Radium::GetPixelScale(), (uint)sourceRect.h * Radium::GetPixelScale(),
                r, g, b,
                (uint)sourceRect.x, (uint)sourceRect.y,
                (uint)size.x, (uint)size.y,
                textureWidth, textureHeight,
                rotation, z, flags
            );
        }
    }
}