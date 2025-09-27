#pragma once

#include <iostream>
#include <Radium/Math.hpp>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

namespace Radium {
    class Application {
    public:
        entt::registry registry;

        virtual std::string GetTitle() {
            return "";
        };
        virtual Radium::Vector2i GetSize() = 0;

        virtual void OnLoad() {}
        virtual void OnTick(float dt) {}
        virtual void OnRender() {}

        void Run();

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

#define RADIUM_ENTRYPOINT(appClass) int main() { \
    appClass app; \
    Radium::currentApplication = &app; \
    app.Run(); \
    return 0; \
}