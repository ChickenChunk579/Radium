#pragma once

#include <iostream>
#include <Radium/Math.hpp>
#include <SDL2/SDL.h>
#include <Radium/Nodes/Tree.hpp>
#include <box2d/box2d.h>
#include <Radium/Nodes/Tree.hpp>
#include <Radium/Camera.hpp>

/**
 * @brief Defines the application entry point.
 * 
 * This macro defines the `main()` function for a Radium application.
 * It instantiates the application class, sets it as the current application,
 * and runs its main loop.
 * 
 * @param appClass The name of the class that implements the application, derived from Radium::Application.
 */
#define RADIUM_ENTRYPOINT(appClass) int main(int argc, char* argv[]) { \
    appClass app; \
    app.OnPreLoad(argc, argv); \
    Radium::currentApplication = &app; \
    app.Run(); \
    return 0; \
}

namespace Radium {

    /**
     * @brief Base class for Radium applications.
     * 
     * Inherit from this class to define the behavior of your application.
     * Override the virtual methods to customize application lifecycle events such as loading,
     * ticking, rendering, and cleanup.
     */
    class Application {
    public:
        /** @brief The physics world ID used for Box2D simulations. */
        b2WorldId worldId;

        /** @brief The root scene tree node of the application. */
        Radium::Nodes::SceneTree tree{"MainScene"};

        /** @brief The main camera for the application. */
        Radium::Camera camera;

        /**
         * @brief Returns the window title of the application.
         * @return A string containing the window title.
         */
        virtual std::string GetTitle() {
            return "";
        };

        /**
         * @brief Returns the preferred window size of the application.
         * @return A Vector2i containing the width and height in pixels.
         */
        virtual Radium::Vector2i GetPreferredSize() = 0;

        /**
         * @brief Returns the gravity vector used in the physics simulation.
         * @return A Vector2f representing the gravity (e.g. {0, -9.8}).
         */
        virtual Radium::Vector2f GetGravity() = 0;

        /**
         * @brief Called after SDL and the engine have been initialized.
         * Override to perform any additional loading or setup.
         */
        virtual void OnLoad() {}

        /**
         * @brief Called once per frame with the time elapsed since the last frame.
         * 
         * @param dt Time delta in seconds.
         */
        virtual void OnTick(float dt) {}

        /**
         * @brief Called once per frame to render the scene.
         */
        virtual void OnRender() {}

        /**
         * @brief Called to render ImGui user interfaces.
         */
        virtual void OnImgui() {}

        /**
         * @brief Called before rendering begins for the current frame.
         */
        virtual void OnPreRender() {}

        /**
         * @brief Called before the application starts loading. Ideal for early setup.
         */
        virtual void OnEarlyLoad() {}

        /**
         * @brief Called when the application is closing to release resources.
         */
        virtual void OnRelease() {}

        /**
         * @brief Called before Run() starts. You can access command-line arguments here.
         * 
         * @param argc Argument count.
         * @param argv Argument values.
         */
        virtual void OnPreLoad(int argc, char* argv[]) {}

        /**
         * @brief Starts the main application loop.
         * 
         * Initializes SDL, sets up the window and rendering, then enters the main loop.
         */
        void Run();

        /**
         * @brief Returns the current window size.
         * @return A Vector2i representing the width and height of the window.
         */
        Radium::Vector2i GetSize();

        /**
         * @brief Gets a reference to the main camera.
         * @return A reference to the camera object.
         */
        Radium::Camera* GetCamera();

        /**
         * @brief Processes and renders a single frame of the application.
         * 
         * @param time The current time (used for animations and delta time).
         */
        void RunFrame(double time);

        /**
         * @brief Constructs a new Application object.
         * Initializes internal state.
         */
        Application();

    private:
        /** @brief Pointer to the SDL window. */
        SDL_Window* window;

        /** @brief Indicates whether the main loop is still running. */
        bool running = true;
    };

    /** @brief Pointer to the currently running Radium application. */
    extern Application* currentApplication;

    #ifdef __EMSCRIPTEN__

    /**
     * @brief Wrapper function for Emscripten's main loop.
     * 
     * @param time The current time in milliseconds.
     * @param userdata Pointer to application-specific data.
     * @return Always true; used to continue the loop.
     */
    bool RunFrameWrapper(double time, void* userdata);

    #endif
};
