#pragma once

#include <Radium/Nodes/Node.hpp>

namespace Radium::Nodes {
    class Script {
    public:
        void* me = nullptr;

        virtual void OnLoad() {}
        virtual void OnTick(float dt) {}
        virtual void OnRender() {}
        virtual void OnImgui() {}

        virtual ~Script() = default;
    };
}