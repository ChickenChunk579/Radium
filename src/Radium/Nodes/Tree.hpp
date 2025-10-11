#pragma once

#include <vector>
#include <string>
#include <Radium/Nodes/Tree.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/json.hpp>

using json = nlohmann::json;

namespace Radium::Nodes {
    /**
     * @brief Holds the nodes in a scene
     * 
     * Holds, manages and updates nodes for a scene.
     */
    class SceneTree : public Object {
    public:
        /**
         * @brief Constructs a scene with a name
         * 
         * @param name A name to give the scene
         */
        SceneTree(std::string name);
        SceneTree();

        /**
         * @brief Register the node with ClassDB
         */
        static void Register();

        /**
         * The scene's name
         */
        std::string name;
        
        /**
         * A list of nodes in the scene
         */
        std::vector<Node*> nodes;

        /**
         * @brief Called on program load
         */
        void OnLoad();

        /**
         * @brief Called on every tick
         * 
         * @param dt Delta time
         */
        void OnTick(float dt);

        /**
         * @brief Called on every render
         */
        void OnRender();

        /**
         * @brief Called on every imgui render
         */
        void OnImgui();

        /**
         * @brief Update global positions based on there parents
         */
        void UpdateAllGlobalPositions();

        /**
         * @brief Serialize the scene to a file
         * 
         * Serialize the scene to a .json file. Includes nodes, scripts and children and the scene name etc
         * 
         * @param path Path to the file
         */
        void Serialize(std::string path);
        /**
         * @brief Deerialize the scene from a file
         * 
         * Deserialize the scene from a .json file. Includes nodes, scripts and children and the scene name etc
         * @param path Path to the scene file
         * @param stubScripts Whether or not to actually load scripts.
         * @param external Whether the path is in assetfs or not. 
         */
        void Deserialize(std::string path, bool stubScripts = false, bool external = false);

        /**
         * @brief Get a node from a path
         * 
         * Get a node from a path such as:
         * - /Bird
         * - /PipeGroup/TopPipe
         */
        Node* GetNodeByPath(std::string path);
    };

    /**
     * @brief Serialize a node to JSON
     * 
     * Helper to serialize a node* to json.
     * 
     * @param node Node* to serialize
     * 
     * @return json JSON output.
     */
    json SerializeNode(Node *node);

    /**
     * @brief Deserialize a node to JSON
     * 
     * Helper to deserialize a node* to json.
     * 
     * @param nodeJson JSON to deserialize the node from
     * @param tree SceneTree to attach the node to
     * @param parent Parent for the node
     * @param stubScripts Stub scripts, used for the editor
     * 
     * @return Node* The deserialized node.
     */
    Node *DeserializeNode(const json &nodeJson, SceneTree* tree, Node *parent = nullptr, bool stubScripts = false);
}