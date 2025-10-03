#include <Radium/Application.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/2D/RigidBody2D.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <Radium/FontRenderer.hpp>
#include <Radium/PhysicsUtil.hpp>
#include <Radium/Input.hpp>
#include <Radium/AssetLoader.hpp>
#include <Rune/GeometryRenderer.hpp>
#include "imgui.h"
#include <spdlog/spdlog.h>
#include <Radium/Random.hpp>

class MyApp : public Radium::Application
{
public:
    std::string GetTitle() override
    {
        return "My Empty Radium App";
    }

    Radium::Vector2i GetPreferredSize() override
    {
        return {1280, 720};
    }

    Radium::Vector2f GetGravity() override
    {
        return {0.0f, -200.0f};
    }

    void OnLoad() override
    {
        
        Radium::SpriteBatchRegistry::Add("flappy", "texture.png", Rune::SpriteOrigin::TopLeft, Rune::SamplingMode::Nearest);
        Radium::SpriteBatchRegistry::Add("flappyCenter", "texture.png", Rune::SpriteOrigin::Center, Rune::SamplingMode::Nearest);

        Radium::Vector2f::Register();
        Radium::Vector2i::Register();
        Radium::RectangleF::Register();

        Radium::Nodes::Node::Register();
        Radium::Nodes::Node2D::Register();
        Radium::Nodes::Sprite2D::Register();
        Radium::Nodes::RigidBody2D::Register();

        Radium::Nodes::Node2D *backgroundCollection = new Radium::Nodes::Node2D();

        for (int i = 0; i < 4; i++)
        {
            Radium::Nodes::Sprite2D *background = new Radium::Nodes::Sprite2D();
            background->batchTag = "flappy";
            background->sourceRect = {0, 0, 288, 512};
            background->textureWidth = 1024;
            background->textureHeight = 1024;
            background->position = {0.0f, 0.0f};
            background->r = 1.0f;
            background->g = 1.0f;
            background->b = 1.0f;
            background->z = -10.0f;
            background->origin = Radium::Nodes::CoordinateOrigin::TopLeft;
            background->position.x = i * 288;

            background->parent = backgroundCollection;
            backgroundCollection->children.push_back(background);
        }
        tree.nodes.push_back(backgroundCollection);

        float gap = 128.0f;
        float pipeHeight = 320.0f;

        float x = 0.0f;       // group is centered for now

        Radium::Nodes::Node2D *pipeGroup = new Radium::Nodes::Node2D();

        // Bottom pipe
        Radium::Nodes::Sprite2D *bottomPipe = new Radium::Nodes::Sprite2D();
        bottomPipe->batchTag = "flappyCenter";
        bottomPipe->sourceRect = {0, 624, 52, 320};
        bottomPipe->textureWidth = 1024;
        bottomPipe->textureHeight = 1024;
        bottomPipe->position = {0.0f, -250.0f};
        bottomPipe->r = 1.0f;
        bottomPipe->g = 1.0f;
        bottomPipe->b = 1.0f;
        bottomPipe->z = 0.0f;
        bottomPipe->origin = Radium::Nodes::CoordinateOrigin::Center;
        bottomPipe->flags = 0b00000000; // no flip
        bottomPipe->parent = pipeGroup;
        pipeGroup->children.push_back(bottomPipe);

        // Top pipe
        Radium::Nodes::Sprite2D *topPipe = new Radium::Nodes::Sprite2D();
        topPipe->batchTag = "flappyCenter";
        topPipe->sourceRect = {0, 624, 52, 320};
        topPipe->textureWidth = 1024;
        topPipe->textureHeight = 1024;
        topPipe->position = {0.0f, 250.0f};
        topPipe->r = 1.0f;
        topPipe->g = 1.0f;
        topPipe->b = 1.0f;
        topPipe->z = 0.0f;
        topPipe->origin = Radium::Nodes::CoordinateOrigin::Center;
        topPipe->flags = 0b00000010; // vertical flip
        topPipe->parent = pipeGroup;
        pipeGroup->children.push_back(topPipe);

        // Optional Lua script for controlling the whole pipe group
        Radium::Nodes::LuaScript *script = new Radium::Nodes::LuaScript("assets/scripts/pipe_group.lua");
        script->me = pipeGroup;
        pipeGroup->script = script;

        tree.nodes.push_back(pipeGroup);

        // Create a physics body for the bird
        Radium::Nodes::RigidBody2D *birdBody = new Radium::Nodes::RigidBody2D();
        birdBody->position = {0, 0};
        birdBody->width = 32.0f / 32.0f;   // Assuming 32 pixels = 1 unit
        birdBody->height = 24.0f / 32.0f;
        birdBody->density = 1.0f;
        birdBody->friction = 0.3f;
        birdBody->isStatic = false; // It's dynamic, it falls and flaps

        // Create the sprite and make it a child of the rigid body
        Radium::Nodes::Sprite2D *sprite = new Radium::Nodes::Sprite2D();
        sprite->batchTag = "flappyCenter";
        sprite->sourceRect = {178, 768, 32, 24};
        sprite->textureWidth = 1024;
        sprite->textureHeight = 1024;
        sprite->position = {0, 0}; // Local to the rigid body
        sprite->r = 1.0f;
        sprite->g = 1.0f;
        sprite->b = 1.0f;
        sprite->z = 0.0f;
        sprite->origin = Radium::Nodes::CoordinateOrigin::Center;

        // Set parent-child relationship
        sprite->parent = birdBody;
        birdBody->children.push_back(sprite);

        // Add to scene tree
        tree.nodes.push_back(birdBody);


        // tree.Deserialize("scene.json");

        tree.Serialize("scene.json");
        
    }

    void OnTick(float dt) override
    {
    }

    void OnRender() override
    {
    }

    void OnImgui() override
    {
        // ImGui::ShowDemoWindow();
    }
};

RADIUM_ENTRYPOINT(MyApp)