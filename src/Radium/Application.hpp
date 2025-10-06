#pragma once

#include <iostream>
#include <Radium/Math.hpp>
#include <SDL2/SDL.h>
#include <Radium/Nodes/Tree.hpp>
#include <box2d/box2d.h>
#include <Radium/Nodes/Tree.hpp>

namespace Radium {
    class Application {
    public:
        b2WorldId worldId;

        Radium::Nodes::SceneTree tree{"MainScene"};

        virtual std::string GetTitle() {
            return "";
        };
        virtual Radium::Vector2i GetPreferredSize() = 0;
        virtual Radium::Vector2f GetGravity() = 0;

        virtual void OnLoad() {}
        virtual void OnTick(float dt) {}
        virtual void OnRender() {}
        virtual void OnImgui() {}
        virtual void OnPreRender() {}
        virtual void OnEarlyLoad() {}
        virtual void OnRelease() {}
        virtual void OnPreLoad(int argc, char* argv[]) {}

        void Run();

        Radium::Vector2i GetSize();

        void RunFrame(double time);

        Application();
    private:
        SDL_Window* window;
        bool running = true;
    };

    extern Application* currentApplication;

    #ifdef __EMSCRIPTEN__
    bool RunFrameWrapper(double time, void* userdata);
    #endif
};

#define RADIUM_ENTRYPOINT(appClass) int main(int argc, char* argv[]) { \
    appClass app; \
    app.OnPreLoad(argc, argv); \
    Radium::currentApplication = &app; \
    app.Run(); \
    return 0; \
}