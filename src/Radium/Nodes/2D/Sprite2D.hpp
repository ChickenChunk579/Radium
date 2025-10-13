#pragma once
#include <vector>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Math.hpp>

namespace Radium::Nodes {

    /**
     * @enum CoordinateOrigin
     * @brief Defines the origin point for positioning sprites.
     */
    enum class CoordinateOrigin {
        TopLeft, ///< Origin is at the top-left corner of the sprite.
        Center   ///< Origin is at the center of the sprite.
    };

    /**
     * @class Sprite2D
     * @brief Represents a 2D sprite node with color, texture, and transform properties.
     * 
     * Inherits from Node2D to provide spatial positioning and sizing in 2D space,
     * while adding rendering-specific properties like color, texture region, rotation, and layering.
     */
    class Sprite2D : public Node2D {
    public:
        /**
         * @brief Construct a new Sprite2D object with default values.
         * 
         * Initializes color to black, origin to TopLeft, and other properties to zero or empty.
         */
        Sprite2D();

        /**
         * @brief Registers the Sprite2D class with the ClassDB system.
         * 
         * Used for reflection, serialization, or scripting.
         */
        static void Register();

        /// Red component of the sprite's color (0 to 1).
        float r = 0;

        /// Green component of the sprite's color (0 to 1).
        float g = 0;

        /// Blue component of the sprite's color (0 to 1).
        float b = 0;

        /// Origin point used for positioning and rotation.
        CoordinateOrigin origin;

        /// Rectangle defining the source region of the texture to display (x, y, width, height).
        Radium::RectangleF sourceRect{0, 0, 0, 0};

        /// Width of the texture in pixels.
        uint32_t textureWidth = 0;

        /// Height of the texture in pixels.
        uint32_t textureHeight = 0;

        /// Z-order for layering sprites (higher values render on top).
        float z = 0;

        /// Flags controlling rendering options (bitmask).
        uint32_t flags = 0;

        /// Tag used for batching sprites to optimize rendering.
        std::string batchTag;

        /**
         * @brief Called every render frame to draw the sprite.
         * 
         * Override from Node2D.
         */
        void OnRender() override;
    };

}
