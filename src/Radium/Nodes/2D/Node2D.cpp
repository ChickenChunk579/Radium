#include <Radium/Nodes/2D/Node2D.hpp>
#include <spdlog/spdlog.h>
#include "Node2D.hpp"

namespace Radium::Nodes {
    Node2D::Node2D() {
    }

    void Node2D::Register() {
        CLASSDB_REGISTER_SUBCLASS(Node2D, Node);
        CLASSDB_DECLARE_PROPERTY(Node2D, Radium::Vector2f, position);
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

    Radium::Vector2f Node2D::GetPosition() {
        return position;
    }

    void Node2D::SetPosition(Radium::Vector2f val) {
        position = val;
    }

    Radium::Vector2f Node2D::GetGlobalPosition() {
        return globalPosition;
    }

    void Node2D::SetGlobalPosition(Radium::Vector2f val) {
        globalPosition = val;
    }

    Radium::Vector2f Node2D::GetSize() {
        return size;
    }

    void Node2D::SetSize(Radium::Vector2f val) {
        size = val;
    }

    void Node2D::UpdateGlobalPosition(const Radium::Vector2f& parentGlobalPos) {
        globalPosition = parentGlobalPos + position;
    }
}