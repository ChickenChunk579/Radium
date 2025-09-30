#pragma once

#include <vector>
#include <string>
#include <Radium/Nodes/Tree.hpp>
#include <Radium/Nodes/Node.hpp>
namespace Radium::Nodes {
    class SceneTree {
    public:
        SceneTree(std::string name);

        std::string name;
        
        std::vector<Node*> nodes;

        void OnLoad();

        void OnTick(float dt);

        void OnRender();

        void OnImgui();
    };
}