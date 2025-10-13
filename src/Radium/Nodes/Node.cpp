#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <spdlog/spdlog.h>

namespace Radium::Nodes {
    Node::Node() : parent(nullptr) {
        name = std::string(Demangle(typeid(this).name()));
    }

    void Node::Register() {
        CLASSDB_REGISTER(Node);
        CLASSDB_DECLARE_PROPERTY(Node, Node*, parent);
        CLASSDB_DECLARE_PROPERTY(Node, Script*, script);
        CLASSDB_DECLARE_PROPERTY(Node, std::vector<Node>, children);
        CLASSDB_DECLARE_PROPERTY(Node, std::string, name);
    
        LUA_FUNC("Radium::Nodes::Node::GetChildByName", [](lua_State *L) -> int
                 {
            Node* instance = (Node*)lua_touserdata(L, lua_upvalueindex(1));
            const char* name = lua_tostring(L, 2);
            
            return classdb_lua_wrap(L, instance->GetChildByName(std::string(name))); });
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

    Node* Node::GetChildByName(std::string name) {
        for (auto& child : children) {
            if (child) {
                if (child->name.data()) {
                    spdlog::info("{}", child->name);
                    if (child->name == name) {
                        return child;
                    }
                }
                
            }
            
        }
    }
}