#pragma once

namespace Radium::Nodes {
    class Node;

    class Script {
    public:
        Node* me;
        virtual void OnLoad() {}
        virtual void OnTick(float dt) {}
        virtual void OnRender() {}
        virtual void OnImgui() {}

        virtual ~Script() = default;
    };
}