#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
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
    }

    void Sprite2D::OnRender() {
        Node2D::OnRender();

        Rune::SpriteBatch* batch = nullptr;
        batch = Radium::SpriteBatchRegistry::Get(batchTag);
        if (!batch->started) {
            batch->Begin();
        }
        if (sourceRect.w > 0 && sourceRect.h > 0) {
            batch->DrawImageRect(
                position.x, position.y,
                (uint)sourceRect.w, (uint)sourceRect.h,
                r, g, b,
                (uint)sourceRect.x, (uint)sourceRect.y,
                (uint)sourceRect.w, (uint)sourceRect.h,
                textureWidth, textureHeight,
                rotation, z, flags
            );
        }
    }
}