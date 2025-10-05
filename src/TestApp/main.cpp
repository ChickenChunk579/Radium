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

class MyApp : public Radium::Application
{
public:
    Rune::Viewport* viewport;
    Rune::GeometryRenderer* geometry;

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

        tree.Deserialize("test.json");

        //tree.Serialize("scene.json");

        



        //viewport = new Rune::Viewport(128, 128);
        
        //Radium::SpriteBatchRegistry::Add("viewportDisplay", new Rune::Texture(viewport->textureView, Rune::SamplingMode::Linear), Rune::SpriteOrigin::TopLeft, Rune::SamplingMode::Nearest);

    }

    void OnTick(float dt) override
    {
        
    }

    void OnPreRender() override {
        //viewport->SetupFrame();
        //viewport->FinishFrame();
    }

    void OnRender() override
    {
        //Rune::SpriteBatch* batch = Radium::SpriteBatchRegistry::Get("viewportDisplay");

        //batch->Begin();
        //batch->DrawImageRect(0, 0, 100, 100, 1, 1, 1, 0, 0, 128, 128, 128, 128, 0);
        //batch->End();
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