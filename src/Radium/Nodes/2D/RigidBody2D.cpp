#include <Radium/Nodes/2D/RigidBody2D.hpp>
#include <Radium/Application.hpp>
#include <box2d/box2d.h>
#include <spdlog/spdlog.h>

namespace Radium::Nodes {

    RigidBody2D::RigidBody2D() {}

    void RigidBody2D::Register() {
        
    }

    void RigidBody2D::OnLoad() {
        Node2D::OnLoad();

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = {position.x.get<float>(), position.y.get<float>()};
        bodyDef.type = isStatic ? b2_staticBody : b2_dynamicBody;

        b2WorldId worldId = Radium::currentApplication->worldId;
        bodyId = b2CreateBody(worldId, &bodyDef);

        b2Polygon box = b2MakeBox(width / 2.0f, height / 2.0f);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = density;
        shapeDef.material.friction = friction;

        b2CreatePolygonShape(bodyId, &shapeDef, &box);
    }

    void RigidBody2D::OnTick(float dt) {
        Node2D::OnTick(dt);

        b2Vec2 pos = b2Body_GetPosition(bodyId);
        position.x = pos.x;
        position.y = pos.y;

        // Optional: update global position manually
        if (parent) {
            Node2D* parent2D = dynamic_cast<Node2D*>(parent);
            if (parent2D) {
                UpdateGlobalPosition(parent2D->globalPosition);
            }
        } else {
            UpdateGlobalPosition({0, 0});
        }
    }

    void RigidBody2D::AddForce(Radium::Vector2f force) {
        b2Body_ApplyForceToCenter(bodyId, {force.x.get<float>(), force.y.get<float>()}, true);
    }

    void RigidBody2D::SetVelocity(Radium::Vector2f force) {
        b2Body_SetLinearVelocity(bodyId, {force.x.get<float>(), force.y.get<float>()});
    }

}
