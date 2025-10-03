#include <Radium/Nodes/Node.hpp>
#include <spdlog/spdlog.h>
#include <Radium/Nodes/Script.hpp>

namespace Radium::Nodes {
    Node::Node() : parent(nullptr) {
        
    }

    void Node::Register() {

    }

    void Node::OnLoad() {
        if (script) {
            script->OnLoad();
        }
        for (auto& child : children) {
            child->OnLoad();
        }
    }

    void Node::OnTick(float dt) {
        if (script) {
            script->OnTick(dt);
        }
        for (auto& child : children) {
            child->OnTick(dt);
        }
    }

    void Node::OnRender() {
        if (script) {
            script->OnRender();
        }
        for (auto& child : children) {
            child->OnRender();
        }
    }


    void Node::OnImgui() {
        if (script) {
            script->OnImgui();
        }
        for (auto& child : children) {
            child->OnImgui();
        }
    }
}