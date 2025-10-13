#pragma once
#include <vector>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Math.hpp>

namespace Radium::Nodes {

    /**
     * @class Node2D
     * @brief A 2D node representing an element in a scene graph with position and size.
     * 
     * Extends the base Node class by adding 2D spatial properties such as position,
     * global position (world space), and size. Supports update and rendering callbacks.
     */
    class Node2D : public Node {
    public:
        /**
         * @brief Construct a new Node2D object with default values.
         * 
         * Initializes position, globalPosition, and size to zero vectors.
         */
        Node2D();

        /**
         * @brief Registers the Node2D class with the ClassDB system.
         * 
         * Typically used for reflection, scripting exposure, or serialization.
         */
        static void Register();

        /// Local position relative to the parent node.
        Radium::Vector2f position = {0, 0};

        /// Global position in world coordinates (computed based on parent nodes).
        Radium::Vector2f globalPosition = {0, 0};

        /// Size of the node (width and height).
        Radium::Vector2f size = {0, 0};

        // Global size in world coordinates (computed based on parent nodes)
        Radium::Vector2f globalSize = {0, 0};

        /// Rotation of the node in degrees
        float rotation = 0;

        /// Global rotation of the node in degrees (computed based on parent nodes)
        float globalRotation = 0;

        /**
         * @brief Called when the node is loaded.
         * 
         * Override from base Node class.
         */
        void OnLoad() override;

        /**
         * @brief Called every tick to update the node state.
         * 
         * @param dt Delta time in seconds since last update.
         */
        void OnTick(float dt) override;

        /**
         * @brief Called every render frame for rendering the node.
         */
        void OnRender() override;

        /**
         * @brief Called during ImGui rendering for UI debug or editing.
         */
        void OnImgui() override;

        /**
         * @brief Get the local position of this node.
         * 
         * @return Radium::Vector2f The local position vector.
         */
        Radium::Vector2f GetPosition();

        /**
         * @brief Set the local position of this node.
         * 
         * @param val The new position vector to set.
         */
        void SetPosition(Radium::Vector2f val);

        /**
         * @brief Get the global position of this node in world space.
         * 
         * @return Radium::Vector2f The global position vector.
         */
        Radium::Vector2f GetGlobalPosition();

        /**
         * @brief Set the global position of this node.
         * 
         * Note: This may internally update local position to maintain hierarchy consistency.
         * 
         * @param val The new global position vector to set.
         */
        void SetGlobalPosition(Radium::Vector2f val);

        /**
         * @brief Get the size of this node.
         * 
         * @return Radium::Vector2f The size vector (width and height).
         */
        Radium::Vector2f GetSize();

        /**
         * @brief Set the size of this node.
         * 
         * @param val The new size vector to set.
         */
        void SetSize(Radium::Vector2f val);

        /**
         * @brief Update the global position based on the current local position and parent's global position.
         */
        void UpdateGlobals();

    private:
        /**
         * @brief Internal recursive method to update global position.
         * 
         * Calculates the global position based on the parent's global position.
         * 
         * @param parentGlobalPos The global position of the parent node (default is origin).
         */
        void UpdateGlobalPositionInternal(const Radium::Vector2f& parentGlobalPos = {0,0});
        void UpdateGlobalSizeInternal(const Radium::Vector2f& parentGlobalSize = {0,0});
        void UpdateGlobalRotationInternal(const float& parentGlobalRotation = 0);
    };

}
