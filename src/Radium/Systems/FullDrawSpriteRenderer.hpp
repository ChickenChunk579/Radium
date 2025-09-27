#pragma once

#include <entt/entt.hpp>
#include <Radium/Math.hpp>
#include <Radium/Components/Position.hpp>
#include <Radium/Components/Size.hpp>
#include <Radium/Components/Sprite.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Rune/Rune.hpp>

namespace Radium::Systems {
    inline void FullDrawSpriteRender(entt::registry& registry) {
        auto view = registry.view<
            Radium::Components::Position,
            Radium::Components::Size,
            Radium::Components::FullDrawSprite
        >();

        for (auto [entity, pos, size, sprite] : view.each()) {
            Rune::SpriteBatch* batch = Radium::SpriteBatchRegistry::Get(sprite.batchTag);
            if (!batch->started) {
                batch->Begin();
            }

            // Offset position so (0,0) is center of screen
            float drawX = pos.position.x + Rune::windowWidth  / 2.0f;
            float drawY = pos.position.y + Rune::windowHeight / 2.0f;

            batch->Draw(
                drawX,
                drawY,
                size.size.x,
                size.size.y,
                sprite.r,
                sprite.g,
                sprite.b,
                sprite.rotation
            );
        }
    }
}
