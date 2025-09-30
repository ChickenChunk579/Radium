#pragma once
#include <vector>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/Nodes/Script.hpp>

namespace Radium::Nodes {
    class Node : public Object {
    public:
        Node();

        static void Register();

        Node* parent;
        std::vector<Node> children;
        Script* script = nullptr;

        virtual void OnLoad();
        virtual void OnTick(float dt);
        virtual void OnRender();
        virtual void OnImgui();
    };
}