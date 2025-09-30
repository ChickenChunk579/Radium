#include <Radium/Application.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
//#include <Radium/Components/Position.hpp>
//#include <Radium/Components/Size.hpp>
//#include <Radium/Components/RigidBody.hpp>
//#include <Radium/Components/StaticBody.hpp>
//#include <Radium/Components/Sprite.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/FontRenderer.hpp>
#include <Radium/PhysicsUtil.hpp>
#include <Radium/Input.hpp>
#include <Radium/AssetLoader.hpp>
//#include <Radium/Components/ClearColor.hpp>
#include <Rune/GeometryRenderer.hpp>
#include "imgui.h"
#include <spdlog/spdlog.h>
#include <TestApp/json.hpp>
#include <Radium/Random.hpp>

using json = nlohmann::json;
/*
struct PipePair
{
    entt::entity topPipe;
    entt::entity bottomPipe;
};

std::vector<PipePair> pipePairs;

struct BirdTag
{
    int null;
};

struct PipeTag
{
    bool top;
};

inline void JumpSystem(entt::registry &registry)
{
    auto view = registry.view<Radium::Components::RigidBody, BirdTag>();

    for (auto [entity, rb, _] : view.each())
    {

        if (Radium::Input::WasKeyPressedThisFrame(Key::SPACE))
        {
            Radium::Physics::SetVelocity(rb, 0, -Radium::currentApplication->GetGravity().y);
            spdlog::info("Pressed this frame");
        }
    }
}

const float pipeSpeed = 2.5f;

inline void PipeMoveSystem(entt::registry& registry, std::vector<PipePair>& pipePairs)
{
    constexpr float pipeSpeed = 2.5f;
    constexpr int pipeGap = 200;
    constexpr int screenHeight = 480;
    constexpr int verticalMargin = 40;

    const int minCenterY = pipeGap / 2 + verticalMargin;
    const int maxCenterY = screenHeight - pipeGap / 2 - verticalMargin;

    for (auto& pair : pipePairs)
    {
        auto& topPos = registry.get<Radium::Components::Position>(pair.topPipe);
        auto& botPos = registry.get<Radium::Components::Position>(pair.bottomPipe);

        topPos.position.x -= pipeSpeed;
        botPos.position.x -= pipeSpeed;

        // Reset if out of bounds
        if (topPos.position.x < -740)
        {
            float newX = 1280.0f;
            float newGapCenterY = static_cast<float>(Radium::Random::IntBetween(minCenterY, maxCenterY));

            // Get sizes to re-position vertically
            auto& topSize = registry.get<Radium::Components::Size>(pair.topPipe);
            auto& botSize = registry.get<Radium::Components::Size>(pair.bottomPipe);

            float halfTopH = topSize.size.y / 2.0f;
            float halfBotH = botSize.size.y / 2.0f;

            // Reset positions
            topPos.position = Radium::Vector2f(newX, newGapCenterY + pipeGap / 2.0f + halfTopH);
            botPos.position = Radium::Vector2f(newX, newGapCenterY - pipeGap / 2.0f - halfBotH);
        
            Radium::Physics::StaticBodyTeleport(pair.topPipe, topPos.position.x, topPos.position.y);
            Radium::Physics::StaticBodyTeleport(pair.bottomPipe, botPos.position.x, botPos.position.y);
        }
    }
}
*/

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

    /*Radium::Components::CustomDrawSprite CreateCustomDrawFromJSON(std::string frame, json j, std::string batch, Radium::Components::CoordinateOrigin origin, float z = 0)
    {

        auto frameData = j["frames"][frame]["frame"];
        uint x = frameData["x"];
        uint y = frameData["y"];
        uint w = frameData["w"];
        uint h = frameData["h"];

        Radium::Components::CustomDrawSprite sprite = {
            .batchTag = batch,
            .r = 1.0f,
            .g = 1.0f,
            .b = 1.0f,
            .sx = x,
            .sy = y,
            .sw = w,
            .sh = h,
            .texWidth = 1024,
            .texHeight = 1024,
            .rotation = 0.0f,
            .coordinateOrigin = origin,
            .z = z,
            .flags = 0};

        return sprite;
    }*/

    void OnLoad() override
    {
        Radium::SpriteBatchRegistry::Add("flappy", "texture.png", Rune::SpriteOrigin::TopLeft, Rune::SamplingMode::Nearest);
        Radium::SpriteBatchRegistry::Add("flappyCenter", "texture.png", Rune::SpriteOrigin::Center, Rune::SamplingMode::Nearest);
        /*{
            const auto clear = registry.create();
            registry.emplace<Radium::Components::ClearColor>(clear, 0.0f, 0.0f, 0.0f);
        }
        std::string content = Radium::ReadFileToString("texture.json");

        if (content.empty())
        {
            spdlog::info("Failed to read JSON file");
            exit(0);
        }

        json j = json::parse(content);

        for (int i = 0; i < 5; i++)
        {
            auto sprite = CreateCustomDrawFromJSON("background-day.png", j, "flappy", Radium::Components::CoordinateOrigin::TopLeft, -10);
            const auto bg = registry.create();
            registry.emplace<Radium::Components::Position>(bg, Radium::Vector2f(i * sprite.sw, 0));
            registry.emplace<Radium::Components::Size>(bg, Radium::Vector2i(sprite.sw, sprite.sh));
            registry.emplace<Radium::Components::CustomDrawSprite>(bg, sprite);
        }

        {
            auto sprite = CreateCustomDrawFromJSON("yellowbird-midflap.png", j, "flappyCenter", Radium::Components::CoordinateOrigin::Center, -20);
            const auto birdEntity = registry.create();
            registry.emplace<Radium::Components::Position>(birdEntity, Radium::Vector2f(0, 0));
            registry.emplace<Radium::Components::Size>(birdEntity, Radium::Vector2i(sprite.sw, sprite.sh));
            registry.emplace<Radium::Components::CustomDrawSprite>(birdEntity, sprite);
            registry.emplace<Radium::Components::RigidBody>(birdEntity);
            registry.emplace<BirdTag>(birdEntity, 1);
        }

        const int pipePairCount = 5;
        const int pipeGap = 200; // 80px vertical gap between top and bottom pipe
        const int pipeOffsetX = 500;
        const float minCenterY = pipeGap / 2.0f + 50;       // 90
        const float maxCenterY = 480 - pipeGap / 2.0f - 50; // 390

        pipePairs.reserve(pipePairCount);

        for (int i = 0; i < pipePairCount; ++i)
        {
            float baseX = 640 + i * pipeOffsetX;
            float gapCenterY = static_cast<float>(Radium::Random::IntBetween(minCenterY, maxCenterY));

            auto sprite = CreateCustomDrawFromJSON("pipe-green.png", j, "flappy", Radium::Components::CoordinateOrigin::Center, -15);
            Radium::Vector2i pipeSize(sprite.sw, sprite.sh);
            float halfPipeHeight = pipeSize.y / 2.0f;

            // Bottom pipe
            entt::entity bottomPipe = registry.create();
            sprite.flags = 0b00000001;
            registry.emplace<Radium::Components::Position>(bottomPipe, Radium::Vector2f(baseX, gapCenterY - pipeGap / 2.0f - halfPipeHeight));
            registry.emplace<Radium::Components::Size>(bottomPipe, pipeSize);
            registry.emplace<Radium::Components::CustomDrawSprite>(bottomPipe, sprite);
            registry.emplace<PipeTag>(bottomPipe, false);
            registry.emplace<Radium::Components::StaticBody>(bottomPipe);

            // Top pipe
            entt::entity topPipe = registry.create();
            sprite.flags = 0b00000011;
            registry.emplace<Radium::Components::Position>(topPipe, Radium::Vector2f(baseX, gapCenterY + pipeGap / 2.0f + halfPipeHeight));
            registry.emplace<Radium::Components::Size>(topPipe, pipeSize);
            registry.emplace<Radium::Components::CustomDrawSprite>(topPipe, sprite);
            registry.emplace<PipeTag>(topPipe, true);
            registry.emplace<Radium::Components::StaticBody>(topPipe);

            // Save the pair
            pipePairs.push_back({topPipe, bottomPipe});
        }*/


        Radium::Nodes::Node::Register();
        Radium::Nodes::Node2D::Register();
        Radium::Nodes::Sprite2D::Register();

        Radium::Nodes::Sprite2D* sprite = new Radium::Nodes::Sprite2D();
        sprite->batchTag = "flappy";
        sprite->sourceRect = {178, 768, 32, 24};
        sprite->textureWidth = 1024;
        sprite->textureHeight = 1024;
        sprite->position = {200, 200};
        sprite->r = 1.0f;
        sprite->g = 1.0f;
        sprite->b = 1.0f;
        sprite->z = -5.0f;

        tree.nodes.push_back(sprite);
    }

    void OnTick(float dt) override
    {
        // Do nothing
        //JumpSystem(registry);
        //PipeMoveSystem(registry, pipePairs);
    }

    void OnRender() override
    {
    }

    void OnImgui() override
    {
        ImGui::ShowDemoWindow();
    }
};

#ifndef __ANDROID__
RADIUM_ENTRYPOINT(MyApp)
#else
extern "C" int SDL_main(int argc, char *argv[])
{
    MyApp app;
    Radium::currentApplication = &app;
    app.Run();
    return 0;
}
#endif