#include <Radium/Nodes/2D/RigidBody.hpp>
#include <Radium/Application.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <spdlog/spdlog.h>
#include <box2d/box2d.h>
#include <Radium/DebugRenderer.hpp>

namespace Radium::Nodes
{

    RigidBody::RigidBody()
    {
        awake = true;
        enabled = true;
        bodyType = BodyType::Dynamic;
        linearDamping = 0.0f;
        angularDamping = 0.0f;
        fixedRotation = false;
        bullet = false;
    }

    RigidBody::~RigidBody()
    {
        DestroyBody();
    }

    void RigidBody::Register()
    {
        CLASSDB_REGISTER_SUBCLASS(RigidBody, Node2D);
        CLASSDB_DECLARE_PROPERTY(RigidBody, BodyType, bodyType);
        CLASSDB_DECLARE_PROPERTY(RigidBody, CollisionType, collisionType);
        CLASSDB_DECLARE_PROPERTY(RigidBody, float, mass);
        CLASSDB_DECLARE_PROPERTY(RigidBody, float, linearDamping);
        CLASSDB_DECLARE_PROPERTY(RigidBody, float, angularDamping);
        CLASSDB_DECLARE_PROPERTY(RigidBody, bool, fixedRotation);
        CLASSDB_DECLARE_PROPERTY(RigidBody, bool, awake);
        CLASSDB_DECLARE_PROPERTY(RigidBody, bool, bullet);
        CLASSDB_DECLARE_PROPERTY(RigidBody, bool, enabled);

        // Lua function bindings
        LUA_FUNC("Radium::Nodes::RigidBody::ApplyForce", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float x = lua_tonumber(L, 2);  // fix: use 2 and 3 instead of 1 and 2
            float y = lua_tonumber(L, 3);
            
            spdlog::info("Apply force: {}, {}", x, y);
            instance->ApplyForce({x, y});
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::ApplyForceAtPoint", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float forceX = lua_tonumber(L, 2);
            float forceY = lua_tonumber(L, 3);
            float pointX = lua_tonumber(L, 4);
            float pointY = lua_tonumber(L, 5);
            
            instance->ApplyForceAtPoint({forceX, forceY}, {pointX, pointY});
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::ApplyImpulse", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float x = lua_tonumber(L, 2);
            float y = lua_tonumber(L, 3);
            
            instance->ApplyImpulse({x, y});
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::ApplyImpulseAtPoint", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float impulseX = lua_tonumber(L, 2);
            float impulseY = lua_tonumber(L, 3);
            float pointX = lua_tonumber(L, 4);
            float pointY = lua_tonumber(L, 5);
            
            instance->ApplyImpulseAtPoint({impulseX, impulseY}, {pointX, pointY});
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::SetLinearVelocity", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float x = lua_tonumber(L, 2);
            float y = lua_tonumber(L, 3);
            
            instance->SetLinearVelocity({x, y});
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::TeleportMove", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            
            instance->TeleportMove();
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::GetLinearVelocity", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            Radium::Vector2f velocity = instance->GetLinearVelocity();
            
            return classdb_lua_wrap(L, &velocity); });

