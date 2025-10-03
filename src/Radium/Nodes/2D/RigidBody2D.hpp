#pragma once

#include <Radium/Nodes/2D/Node2D.hpp>
#include <box2d/box2d.h>

namespace Radium::Nodes {

    class RigidBody2D : public Node2D {
    public:
        RigidBody2D();

        static void Register();

        float width = 1.0f;
        float height = 1.0f;
        float density = 1.0f;
        float friction = 0.3f;
        bool isStatic = false;

        void OnLoad() override;
        void OnTick(float dt) override;

        void AddForce(Radium::Vector2f force);
        void SetVelocity(Radium::Vector2f velocity);

    private:
        b2BodyId bodyId = {0};
    };

}
