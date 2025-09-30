#include <Radium/Nodes/Node.hpp>
#include <spdlog/spdlog.h>

namespace Radium::Nodes {
    Node::Node() : parent(nullptr) {
        
    }

    void Node::Register() {
        CLASSDB_REGISTER(Node);
        CLASSDB_DECLARE_PROPERTY(Node, Node*, parent);
        CLASSDB_DECLARE_PROPERTY(Node, Script*, script);
        CLASSDB_DECLARE_PROPERTY(Node, std::vector<Node>, children);
    }

    void Node::OnLoad() {
        if (script) {
            script->OnLoad();
        }
        for (auto& child : children) {
            child.OnLoad();
        }
    }

    void Node::OnTick(float dt) {
        if (script) {
            script->OnTick(dt);
        }
        for (auto& child : children) {
            child.OnTick(dt);
        }
    }

    void Node::OnRender() {
        if (script) {
            script->OnRender();
        }
        for (auto& child : children) {
            child.OnRender();
        }
    }


    void Node::OnImgui() {
        if (script) {
            script->OnImgui();
        }
        for (auto& child : children) {
            child.OnImgui();
        }
    }
}