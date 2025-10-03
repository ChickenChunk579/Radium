#pragma once
#include <vector>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Math.hpp>

namespace Radium::Nodes {
    class Node2D : public Node {
    public:
        Node2D();

        static void Register();

        Radium::Vector2f position;
        Radium::Vector2f globalPosition = {0, 0};

        void OnLoad() override;
        void OnTick(float dt) override;
        void OnRender() override;
        void OnImgui() override;

        void UpdateGlobalPosition(const Radium::Vector2f& parentGlobalPos = {0,0});
    };
}