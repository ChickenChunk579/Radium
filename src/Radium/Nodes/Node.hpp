#pragma once
#include <vector>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/Nodes/Script.hpp>

namespace Radium::Nodes {
    /**
     * @brief Base of all nodes
     * 
     * Manages children and scripts, behavior present on every node
     * Every node should inherit Node, or a derivative of it.
     */
    class Node : public Object {
    public:
        /**
         * @brief Construct a node
         * 
         * Construct a node
         */
        Node();

        /**
         * @brief Register to ClassDB
         * 
         * Registers the Node class into the reflection system.
         */
        static void Register();

        /**
         * @brief The nodes parent
         * 
         * A reference to the nodes parent.
         */
        Node* parent;
        /**
         * @brief The nodes children
         * 
         * A list of the nodes children, which will be updated with the root
         * node.
         */
        std::vector<Node*> children;
        /**
         * @brief A refernece to a script to be used
         * 
         * A pointer to a script to be used on the node. Will be called automatically along side the other functions.
         * If it is nullptr, it will be skipped.
         */
        Script* script = nullptr;
        /**
         * @brief The name of the node
         * 
         * The name of the node, to be used for organization and editing.
         */
        std::string name;

        /**
         * @brief Called on program load
         */
        virtual void OnLoad();
        /**
         * @brief Called on every tick
         * @param dt Delta time
         */
        virtual void OnTick(float dt);
        /**
         * @brief Called on every render
         */
        virtual void OnRender();
        /**
         * @brief Called on every imgui render
         */
        virtual void OnImgui();

        /**
         * @brief Get a child node by its name
         */
        Node* GetChildByName(std::string name);
    };
}