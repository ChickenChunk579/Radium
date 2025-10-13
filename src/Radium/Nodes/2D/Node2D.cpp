#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <spdlog/spdlog.h>
#include "Node2D.hpp"

namespace Radium::Nodes {
    Node2D::Node2D() {
    }

    void Node2D::Register() {
        CLASSDB_REGISTER_SUBCLASS(Node2D, Node);
        CLASSDB_DECLARE_PROPERTY(Node2D, Radium::Vector2f, globalPosition);
        CLASSDB_DECLARE_PROPERTY(Node2D, Radium::Vector2f, globalSize);
        CLASSDB_DECLARE_PROPERTY(Node2D, float, globalRotation);
        CLASSDB_DECLARE_PROPERTY(Node2D, Radium::Vector2f, position);
        CLASSDB_DECLARE_PROPERTY(Node2D, Radium::Vector2f, size);
        CLASSDB_DECLARE_PROPERTY(Node2D, float, rotation);
        
        LUA_FUNC("Radium::Nodes::Node2D::UpdateGlobalPosition", [](lua_State *L) -> int
                 {
            Node2D* instance = (Node2D*)lua_touserdata(L, lua_upvalueindex(1));
            
            instance->UpdateGlobals();

            return 0; });
    }

    void Node2D::OnLoad() {
        Node::OnLoad();
    }

    void Node2D::OnTick(float dt) {
        UpdateGlobals();
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
        if (parent) {
            Node2D* parent2D = dynamic_cast<Node2D*>(parent);
            if (parent2D) {
                position = val - parent2D->globalPosition;
            } else {
                position = val;
            }
        } else {
            position = val;
        }
    }


    Radium::Vector2f Node2D::GetSize() {
        return size;
    }

    void Node2D::SetSize(Radium::Vector2f val) {
        size = val;
    }

    void Node2D::UpdateGlobalPositionInternal(const Radium::Vector2f& parentGlobalPos) {
        globalPosition = parentGlobalPos + position;
    }

    void Node2D::UpdateGlobalSizeInternal(const Radium::Vector2f& parentGlobalSize) {
        globalSize = {parentGlobalSize.x * size.x, parentGlobalSize.y * size.y};
    }

    void Node2D::UpdateGlobalRotationInternal(const float& parentGlobalRotation) {
        globalRotation = parentGlobalRotation + rotation;
    }


    void Node2D::UpdateGlobals() {
        if (parent) {
            Node2D* parent2D = dynamic_cast<Node2D*>(parent);
            if (parent2D) {
                UpdateGlobalPositionInternal(parent2D->globalPosition);
                UpdateGlobalSizeInternal(parent2D->globalSize);
                UpdateGlobalRotationInternal(parent2D->globalRotation);
            } else {
                UpdateGlobalPositionInternal({0,0});
                UpdateGlobalSizeInternal({1,1});
                UpdateGlobalRotationInternal(0);
            }
        } else {
            UpdateGlobalPositionInternal({0,0});
            UpdateGlobalSizeInternal({1,1});
            UpdateGlobalRotationInternal(0);
        }
    }
}