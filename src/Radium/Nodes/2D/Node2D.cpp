#include <Radium/Nodes/2D/Node2D.hpp>
#include <spdlog/spdlog.h>

namespace Radium::Nodes {
    Node2D::Node2D() : position(0.0f, 0.0f) {
        
    }

    void Node2D::Register() {
        CLASSDB_REGISTER_SUBCLASS(Node2D, Node);
        CLASSDB_DECLARE_PROPERTY(Node2D, Radium::Vector2f, position);
    }

    void Node2D::OnLoad() {
        Node::OnLoad();
    }

    void Node2D::OnTick(float dt) {
        Node::OnTick(dt);
    }

    void Node2D::OnRender() {
        Node::OnRender();
    }

    void Node2D::OnImgui() {
        Node::OnImgui();
    }
}