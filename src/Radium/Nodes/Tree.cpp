#include <Radium/Nodes/Tree.hpp>
#include <Radium/Nodes/ChaiScript.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Math.hpp>
#include <Radium/AssetLoader.hpp>
#include <fstream>
#include <ostream>
#include <Flux/Flux.hpp>

namespace Radium::Nodes
{
    SceneTree::SceneTree(std::string name) : name(name)
    {
    }

    SceneTree::SceneTree() : name("Default") {}

    void SceneTree::Register() {
        CLASSDB_REGISTER(SceneTree);
        CLASSDB_DECLARE_PROPERTY(SceneTree, std::string, name);

        LUA_FUNC("Radium::Nodes::SceneTree::GetNodeByPath", [](lua_State* L) -> int {
            SceneTree* instance = (SceneTree*)lua_touserdata(L, lua_upvalueindex(1));
            const char* cstr = lua_tostring(L, 2);
            std::string path = std::string(cstr);

            return classdb_lua_wrap(L, instance->GetNodeByPath(path));
        });
    }

    void SceneTree::OnLoad()
    {
        for (auto &node : nodes)
        {
            node->OnLoad();
        }
    }

    void SceneTree::OnTick(float dt)
    {
        for (auto &node : nodes)
        {
            node->OnTick(dt);
        }
    }

    void SceneTree::OnRender()
    {
        for (auto &node : nodes)
        {
            node->OnRender();
        }
    }

    void SceneTree::OnImgui()
    {
        for (auto &node : nodes)
        {
            node->OnImgui();
        }
    }

