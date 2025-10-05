#pragma once
#include <Rune/Viewport.hpp>
#include <Radium/Nodes/Tree.hpp>

namespace Radium {
    class SubViewport {
    public:
        SubViewport(int width, int height);

        Rune::Viewport* viewport;
        Nodes::SceneTree tree;

        void OnLoad();
        void OnTick(float dt);
        void OnRender();
        void OnImgui();
    };
}