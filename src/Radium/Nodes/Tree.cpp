#include <Radium/Nodes/Tree.hpp>
#include <Radium/Math.hpp>
#include <fstream>
#include <ostream>
#include <spdlog/spdlog.h>

namespace Radium::Nodes
{
    SceneTree::SceneTree(std::string name) : name(name)
    {
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

        for (const auto &prop : ClassDB::GetProperties(node))
        {
            spdlog::info("Serializing property {} of type {}", prop.name, prop.type);

            if (prop.name == "parent")
                continue; // avoid circular references

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
            else if (prop.type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >")
            {
                nodeJson[prop.name] = ClassDB::GetProperty<std::string>(prop.name, node);
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

    Node *DeserializeNode(const json &nodeJson, Node *parent = nullptr)
    {
        std::string typeName = nodeJson.value("type", "");
        if (typeName.empty())
        {
            spdlog::warn("Node type missing");
            return nullptr;
        }

        Node *node = dynamic_cast<Node *>(ClassDB::Create(typeName));
        if (!node)
        {
            spdlog::error("Failed to create node of type {}", typeName);
            return nullptr;
        }

        for (const auto &prop : ClassDB::GetProperties(node))
        {
            if (prop.name == "parent")
                continue; // we handle this manually below

            if (!nodeJson.contains(prop.name))
                continue;

            spdlog::info("Deserializing property {} of type {}", prop.name, prop.type);

            try
            {
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
                else if (prop.type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >")
                {
                    ClassDB::SetProperty<std::string>(prop.name, node, nodeJson[prop.name].get<std::string>());
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
                spdlog::error("Error deserializing property {}: {}", prop.name, e.what());
            }
        }

        // Set parent manually
        node->parent = parent;

        // Deserialize children recursively
        if (nodeJson.contains("children"))
        {
            for (const auto &childJson : nodeJson["children"])
            {
                Node *child = DeserializeNode(childJson, node);
                if (child)
                {
                    node->children.push_back(child);
                }
            }
        }

        return node;
    }

    void SceneTree::Deserialize(std::string path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            spdlog::error("Failed to open file: {}", path);
            return;
        }

        json j;
        file >> j;
        file.close();

        name = j.value("name", "UnnamedScene");
        nodes.clear();

        for (const auto &nodeJson : j["nodes"])
        {
            Node *node = DeserializeNode(nodeJson);
            if (node)
            {
                nodes.push_back(node);
            }
        }
    }

}