    json SerializeNode(Node *node)
    {
        json nodeJson;
        std::string typeName = Demangle(typeid(*node).name());
        nodeJson["type"] = typeName;

        // Serialize script if it's a ChaiScript
        if (ClassDB::HasProperty("script", node))
        {
            auto script = ClassDB::GetProperty<Radium::Nodes::Script*>("script", node);
            if (auto chaiScript = dynamic_cast<Radium::Nodes::ChaiScript*>(script))
            {
                json scriptInfo = {
                    {"type", "ChaiScript"},
                    {"path", chaiScript->path},
                };

                nodeJson["script"] = scriptInfo;

            }
            if (auto luaScript = dynamic_cast<Radium::Nodes::LuaScript*>(script))
            {
                json scriptInfo = {
                    {"type", "LuaScript"},
                    {"path", luaScript->path},
                };

                nodeJson["script"] = scriptInfo;

            }
        }

        for (const auto &prop : ClassDB::GetProperties(node))
        {
            if (prop.name == "parent")
                continue; // avoid circular references

            if (ClassDB::IsEnum(prop.type)) {
                nodeJson[prop.name] = ClassDB::GetProperty<int>(prop.name, node);
            }

            if (prop.type == "int")
            {
                nodeJson[prop.name] = ClassDB::GetProperty<int>(prop.name, node);
            }
            else if (prop.type == "float")
            {
                nodeJson[prop.name] = ClassDB::GetProperty<float>(prop.name, node);
            }
            else if (prop.type == "unsigned int")
            {
                nodeJson[prop.name] = ClassDB::GetProperty<unsigned int>(prop.name, node);
            }
            #if !defined(_MSC_VER)
            #if defined(__EMSCRIPTEN__)
            else if (prop.type == "std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char>>")
            #else
            #if defined(__ANDROID__)
            else if (prop.type == "std::__ndk1::basic_string<char, std::__ndk1::char_traits<char>, std::__ndk1::allocator<char>>")
            #else
            else if (prop.type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >")
            #endif
            #endif
            #else
            else if (prop.type == "std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >")
            #endif
            {
                nodeJson[prop.name] = ClassDB::GetProperty<std::string>(prop.name, node);
            }
            else if (prop.type == "bool")
            {
                nodeJson[prop.name] = ClassDB::GetProperty<bool>(prop.name, node);
            }
            else if (prop.type == "Radium::Vector2f")
            {
                auto v = ClassDB::GetProperty<Radium::Vector2f>(prop.name, node);
                nodeJson[prop.name] = {v.x, v.y};
            }
            else if (prop.type == "Radium::RectangleF")
            {
                auto r = ClassDB::GetProperty<Radium::RectangleF>(prop.name, node);
                nodeJson[prop.name] = {r.x, r.y, r.w, r.h};
            }
            
        }

        // Recursively serialize children
        nodeJson["children"] = json::array();
        for (Node* child : node->children)
        {
            nodeJson["children"].push_back(SerializeNode(child));
        }

        return nodeJson;
    }

    void SceneTree::Serialize(std::string path)
    {
        json j;
        j["name"] = name;
        j["nodes"] = json::array();

        for (Node *node : nodes)
        {
            j["nodes"].push_back(SerializeNode(node));
        }

        std::ofstream file(path);
        if (file.is_open())
        {
            file << j.dump(4);
            file.close();
        }
    }

    Node *DeserializeNode(const json &nodeJson, SceneTree* tree, Node *parent, bool stubScripts)
    {
        std::string typeName = nodeJson.value("type", "");
        if (typeName.empty())
        {
            Flux::Warn("Node type missing");
            return nullptr;
        }

        Flux::Trace("Creating type: {}", typeName);

        Node *node = dynamic_cast<Node *>(ClassDB::Create(typeName));
        Flux::Trace("Post create");
        if (!node)
        {
            Flux::Error("Failed to create node of type {}", typeName);
            return nullptr;
        }

        if (nodeJson.contains("script"))
        {
            const auto& scriptJson = nodeJson["script"];
            std::string scriptType = scriptJson.value("type", "");
            if (scriptType == "ChaiScript")
            {
                // Create a ChaiScript instance
                // Set the path property from JSON
                if (scriptJson.contains("path"))
                {
                    std::string path = scriptJson["path"].get<std::string>();
                    auto chaiScript = new Radium::Nodes::ChaiScript(path, tree, !stubScripts);

                    chaiScript->me = node;

                    // Assign script property on the node
                    ClassDB::SetProperty<Radium::Nodes::Script*>("script", node, chaiScript);
                }
                else
                {
                    Flux::Error("Failed to create ChaiScript instance during deserialization");
                }
            }
            else if (scriptType == "LuaScript") {
                // Create a LuaScript instance
                // Set the path property from JSON
                if (scriptJson.contains("path"))
                {
                    std::string path = scriptJson["path"].get<std::string>();
                    auto luaScript = new Radium::Nodes::LuaScript(path, tree, !stubScripts);

                    luaScript->me = node;

                    // Assign script property on the node
                    ClassDB::SetProperty<Radium::Nodes::LuaScript*>("script", node, luaScript);
                }
                else
                {
                    Flux::Error("Failed to create LuaScript instance during deserialization");
                }
            }
            else
            {
                Flux::Warn("Unsupported script type during deserialization: {}", scriptType);
            }
        }

        for (const auto &prop : ClassDB::GetProperties(node))
        {
            if (prop.name == "parent")
                continue; // we handle this manually below

            if (!nodeJson.contains(prop.name))
                continue;
            
            try
            {
                Flux::Info("Is it a string or not: {}", prop.type);

                if (ClassDB::IsEnum(prop.type)) {
                    ClassDB::SetProperty<int>(prop.name, node, nodeJson[prop.name].get<int>());
                }

                if (prop.type == "int")
                {
                    ClassDB::SetProperty<int>(prop.name, node, nodeJson[prop.name].get<int>());
                }
                else if (prop.type == "float")
                {
                    ClassDB::SetProperty<float>(prop.name, node, nodeJson[prop.name].get<float>());
                }
                else if (prop.type == "unsigned int")
                {
                    ClassDB::SetProperty<unsigned int>(prop.name, node, nodeJson[prop.name].get<unsigned int>());
                }
                #if !defined(_MSC_VER)
                #if defined(__EMSCRIPTEN__)
                else if (prop.type == "std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char>>")
                #else
                #if defined(__ANDROID__)
                else if (prop.type == "std::__ndk1::basic_string<char, std::__ndk1::char_traits<char>, std::__ndk1::allocator<char>>")
                #else
                else if (prop.type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >")
                #endif
                #endif
                #else
                else if (prop.type == "std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >")
                #endif
                {
                    ClassDB::SetProperty<std::string>(prop.name, node, nodeJson[prop.name].get<std::string>());
                }
                else if (prop.type == "bool")
                {
                    ClassDB::SetProperty<bool>(prop.name, node, nodeJson[prop.name].get<bool>());
                }
                else if (prop.type == "Radium::Vector2f")
                {
                    auto arr = nodeJson[prop.name];
                    if (arr.is_array() && arr.size() == 2)
                    {
                        Radium::Vector2f v{arr[0].get<float>(), arr[1].get<float>()};
                        ClassDB::SetProperty<Radium::Vector2f>(prop.name, node, v);
                    }
                }
                else if (prop.type == "Radium::RectangleF")
                {
                    auto arr = nodeJson[prop.name];
                    if (arr.is_array() && arr.size() == 4)
                    {
                        Radium::RectangleF r{arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>()};
                        ClassDB::SetProperty<Radium::RectangleF>(prop.name, node, r);
                    }
                }
            }
            catch (std::exception &e)
            {
                Flux::Error("Error deserializing property {}: {}", prop.name, e.what());
            }
        }

        // Set parent manually
        node->parent = parent;

        // Deserialize children recursively
        if (nodeJson.contains("children"))
        {
            for (const auto &childJson : nodeJson["children"])
            {
                Node *child = DeserializeNode(childJson, tree, node);
                if (child)
                {
                    node->children.push_back(child);
                }
            }
        }

        return node;
    }

    void SceneTree::Deserialize(std::string path, bool stubScripts, bool external)
    {
        std::string file = Radium::ReadFileToString(path, external);

        Flux::Trace("Got file: {}", file);

        json j = json::parse(file);
        Flux::Trace("Created json");

        name = j.value("name", "UnnamedScene");
        Flux::Trace("Name: {}", name);
        nodes.clear();
        Flux::Trace("Cleared nodes");

        for (const auto &nodeJson : j["nodes"])
        {
            Flux::Trace("Before deserialize");
            Node *node = DeserializeNode(nodeJson, this, nullptr, stubScripts);
            Flux::Trace("Post deserialize");
            if (node)
            {
                Flux::Trace("Added node: {}", node->name);
                nodes.push_back(node);
            }
        }
    }

    // Recursive helper to update global position for node and all descendants
    void UpdateGlobalPositionsRecursive(Node* node) {
        if (!node) return;

        // Try to cast to Node2D to call UpdateGlobalPosition
        if (auto node2D = dynamic_cast<Node2D*>(node)) {
            node2D->UpdateGlobals();
        }

        // Recurse for all children
        for (auto* child : node->children) {
            UpdateGlobalPositionsRecursive(child);
        }
    }

    // Call this on your SceneTree to update all nodes' global positions
    void SceneTree::UpdateAllGlobalPositions() {
        for (auto* rootNode : nodes) {
            UpdateGlobalPositionsRecursive(rootNode);
        }
    }

    Node* SceneTree::GetNodeByPath(std::string path) {
        if (path.empty()) return nullptr;

        // Remove leading slash if present
        if (path.front() == '/') {
            path.erase(0, 1);
        }

        // Split the path by '/'
        std::vector<std::string> parts;
        size_t start = 0;
        while (true) {
            size_t pos = path.find('/', start);
            if (pos == std::string::npos) {
                parts.push_back(path.substr(start));
                break;
            }
            parts.push_back(path.substr(start, pos - start));
            start = pos + 1;
        }

        // Search for the first part in root nodes
        Node* current = nullptr;
        for (auto* rootNode : nodes) {
            if (rootNode->name == parts[0]) {
                current = rootNode;
                break;
            }
        }

        if (!current) return nullptr;

        // Traverse down children for the remaining parts
        for (size_t i = 1; i < parts.size(); ++i) {
            const auto& part = parts[i];
            Node* next = nullptr;
            for (auto* child : current->children) {
                if (child->name == part) {
                    next = child;
                    break;
                }
            }
            if (!next) {
                return nullptr; // part not found in children
            }
            current = next;
        }

        return current;
    }


}