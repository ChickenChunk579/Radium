#include <Radium/Application.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/Components/Position.hpp>
#include <Radium/Components/Size.hpp>
#include <Radium/Components/Sprite.hpp>
#include <Radium/FontRenderer.hpp>
#include <Radium/Components/ClearColor.hpp>
#include <Rune/GeometryRenderer.hpp>

inline void RotationSystem(entt::registry& registry) {
    auto view = registry.view<Radium::Components::FullDrawSprite>();

    for (auto [entity, sprite] : view.each()) {
        sprite.rotation += 1;
    }
}

class MyApp : public Radium::Application {
public:
    Radium::FontBatch* batch;
    Rune::GeometryRenderer* geometryRenderer;


    std::string GetTitle() override {
        return "My Empty Radium App";
    }

    Radium::Vector2i GetSize() override {
        return {1280, 720};
    }

    void OnLoad() override {
        batch = new Radium::FontBatch("Roboto/static/Roboto-Black.ttf", 64);

        /*geometryRenderer = new Rune::GeometryRenderer();
        geometryRenderer->SetVertices({
            // x, y, r, g, b, u, v
            100, 100, 1, 0, 0, 0, 0,
            200, 100, 0, 1, 0, 1, 0,
            150, 200, 0, 0, 1, 1, 1
        });*/

        Radium::SpriteBatchRegistry::Add("logo", "Logo.png", Rune::SpriteOrigin::Center);
        {
            const auto logo = registry.create();
            registry.emplace<Radium::Components::Position>(logo, Radium::Vector2f(0, 0));
            registry.emplace<Radium::Components::Size>(logo, Radium::Vector2i(500, 500));
            registry.emplace<Radium::Components::FullDrawSprite>(logo, "logo", 1.0f, 1.0f, 1.0f, 0.0f);
            
        }
        {
            const auto clear = registry.create();
            registry.emplace<Radium::Components::ClearColor>(clear, 0.0f, 0.0f, 0.0f);
        }
    }

    void OnTick(float dt) override {
        // Do nothing
        RotationSystem(registry);
    }

    void OnRender() override {
        // Do nothing
        batch->Begin();

        batch->DrawString(100, 100, "The quick brown fox jumps over the lazy dog.");

        batch->End();
        
        //geometryRenderer->Draw();
    }
};

#ifndef __ANDROID__
RADIUM_ENTRYPOINT(MyApp)
#else
extern "C" int SDL_main(int argc, char* argv[]) {
    MyApp app;
    Radium::currentApplication = &app;
    app.Run();
    return 0;
}
#endif