#include <Radium/Application.hpp>
#include "imgui.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <mutex>
#include <vector>
#include <string>
#include <memory>
#include <Radium/imgui_impl_rune.h>
#include <Rune/Viewport.hpp>
#include <Rune/Texture.hpp>

// Simple thread-safe console buffer used by the ImGui sink and UI
struct ImGuiConsole
{
    std::mutex mutex;
    std::vector<std::string> lines;
    bool autoScroll = true;

    void AddLine(const std::string &s)
    {
        std::lock_guard<std::mutex> lk(mutex);
        lines.emplace_back(s);
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lk(mutex);
        lines.clear();
    }

    std::vector<std::string> Snapshot()
    {
        std::lock_guard<std::mutex> lk(mutex);
        return lines;
    }
};

static ImGuiConsole g_imguiConsole;

// custom sink forwarding formatted messages to g_imguiConsole
class ImGuiSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
    ImGuiConsole *console;
    explicit ImGuiSink(ImGuiConsole *c) : console(c) {}

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::memory_buf_t formatted;
        base_sink<std::mutex>::formatter_->format(msg, formatted);
        std::string s(formatted.data(), formatted.size());
        if (console)
            console->AddLine(s);
    }

    void flush_() override {}
};

// credit to https://github.com/simongeilfus/Cinder-ImGui
void imguiTheme()
{
    ImGuiStyle &mStyle = ImGui::GetStyle();

    mStyle.WindowMinSize = ImVec2(160, 20);
    mStyle.FramePadding = ImVec2(4, 2);
    mStyle.ItemSpacing = ImVec2(6, 2);
    mStyle.ItemInnerSpacing = ImVec2(6, 4);
    mStyle.Alpha = 0.95f;
    mStyle.WindowRounding = 4.0f;
    mStyle.FrameRounding = 2.0f;
    mStyle.IndentSpacing = 6.0f;
    mStyle.ItemInnerSpacing = ImVec2(2, 4);
    mStyle.ColumnsMinSpacing = 50.0f;
    mStyle.GrabMinSize = 14.0f;
    mStyle.GrabRounding = 16.0f;
    mStyle.ScrollbarSize = 12.0f;
    mStyle.ScrollbarRounding = 16.0f;
    mStyle.WindowBorderSize = 1.5f;

    ImGuiStyle &style = mStyle;
    style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.65f, 0.20f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.92f, 0.18f, 0.29f, 0.50f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.92f, 0.18f, 0.29f, 1.0f); // Bright red outline, fully opaque

    ImGuiIO &io = ImGui::GetIO();

    ImFont *customFont = io.Fonts->AddFontFromFileTTF("assets/Roboto/static/Roboto-Light.ttf", 16.0f);

    if (customFont == nullptr)
    {
        spdlog::error("Failed to load font!");
    }

    io.FontDefault = customFont;
}

class Editor : public Radium::Application
{
public:
    Rune::Viewport *scene;
    ImTextureID sceneTexture;
    // Keep the sink alive for the lifetime of the application
    std::shared_ptr<spdlog::sinks::sink> imguiSink;

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
        imguiTheme();
        scene = new Rune::Viewport(640, 480);
        Rune::Texture *tex = new Rune::Texture(scene->textureView, Rune::SamplingMode::Nearest);

        sceneTexture = ImGui_ImplRune_TextureToID(tex);
    }

    void OnEarlyLoad() override
    {
        // attach the previously-declared ImGuiSink so spdlog messages are shown in the UI console
        auto sink = std::make_shared<ImGuiSink>(&g_imguiConsole);
        imguiSink = sink;
        if (spdlog::default_logger())
        {
            spdlog::default_logger()->sinks().push_back(sink);
        }
    }

    void OnTick(float dt) override
    {
    }

    void OnPreRender() override
    {
        scene->SetupFrame();
        scene->FinishFrame();
    }

    void OnRender() override
    {
    }

    void OnImgui() override
    {
        {
            ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGui::Begin("Radium Editor", NULL, window_flags);
            ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Radium"))
                {

                    if (ImGui::MenuItem("Close", NULL, false))
                        exit(0);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Inspector");

            ImGui::End();
        }
        {
            ImGui::Begin("Assets");

            ImGui::End();
        }
        {
            ImGui::Begin("Hierarchy");

            ImGui::End();
        }
        {
            ImGui::Begin("Console");

            // toolbar
            if (ImGui::Button("Clear"))
            {
                g_imguiConsole.Clear();
            }
            ImGui::SameLine();
            bool autoScroll = g_imguiConsole.autoScroll;
            if (ImGui::Checkbox("Auto-scroll", &autoScroll))
            {
                g_imguiConsole.autoScroll = autoScroll;
            }

            ImGui::Separator();

            ImGui::BeginChild("ConsoleScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
            auto snapshot = g_imguiConsole.Snapshot();
            for (const auto &line : snapshot)
            {
                ImGui::TextUnformatted(line.c_str());
            }
            if (g_imguiConsole.autoScroll)
            {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

            ImGui::End();
        }
        {
            ImGui::Begin("Viewport");

            // Get available content region inside the window
            ImVec2 avail = ImGui::GetContentRegionAvail();

            // Fallback in case viewport size is invalid
            int imgW = (scene && scene->width > 0) ? scene->width : 640;
            int imgH = (scene && scene->height > 0) ? scene->height : 480;

            float imageAspect = (float)imgW / (float)imgH;

            // Compute render size that fits the available region while keeping the aspect ratio
            ImVec2 renderSize;
            if (avail.y <= 0.0f || avail.x <= 0.0f)
            {
                renderSize = ImVec2((float)imgW, (float)imgH);
            }
            else
            {
                float availAspect = avail.x / avail.y;
                if (availAspect > imageAspect)
                {
                    // available region is wider than image -> fit by height
                    renderSize.y = avail.y;
                    renderSize.x = renderSize.y * imageAspect;
                }
                else
                {
                    // fit by width
                    renderSize.x = avail.x;
                    renderSize.y = renderSize.x / imageAspect;
                }
            }

            // Center the image inside the available region
            ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
            float offsetX = (avail.x - renderSize.x) * 0.5f;
            float offsetY = (avail.y - renderSize.y) * 0.5f;
            if (offsetX < 0.0f)
                offsetX = 0.0f;
            if (offsetY < 0.0f)
                offsetY = 0.0f;
            ImGui::SetCursorScreenPos(ImVec2(cursorScreenPos.x + offsetX, cursorScreenPos.y + offsetY));

            ImGui::Image(sceneTexture, renderSize);

            // Move cursor to the end of the content region so following widgets are laid out correctly
            ImGui::SetCursorScreenPos(ImVec2(cursorScreenPos.x + avail.x, cursorScreenPos.y + avail.y));

            ImGui::Dummy({0, 0});

            ImGui::End();
        }
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