#include <Radium/Nodes/2D/Node2D.hpp>
#include <spdlog/spdlog.h>

namespace Radium::Nodes {

    void Node2D::UpdateGlobalPosition(const Radium::Vector2f& parentGlobalPos) {
        globalPosition = parentGlobalPos + position;
    }

    Node2D::Node2D() : position(0.0f, 0.0f) {
        
    }

    void Node2D::Register() {
        
    }

    void Node2D::OnLoad() {
        Node::OnLoad();
    }

    void Node2D::OnTick(float dt) {
        if (parent) {
            Node2D* parent2D = dynamic_cast<Node2D*>(parent);
            if (parent2D) {
                UpdateGlobalPosition(parent2D->globalPosition);
            } else {
                UpdateGlobalPosition({0,0});
            }
        } else {
            UpdateGlobalPosition({0,0});
        }
        Node::OnTick(dt);
    }

    void Node2D::OnRender() {
        Node::OnRender();
    }

    void Node2D::OnImgui() {
        Node::OnImgui();
    }
}