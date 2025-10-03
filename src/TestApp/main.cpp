#include <Radium/Application.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Nodes/Node.hpp>
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


        Radium::Nodes::Node::Register();
        Radium::Nodes::Node2D::Register();
        Radium::Nodes::Sprite2D::Register();

               
        Radium::Nodes::Node2D* pipes = new Radium::Nodes::Node2D();

        for (int i = 0; i < 4; i++) {
            Radium::Nodes::Sprite2D* background = new Radium::Nodes::Sprite2D();
            background->batchTag = "flappy";
            background->sourceRect = {0, 0, 288, 512};
            background->textureWidth = 1024;
            background->textureHeight = 1024;
            background->position = {0, 0};
            background->r = 1.0f;
            background->g = 1.0f;
            background->b = 1.0f;
            background->z = -10.0f;
            background->origin = Radium::Nodes::CoordinateOrigin::TopLeft;
            background->position.x = i * 288;

            background->parent = pipes;
            pipes->children.push_back(background);
        }

        tree.nodes.push_back(pipes);

        Radium::Nodes::Sprite2D* sprite = new Radium::Nodes::Sprite2D();
        sprite->batchTag = "flappyCenter";
        sprite->sourceRect = {178, 768, 32, 24};
        sprite->textureWidth = 1024;
        sprite->textureHeight = 1024;
        sprite->position = {0, 0};
        sprite->r = 1.0f;
        sprite->g = 1.0f;
        sprite->b = 1.0f;
        sprite->z = -0.0f;
        sprite->origin = Radium::Nodes::CoordinateOrigin::Center;

        tree.nodes.push_back(sprite);
        

        //tree.Deserialize("scene.json");

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
        //ImGui::ShowDemoWindow();
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