#pragma once
/*
#include <entt/entt.hpp>
#include <Radium/Components/RigidBody.hpp>
#include <Radium/Application.hpp>

namespace Radium::Physics {
    inline void ApplyForce(Radium::Components::RigidBody rb, float forceX, float forceY) {
        b2Vec2 force = { forceX, forceY };
        b2Body_ApplyForce(rb.body, force, b2Body_GetPosition(rb.body), true); // true = wake the body
    }

    inline void SetVelocity(Radium::Components::RigidBody rb, float forceX, float forceY) {
        b2Vec2 velocity = { forceX, forceY };
        b2Body_SetLinearVelocity(rb.body, velocity); // true = wake the body
    }


    inline void StaticBodyTeleport(entt::entity ent, float x, float y) {
        auto sb = Radium::currentApplication->registry.get<Radium::Components::StaticBody>(ent);
        auto pos = Radium::currentApplication->registry.get<Radium::Components::Position>(ent);

        b2Vec2 position = { x, y };
        b2Body_SetTransform(sb.body, position, b2Rot_identity); // true = wake the body
    
        pos.position.x = x;
        pos.position.y = y;
    }
}
    */
