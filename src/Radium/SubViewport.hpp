#pragma once
#include <Rune/Viewport.hpp>
#include <Radium/Nodes/Tree.hpp>

namespace Radium {

    /**
     * @brief Represents a sub-viewport within the main application window.
     * 
     * A SubViewport has its own `SceneTree` and `Viewport`, allowing for
     * rendering an isolated scene (useful for editors, previews, or multi-cam setups).
     */
    class SubViewport {
    public:
        /**
         * @brief Constructs a SubViewport with a specific size.
         * 
         * @param width Width of the viewport in pixels.
         * @param height Height of the viewport in pixels.
         */
        SubViewport(int width, int height);

        /**
         * @brief Destructor. Cleans up any allocated resources.
         */
        ~SubViewport();

        /**
         * @brief Pointer to the underlying rendering viewport.
         * 
         * This is used internally to render to a specific area of the screen or texture.
         */
        Rune::Viewport* viewport;

        /**
         * @brief The scene tree associated with this viewport.
         * 
         * Each SubViewport has its own tree of nodes to simulate an independent world.
         */
        Nodes::SceneTree tree;

        /**
         * @brief Called when the SubViewport is initialized.
         * 
         * Use this to load resources, configure the scene, etc.
         */
        void OnLoad();

        /**
         * @brief Called every frame to update the scene.
         * 
         * @param dt Delta time (in seconds) since the last frame.
         */
        void OnTick(float dt);

        /**
         * @brief Renders the contents of the SubViewport.
         * 
         * Should be called each frame after `OnTick`.
         */
        void OnRender();

        /**
         * @brief Draws ImGui widgets or overlays specific to this SubViewport.
         * 
         * Called after `OnRender`. Typically used for debugging or editor tools.
         */
        void OnImgui();
    };

}
