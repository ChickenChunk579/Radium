#include <Radium/Nodes/Tree.hpp>

namespace Radium::Nodes {
    SceneTree::SceneTree(std::string name) : name(name) {
        
    }

    void SceneTree::OnLoad() {
        for (auto& node : nodes) {
            node->OnLoad();
        }
    }

    void SceneTree::OnTick(float dt) {
        for (auto& node : nodes) {
            node->OnTick(dt);
        }
    }

    void SceneTree::OnRender() {
        for (auto& node : nodes) {
            node->OnRender();
        }
    }

    void SceneTree::OnImgui() {
        for (auto& node : nodes) {
            node->OnImgui();
        }
    }
}