#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/PixelScaleUtil.hpp>
#include <Rune/SpriteBatch.hpp>
#include <spdlog/spdlog.h>

namespace Radium::Nodes {
    Sprite2D::Sprite2D() {
        
    }

    void Sprite2D::Register() {
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
        if (sourceRect.w.get<uint>() > 0 && sourceRect.h.get<uint>() > 0) {
            float drawX = (globalPosition.x.get<float>() * Radium::GetPixelScale());
            float drawY = (-globalPosition.y.get<float>() * Radium::GetPixelScale());

            if (origin == CoordinateOrigin::Center) {
                drawX += Rune::windowWidth / 2.0f;
                drawY += Rune::windowHeight / 2.0f;
            }

            batch->DrawImageRect(
                drawX, drawY,
                sourceRect.w.get<uint>() * Radium::GetPixelScale(), sourceRect.h.get<uint>() * Radium::GetPixelScale(),
                r, g, b,
                sourceRect.x.get<uint>(), sourceRect.y.get<uint>(),
                sourceRect.w.get<uint>(), sourceRect.h.get<uint>(),
                textureWidth, textureHeight,
                rotation, z, flags
            );
        }
    }
}