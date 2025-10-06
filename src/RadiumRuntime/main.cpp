#include <Radium/Application.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/ChaiScript.hpp>
#include <Radium/FontRenderer.hpp>
#include <Radium/PhysicsUtil.hpp>
#include <Radium/Input.hpp>
#include <Rune/Texture.hpp>
#include <Rune/Viewport.hpp>
#include <Radium/AssetLoader.hpp>
#include <Rune/GeometryRenderer.hpp>
#include "imgui.h"
#include <spdlog/spdlog.h>
#include <Radium/Random.hpp>
#include <RadiumRuntime/GameDataParser.hpp>

class MyApp : public Radium::Application
{
public:
    Rune::Viewport* viewport;
    Rune::GeometryRenderer* geometry;
    std::string appBase = ".";
    GameConfig config;

    void OnPreLoad(int argc, char* argv[]) override {
        if (argc > 1) {
            appBase = std::string(argv[1]);
        }
        std::string appConfig = Radium::ReadFileToString(appBase + "/app.json");
        json j = json::parse(appConfig);
        config = j.get<GameConfig>();
    }

    std::string GetTitle() override
    {
        return config.title;
    }

    Radium::Vector2i GetPreferredSize() override
    {
        return {config.preferedSize[0], config.preferedSize[1]};
    }

    Radium::Vector2f GetGravity() override
    {
        return {config.gravity[0], config.gravity[1]};
    }


    void OnLoad() override
    {
        

        Radium::assetBase = appBase + "/";

        for (auto batchInfo : config.spriteBatches) {
            Radium::SpriteBatchRegistry::Add(batchInfo.tag, batchInfo.path, batchInfo.origin, Rune::SamplingMode::Nearest);
        }

        Radium::Nodes::Node::Register();
        Radium::Nodes::Node2D::Register();
        Radium::Nodes::Sprite2D::Register();
        Radium::Nodes::ClassDB::RegisterEnum<Radium::Nodes::CoordinateOrigin>();

        spdlog::info("Running app at {}", appBase);

        tree.Deserialize(config.initialScene);
    }

    void OnTick(float dt) override
    {
        
    }

    void OnPreRender() override {
    }

    void OnRender() override
    {
    }

    void OnImgui() override
    {
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