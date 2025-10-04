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
            background->size = {228, 512};
            background->parent = pipes;
            pipes->children.push_back(background);
        }

        tree.nodes.push_back(pipes);





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
        bottomPipe->position = {0.0f, -225.0f};
        bottomPipe->r = 1.0f;
        bottomPipe->g = 1.0f;
        bottomPipe->b = 1.0f;
        bottomPipe->z = 0.0f;
        bottomPipe->origin = Radium::Nodes::CoordinateOrigin::Center;
        bottomPipe->flags = 0b00000000; // no flip
        bottomPipe->parent = pipeGroup;
        bottomPipe->size = {52, 320};
        pipeGroup->children.push_back(bottomPipe);

        // Top pipe
        Radium::Nodes::Sprite2D *topPipe = new Radium::Nodes::Sprite2D();
        topPipe->batchTag = "flappyCenter";
        topPipe->sourceRect = {0, 624, 52, 320};
        topPipe->textureWidth = 1024;
        topPipe->textureHeight = 1024;
        topPipe->position = {0.0f, 225.0f};
        topPipe->r = 1.0f;
        topPipe->g = 1.0f;
        topPipe->b = 1.0f;
        topPipe->z = 0.0f;
        topPipe->origin = Radium::Nodes::CoordinateOrigin::Center;
        topPipe->flags = 0b00000010; // vertical flip
        topPipe->parent = pipeGroup;
        topPipe->size = {52, 320};
        pipeGroup->children.push_back(topPipe);
        
        pipeGroup->position = {500, 0};

        Radium::Nodes::ChaiScript* script = new Radium::Nodes::ChaiScript("assets/scripts/pipe.chai");
        script->me = pipeGroup;
        pipeGroup->script = script;

        tree.nodes.push_back(pipeGroup);

        Radium::Nodes::Sprite2D* sprite = new Radium::Nodes::Sprite2D();
        sprite->batchTag = "flappyCenter";
        sprite->sourceRect = {178, 768, 32, 24};
        sprite->textureWidth = 1024;
        sprite->textureHeight = 1024;
        sprite->position = {-200, 0};
        sprite->r = 1.0f;
        sprite->g = 1.0f;
        sprite->b = 1.0f;
        sprite->z = -0.0f;
        sprite->size = {32, 24};
        sprite->origin = Radium::Nodes::CoordinateOrigin::Center;

        script = new Radium::Nodes::ChaiScript("assets/scripts/bird.chai");
        
        script->SetGlobal<Radium::Nodes::Sprite2D*>(bottomPipe, "bottomPipe");
        script->SetGlobal<Radium::Nodes::Sprite2D*>(topPipe, "topPipe");
        
        script->me = sprite;
        sprite->script = script;
        

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