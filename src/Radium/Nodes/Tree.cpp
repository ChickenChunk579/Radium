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
        return nullptr;
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
        return nullptr;
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