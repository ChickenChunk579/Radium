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

        Radium::Vector2f position = {0, 0};
        Radium::Vector2f globalPosition = {0, 0};

        Radium::Vector2f size = {0, 0};

        void OnLoad() override;
        void OnTick(float dt) override;
        void OnRender() override;
        void OnImgui() override;

        Radium::Vector2f GetPosition();
        void SetPosition(Radium::Vector2f val);

        Radium::Vector2f GetGlobalPosition();
        void SetGlobalPosition(Radium::Vector2f val);

        Radium::Vector2f GetSize();
        void SetSize(Radium::Vector2f val);

        void UpdateGlobalPosition();
    private:
        void UpdateGlobalPositionInternal(const Radium::Vector2f& parentGlobalPos = {0,0});

    };
}