        LUA_FUNC("Radium::Nodes::RigidBody::SetAngularVelocity", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float velocity = lua_tonumber(L, 2);
            
            instance->SetAngularVelocity(velocity);
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::GetAngularVelocity", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float velocity = instance->GetAngularVelocity();
            
            lua_pushnumber(L, velocity);
            return 1; });

        LUA_FUNC("Radium::Nodes::RigidBody::SetRotation", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float angle = lua_tonumber(L, 2);
            
            instance->SetRotation(angle);
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::GetRotation", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            float angle = instance->GetRotation();
            
            lua_pushnumber(L, angle);
            return 1; });

        LUA_FUNC("Radium::Nodes::RigidBody::SetAwake", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            bool awake = lua_toboolean(L, 1);
            
            instance->SetAwake(awake);
            return 0; });

        LUA_FUNC("Radium::Nodes::RigidBody::IsAwake", [](lua_State *L) -> int
                 {
            RigidBody* instance = (RigidBody*)lua_touserdata(L, lua_upvalueindex(1));
            bool awake = instance->IsAwake();
            
            lua_pushboolean(L, awake);
            return 1; });
    }

    void RigidBody::OnLoad()
    {
        Node2D::OnLoad();
        CreateBody();
    }

    void RigidBody::OnTick(float dt)
    {
        if (bodyCreated && enabled)
        {
            UpdateBodyProperties();
            
            if (bodyType == BodyType::Dynamic) {
                // Sync position from Box2D to node
                b2Vec2 b2Pos = b2Body_GetPosition(bodyId);
                float b2Angle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));

                // Update the global position directly without calling SetGlobalPosition
                // to avoid triggering the node hierarchy update
                SetGlobalPosition(ToRadiumVec2(b2Pos));

                rotation = -b2Angle * (180.0f / 3.14159f);


                
                
                // Note: Box2D uses radians, we might want to store rotation separately
                // For now, we'll just update the position
            } else {
                if (teleported) {
                    teleported = false;
                    return;
                }
                // For static/kinematic bodies, sync from node to Box2D
                b2Vec2 pos = ToB2Vec2(globalPosition);
                b2Body_SetTargetTransform(bodyId, {pos, b2Rot_identity}, 1.0f / 60.0f);
            }
            
            // Debug render the collision shape
            b2Vec2 b2Pos = b2Body_GetPosition(bodyId);
            
            if (collisionType == CollisionType::Rectangle) {
                float rotDegrees = (b2Rot_GetAngle(b2Body_GetRotation(bodyId)) / 3.14159f) * 180.0f;
                if (bodyType == BodyType::Dynamic) {
                    Radium::DebugRenderer::AddRect({b2Pos.x - (size.x / 2), b2Pos.y - (size.y / 2), size.x, size.y}, 0, 1, 0, 10, CoordinateOrigin::Center, rotDegrees);
                } else {
                    Radium::DebugRenderer::AddRect({b2Pos.x - (size.x / 2), b2Pos.y - (size.y / 2), size.x, size.y}, 1, 0, 0, 10, CoordinateOrigin::Center, rotDegrees);
                }
            } else if (collisionType == CollisionType::Circle) {
                // For circles, use the larger dimension as radius
                float radius = std::max(size.x, size.y) / 2.0f;
                if (bodyType == BodyType::Dynamic) {
                    Radium::DebugRenderer::AddCircleOutline({b2Pos.x, b2Pos.y}, 0, 1, 0, radius, 32, CoordinateOrigin::Center);
                } else {
                    Radium::DebugRenderer::AddCircleOutline({b2Pos.x, b2Pos.y}, 1, 0, 0, radius, 32, CoordinateOrigin::Center);
                }
            }
        }
        else if (bodyCreated && !enabled)
        {
            // Ensure the body stays disabled
            if (b2Body_IsEnabled(bodyId)) {
                spdlog::info("Disabled body");
                b2Body_Disable(bodyId);
            }
        }

        teleported = false;
        Node2D::OnTick(dt);
    }

    void RigidBody::TeleportMove() {
        teleported = true;
        UpdateGlobals();
        spdlog::info("Global position: {}, {}", globalPosition.x, globalPosition.y);
        b2Body_SetTransform(bodyId, ToB2Vec2(globalPosition), b2Rot_identity);
    }

    void RigidBody::ApplyForce(const Radium::Vector2f &force)
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 b2Force = ToB2Vec2(force);
            b2Body_ApplyForce(bodyId, b2Force, b2Body_GetPosition(bodyId), true);
        }
    }

    void RigidBody::ApplyForceAtPoint(const Radium::Vector2f &force, const Radium::Vector2f &point)
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 b2Force = ToB2Vec2(force);
            b2Vec2 b2Point = ToB2Vec2(point);
            b2Body_ApplyForce(bodyId, b2Force, b2Point, true);
        }
    }

    void RigidBody::ApplyImpulse(const Radium::Vector2f &impulse)
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 b2Impulse = ToB2Vec2(impulse);
            b2Body_ApplyLinearImpulse(bodyId, b2Impulse, b2Body_GetPosition(bodyId), true);
        }
    }

    void RigidBody::ApplyImpulseAtPoint(const Radium::Vector2f &impulse, const Radium::Vector2f &point)
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 b2Impulse = ToB2Vec2(impulse);
            b2Vec2 b2Point = ToB2Vec2(point);
            b2Body_ApplyLinearImpulse(bodyId, b2Impulse, b2Point, true);
        }
    }

    void RigidBody::SetLinearVelocity(const Radium::Vector2f &velocity)
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 b2Velocity = ToB2Vec2(velocity);
            b2Body_SetLinearVelocity(bodyId, b2Velocity);
        }
    }

    Radium::Vector2f RigidBody::GetLinearVelocity() const
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 b2Velocity = b2Body_GetLinearVelocity(bodyId);
            return ToRadiumVec2(b2Velocity);
        }
        return {0, 0};
    }

    void RigidBody::SetAngularVelocity(float velocity)
    {
        if (bodyCreated && enabled)
        {
            b2Body_SetAngularVelocity(bodyId, velocity);
        }
    }

    float RigidBody::GetAngularVelocity() const
    {
        if (bodyCreated && enabled)
        {
            return b2Body_GetAngularVelocity(bodyId);
        }
        return 0.0f;
    }

    void RigidBody::SetRotation(float angle)
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 currentPos = b2Body_GetPosition(bodyId);
            b2Body_SetTransform(bodyId, currentPos, b2MakeRot((angle / 180.0f) * 3.14159f));
        }
    }

    float RigidBody::GetRotation() const
    {
        if (bodyCreated && enabled)
        {
            return b2Rot_GetAngle(b2Body_GetRotation(bodyId));
        }
        return 0.0f;
    }

    void RigidBody::SetAwake(bool awake)
    {
        if (bodyCreated && enabled)
        {
            b2Body_SetAwake(bodyId, awake);
        }
    }

    bool RigidBody::IsAwake() const
    {
        if (bodyCreated && enabled)
        {
            return b2Body_IsAwake(bodyId);
        }
        return false;
    }

    void RigidBody::CreateBody()
    {
        if (bodyCreated || !Radium::currentApplication)
            return;

        // Update global position before creating body
        UpdateGlobals();

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = ToB2Vec2(globalPosition);
        bodyDef.rotation = b2MakeRot((globalRotation / 180.0f) * 3.14159f);
        bodyDef.linearDamping = linearDamping;
        bodyDef.angularDamping = angularDamping;
        bodyDef.isAwake = awake;
        bodyDef.isBullet = bullet;
        bodyDef.isEnabled = enabled;
        
        if (bodyType == BodyType::Dynamic)
            bodyDef.type = b2_dynamicBody;
        else if (bodyType == BodyType::Kinematic)
            bodyDef.type = b2_kinematicBody;
        else if (bodyType == BodyType::Static)
            bodyDef.type = b2_staticBody;

        bodyId = b2CreateBody(Radium::currentApplication->worldId, &bodyDef);
        if (bodyId.index1 == b2_nullBodyId.index1)
        {
            spdlog::error("Failed to create Box2D body");
            return;
        }

        // Only create shape if size is valid
        if (size.x > 0 && size.y > 0) {
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            
            // Set density based on body type
            if (bodyType == BodyType::Static) {
                shapeDef.density = 0.0f;
            } else {
                shapeDef.density = mass;
            }
            
            shapeDef.material.friction = 0.3f;
            shapeDef.material.restitution = 0.1f;

            if (collisionType == CollisionType::Rectangle) {
                b2Polygon box = b2MakeBox(size.x / 2.0f, size.y / 2.0f);
                b2CreatePolygonShape(bodyId, &shapeDef, &box);
            } else if (collisionType == CollisionType::Circle) {
                // For circles, use the larger dimension as radius
                float radius = std::max(size.x, size.y) / 2.0f;
                b2Circle circle;
                circle.center = {0.0f, 0.0f};
                circle.radius = radius;
                b2CreateCircleShape(bodyId, &shapeDef, &circle);
            }
        }

        bodyCreated = true;
        spdlog::debug("Created body {} at position ({}, {}) with size ({}, {}) and collision type {}", 
                     name, globalPosition.x, globalPosition.y, size.x, size.y, 
                     collisionType == CollisionType::Circle ? "Circle" : "Rectangle");
    }

    void RigidBody::DestroyBody()
    {
        if (bodyCreated && bodyId.index1 != b2_nullBodyId.index1)
        {
            b2DestroyBody(bodyId);
            bodyId = b2_nullBodyId;
            bodyCreated = false;
            spdlog::debug("Destroyed Box2D body for RigidBody node: {}", name);
        }
    }

    void RigidBody::UpdateBodyProperties()
    {
        if (!bodyCreated)
        {
            return;
        }

        // Update body properties
        b2Body_SetLinearDamping(bodyId, linearDamping);
        b2Body_SetAngularDamping(bodyId, angularDamping);
        b2Body_SetBullet(bodyId, bullet);
        b2Body_SetAwake(bodyId, awake);
        if (enabled)
            b2Body_Enable(bodyId);
        else
            b2Body_Disable(bodyId);
    }

    b2Vec2 RigidBody::ToB2Vec2(const Radium::Vector2f &vec) const
    {
        return {vec.x, vec.y};
    }

    Radium::Vector2f RigidBody::ToRadiumVec2(const b2Vec2 &vec) const
    {
        return {vec.x, vec.y};
    }

    Radium::Vector2f RigidBody::GetBox2DPosition() const
    {
        if (bodyCreated && enabled)
        {
            b2Vec2 b2Pos = b2Body_GetPosition(bodyId);
            return ToRadiumVec2(b2Pos);
        }
        return globalPosition;
    }

}
