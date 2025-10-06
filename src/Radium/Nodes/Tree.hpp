#pragma once

#include <vector>
#include <string>
#include <Radium/Nodes/Tree.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/json.hpp>

using json = nlohmann::json;

namespace Radium::Nodes {
    class SceneTree : public Object {
    public:
        SceneTree(std::string name);
        SceneTree();

        static void Register();

        std::string name;
        
        std::vector<Node*> nodes;

        void OnLoad();

        void OnTick(float dt);

        void OnRender();

        void OnImgui();

        void UpdateAllGlobalPositions();

        void Serialize(std::string path);
        void Deserialize(std::string path, bool stubScripts = false, bool external = false);

        Node* GetNodeByPath(std::string path);
    };
}