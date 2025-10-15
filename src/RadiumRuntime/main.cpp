#include <Radium/Application.hpp>
#include <Radium/Camera.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/2D/TileMap2D.hpp>
#include <Radium/Nodes/2D/RigidBody.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/PhysicsUtil.hpp>
#include <Radium/Input.hpp>
#include <Rune/Texture.hpp>
#include <Rune/Viewport.hpp>
#include <Radium/AssetLoader.hpp>
#include <Rune/GeometryRenderer.hpp>
#include "imgui.h"
#include <Flux/Flux.hpp>
#include <Radium/Random.hpp>
#include <Radium/GameDataParser.hpp>

class MyApp : public Radium::Application
{
public:
    Rune::Viewport* viewport;
    Rune::GeometryRenderer* geometry;
    std::string appBase = "";
    Radium::GameConfig config;

    void OnPreLoad(int argc, char* argv[]) override {
        if (argc > 1) {
            appBase = std::string(argv[1]);
        }
        std::string configPath = appBase + "/app.json";
        if (appBase == "") {
            configPath = "app.json";
        }
        std::string appConfig = Radium::ReadFileToString(configPath);
        Flux::Info("App config file: {}", appConfig);
        json j = json::parse(appConfig);
        config = j.get<Radium::GameConfig>();
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
        return {(float)config.gravity[0], (float)config.gravity[1]};
    }


    void OnLoad() override
    {
        

        #ifndef __ANDROID__
        Radium::assetBase = appBase + "/";
        #endif

        for (auto batchInfo : config.spriteBatches) {
            Flux::Info("Add batch {} from {}", batchInfo.tag, batchInfo.path);
            Radium::SpriteBatchRegistry::Add(batchInfo.tag, batchInfo.path, batchInfo.origin, Rune::SamplingMode::Nearest);
        }

        Radium::Vector2f::Register();
        Radium::Vector2i::Register();
        Radium::RectangleF::Register();
        Radium::Nodes::SceneTree::Register();
        Radium::Nodes::Node::Register();
        Radium::Nodes::Node2D::Register();
        Radium::Nodes::Sprite2D::Register();
        Radium::Nodes::TileMap2D::Register();
        Radium::Nodes::RigidBody::Register();
        Radium::Camera::Register();
        Radium::Nodes::ClassDB::RegisterEnum<Radium::Nodes::CoordinateOrigin>();
        Radium::Nodes::ClassDB::RegisterEnum<Radium::Nodes::BodyType>();
        Radium::Nodes::ClassDB::RegisterEnum<Radium::Nodes::CollisionType>();
        
        Flux::Info("Running app at {}", appBase);

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
    app.OnPreLoad(argc, argv);
    app.Run();
    return 0;
}
#endif