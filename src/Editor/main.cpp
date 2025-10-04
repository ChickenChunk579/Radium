#include <Radium/Application.hpp>
#include "imgui.h"
#include <spdlog/spdlog.h>

class Editor : public Radium::Application
{
public:
    std::string GetTitle() override
    {
        return "Radium Editor";
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
        ImGui::ShowDemoWindow();
    }
};
#ifndef __ANDROID__
RADIUM_ENTRYPOINT(Editor)
#else
extern "C" int SDL_main(int argc, char *argv[])
{
    MyApp app;
    Radium::currentApplication = &app;
    app.Run();
    return 0;
}
#endif