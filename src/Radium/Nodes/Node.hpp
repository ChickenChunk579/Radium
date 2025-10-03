#pragma once
#include <vector>
#include <string>

namespace Radium::Nodes {
    class Script;

    class Node {
    public:
        Node();

        static void Register();

        Node* parent;
        std::vector<Node*> children;
        Script* script = nullptr;
        std::string name = "Node";

        virtual void OnLoad();
        virtual void OnTick(float dt);
        virtual void OnRender();
        virtual void OnImgui();
    };
}