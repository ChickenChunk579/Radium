#pragma once
#include <vector>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Math.hpp>
#include <box2d/box2d.h>

namespace Radium::Nodes {

    /**
     * @enum BodyType
     * @brief Defines the type of physics body for Box2D integration.
     */
    enum class BodyType {
        Static,     ///< Static body - doesn't move, infinite mass
        Dynamic,    ///< Dynamic body - moves and responds to forces
        Kinematic   ///< Kinematic body - moves but doesn't respond to forces
    };

    /**
     * @enum CollisionType
     * @brief Defines the shape of the collision box.
     */
    enum class CollisionType {
        Rectangle,  ///< Rectangular collision box
        Circle      ///< Circular collision box
    };

    /**
     * @class RigidBody
     * @brief A 2D physics-enabled node that integrates with Box2D.
     * 
     * Inherits from Node2D to provide spatial positioning and sizing in 2D space,
     * while adding physics simulation capabilities through Box2D integration.
     * Supports different body types (static, dynamic, kinematic) and provides
     * methods for applying forces, setting velocity, and managing physics properties.
     */
    class RigidBody : public Node2D {
    public:
        /**
         * @brief Construct a new RigidBody object with default values.
         * 
         * Initializes as a dynamic body with default physics properties.
         */
        RigidBody();

        /**
         * @brief Destructor to clean up Box2D body resources.
         */
        ~RigidBody();

        /**
         * @brief Registers the RigidBody class with the ClassDB system.
         * 
         * Used for reflection, serialization, or scripting.
         */
        static void Register();

        /// Type of physics body (static, dynamic, or kinematic).
        BodyType bodyType = BodyType::Dynamic;

        /// Type of collision shape (rectangle or circle).
        CollisionType collisionType = CollisionType::Rectangle;

        /// Mass of the body (0 for static bodies).
        float mass = 1.0f;

        /// Linear damping factor (0 = no damping, 1 = full damping).
        float linearDamping = 0.0f;

        /// Angular damping factor (0 = no damping, 1 = full damping).
        float angularDamping = 0.0f;

        /// Whether the body can rotate.
        bool fixedRotation = false;

        /// Whether the body is initially awake.
        bool awake = true;

        /// Whether the body is a bullet (for fast moving objects).
        bool bullet = false;

        /// Whether the body is enabled in the physics simulation.
        bool enabled = true;

        /**
         * @brief Called when the node is loaded.
         * 
         * Creates the Box2D body and fixture.
         */
        void OnLoad() override;

        /**
         * @brief Called every tick to update the node state.
         * 
         * Synchronizes the node's position with the Box2D body's position.
         * 
         * @param dt Delta time in seconds since last update.
         */
        void OnTick(float dt) override;

        /**
         * @brief Apply a force to the center of the body.
         * 
         * @param force The force vector to apply.
         */
        void ApplyForce(const Radium::Vector2f& force);

        /**
         * @brief Apply a force at a specific point on the body.
         * 
         * @param force The force vector to apply.
         * @param point The point where to apply the force (in world coordinates).
         */
        void ApplyForceAtPoint(const Radium::Vector2f& force, const Radium::Vector2f& point);

        /**
         * @brief Apply an impulse to the center of the body.
         * 
         * @param impulse The impulse vector to apply.
         */
        void ApplyImpulse(const Radium::Vector2f& impulse);

        /**
         * @brief Apply an impulse at a specific point on the body.
         * 
         * @param impulse The impulse vector to apply.
         * @param point The point where to apply the impulse (in world coordinates).
         */
        void ApplyImpulseAtPoint(const Radium::Vector2f& impulse, const Radium::Vector2f& point);

        /**
         * @brief Set the linear velocity of the body.
         * 
         * @param velocity The new velocity vector.
         */
        void SetLinearVelocity(const Radium::Vector2f& velocity);

        /**
         * @brief Teleport the body to a new position, ignoring collision
         * 
         * @param pos Position to move to
         */
        void TeleportMove();

        /**
         * @brief Get the linear velocity of the body.
         * 
         * @return Radium::Vector2f The current velocity vector.
         */
        Radium::Vector2f GetLinearVelocity() const;

        /**
         * @brief Set the angular velocity of the body.
         * 
         * @param velocity The new angular velocity in radians per second.
         */
        void SetAngularVelocity(float velocity);

        /**
         * @brief Get the angular velocity of the body.
         * 
         * @return float The current angular velocity in radians per second.
         */
        float GetAngularVelocity() const;

        /**
         * @brief Set the rotation of the body.
         * 
         * @param angle The new rotation angle in radians.
         */
        void SetRotation(float angle);

        /**
         * @brief Get the rotation of the body.
         * 
         * @return float The current rotation angle in radians.
         */
        float GetRotation() const;

        /**
         * @brief Set whether the body is awake or sleeping.
         * 
         * @param awake True to wake the body, false to put it to sleep.
         */
        void SetAwake(bool awake);

        /**
         * @brief Check if the body is awake.
         * 
         * @return bool True if the body is awake, false if sleeping.
         */
        bool IsAwake() const;

        /**
         * @brief Get the Box2D body ID.
         * 
         * @return b2BodyId The Box2D body identifier.
         */
        b2BodyId GetBodyId() const { return bodyId; }

        /**
         * @brief Get the actual Box2D position (for debugging).
         * 
         * @return Radium::Vector2f The Box2D body position.
         */
        Radium::Vector2f GetBox2DPosition() const;

    private:
        /// Box2D body identifier.
        b2BodyId bodyId = b2_nullBodyId;

        /// Whether the Box2D body has been created.
        bool bodyCreated = false;
        
        bool teleported = false;

        /**
         * @brief Create the Box2D body and fixture.
         */
        void CreateBody();

        /**
         * @brief Destroy the Box2D body.
         */
        void DestroyBody();

        /**
         * @brief Update the Box2D body properties.
         */
        void UpdateBodyProperties();

        /**
         * @brief Convert Radium::Vector2f to b2Vec2.
         * 
         * @param vec The Radium vector.
         * @return b2Vec2 The Box2D vector.
         */
        b2Vec2 ToB2Vec2(const Radium::Vector2f& vec) const;

        /**
         * @brief Convert b2Vec2 to Radium::Vector2f.
         * 
         * @param vec The Box2D vector.
         * @return Radium::Vector2f The Radium vector.
         */
        Radium::Vector2f ToRadiumVec2(const b2Vec2& vec) const;
    };

}
