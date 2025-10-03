#pragma once
#include <vector>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Math.hpp>

namespace Radium::Nodes {
    class Node2D : public Node {
    public:
        Node2D();

        static void Register();

        Radium::Vector2f position;

        void OnLoad() override;
        void OnTick(float dt) override;
        void OnRender() override;
        void OnImgui() override;
    };
}