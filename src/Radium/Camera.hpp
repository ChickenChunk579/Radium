#pragma once

#include <Radium/Math.hpp>
#include <Radium/Nodes/ClassDB.hpp>

namespace Radium {

    /**
     * @brief Simple 2D camera with offset functionality.
     * 
     * The camera provides a way to transform world coordinates to screen coordinates
     * by applying an offset. This allows for scrolling and panning effects.
     */
    class Camera : public Nodes::Object {
    public:
        /**
         * @brief Constructs a new Camera with default settings.
         */
        Camera();

        /**
         * @brief Constructs a new Camera with specified offset.
         * @param offset The initial camera offset.
         */
        Camera(const Vector2f& offset);

        /**
         * @brief Registers the Camera class with the ClassDB system.
         * 
         * Used for reflection, serialization, or scripting.
         */
        static void Register();

        /**
         * @brief Gets the camera's viewport rectangle in world coordinates.
         * @param screenWidth The width of the screen/viewport.
         * @param screenHeight The height of the screen/viewport.
         * @return A RectangleF representing the camera's view in world space.
         */
        RectangleF GetViewport(float screenWidth, float screenHeight) const;

        Vector2f offset; ///< The camera's offset from the origin
    private:
        
    };

} // namespace Radium
