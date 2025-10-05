#pragma once
#include <Radium/Nodes/Script.hpp>
#include <Radium/Nodes/Tree.hpp>
#include <string>
#undef Bool
#include <Radium/pch.hpp>

namespace Radium::Nodes {
    class ChaiScript: public Script {
    public:
        ChaiScript(std::string path, SceneTree* tree, bool realLoad = true);

        void OnLoad() override;
        void OnTick(float dt) override;
        void OnRender() override;
        void OnImgui() override;

        std::string path;
        Node* GetMe();
        SceneTree* GetSceneTree();
        void SetMe(Node node);

        template <typename T>
        void SetGlobal(T me, std::string name) {
            chai.add_global(chaiscript::var(me), name);
        }

    private:
        SceneTree* sceneTree;
        bool stubbed = false;
        chaiscript::ChaiScript chai;
    };
}
