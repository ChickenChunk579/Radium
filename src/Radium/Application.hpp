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

        double lastFrameTime = 0.0;

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

#if !defined(__ANDROID__) && !defined(TARGET_OS_IOS)
#define RADIUM_ENTRYPOINT(appClass) int main() { \
    appClass app; \
    Radium::currentApplication = &app; \
    app.Run(); \
    return 0; \
}
#else
#define RADIUM_ENTRYPOINT(appClass) extern "C" int SDL_main(int argc, char *argv[]) { \
    appClass app; \
    Radium::currentApplication = &app; \
    app.Run(); \
    return 0; \
}
#endif