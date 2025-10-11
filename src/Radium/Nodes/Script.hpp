#pragma once

namespace Radium::Nodes {
    class Node;

    /**
     * @brief A base class for scripts to inherit
     * 
     * Scripts can inherit this class to gain access to callbacks
     * to run logic, as well as automatically running along side a node.
     */
    class Script {
    public:
        /**
         * @brief The node the script is attached to
         * 
         * A pointer to the node the script is attached to.
         */
        Node* me;
        /**
         * @brief Called on program load
         */
        virtual void OnLoad() {}
        /**
         * @brief Called on every tick
         * @param dt Delta time
         */
        virtual void OnTick(float dt) {}
        /**
         * @brief Called on every render
         */
        virtual void OnRender() {}
        /**
         * @brief Called on every imgui render
         */
        virtual void OnImgui() {}

        virtual ~Script() = default;
    };
}