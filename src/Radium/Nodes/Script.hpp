#pragma once

namespace Radium::Nodes {
    class Script {
    public:
        virtual void OnLoad() {}
        virtual void OnTick(float dt) {}
        virtual void OnRender() {}
        virtual void OnImgui() {}

        virtual ~Script() = default;
    };
}