#include <Radium/Application.hpp>
#include <Radium/DebugRenderer.hpp>
#include <Radium/Input.hpp>
#include <Rune/Rune.hpp>
#include <spdlog/spdlog.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "imgui_impl_rune.h"
#include <SDL2/SDL_syswm.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
#include <Radium/SpriteBatchRegistry.hpp>
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#ifdef __ANDROID__
#include <android/log.h>
#include <mutex>
#include <spdlog/sinks/base_sink.h>

template <typename Mutex>
class android_sink : public spdlog::sinks::base_sink<Mutex>
{
public:
	explicit android_sink(const std::string &tag = "spdlog") : tag_(tag) {}

protected:
	void sink_it_(const spdlog::details::log_msg &msg) override
	{
		// Format the message
		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
		// Map SPDLOG levels to Android levels
		android_LogPriority priority = ANDROID_LOG_DEFAULT;
		switch (msg.level)
		{
		case spdlog::level::trace:
			priority = ANDROID_LOG_VERBOSE;
			break;
		case spdlog::level::debug:
			priority = ANDROID_LOG_DEBUG;
			break;
		case spdlog::level::info:
			priority = ANDROID_LOG_INFO;
			break;
		case spdlog::level::warn:
			priority = ANDROID_LOG_WARN;
			break;
		case spdlog::level::err:
			priority = ANDROID_LOG_ERROR;
			break;
		case spdlog::level::critical:
			priority = ANDROID_LOG_FATAL;
			break;
		default:
			priority = ANDROID_LOG_UNKNOWN;
			break;
		}

		// Send to Android logcat
		__android_log_print(priority, tag_.c_str(), "%s",
							fmt::to_string(formatted).c_str());
	}

	void flush_() override {} // Nothing to flush

private:
	std::string tag_;
};

#endif

#include <SDL_image.h>

const char *SDL_SYSWMTypeToString(SDL_SYSWM_TYPE type)
{
	switch (type)
	{
	case SDL_SYSWM_UNKNOWN:
		return "UNKNOWN";
	case SDL_SYSWM_WINDOWS:
		return "WINDOWS";
	case SDL_SYSWM_X11:
		return "X11";
	case SDL_SYSWM_DIRECTFB:
		return "DIRECTFB";
	case SDL_SYSWM_COCOA:
		return "COCOA";
	case SDL_SYSWM_UIKIT:
		return "UIKIT";
	case SDL_SYSWM_WAYLAND:
		return "WAYLAND";
	case SDL_SYSWM_MIR:
		return "MIR";
	case SDL_SYSWM_WINRT:
		return "WINRT";
	case SDL_SYSWM_ANDROID:
		return "ANDROID";
	case SDL_SYSWM_VIVANTE:
		return "VIVANTE";
	case SDL_SYSWM_OS2:
		return "OS2";
	case SDL_SYSWM_HAIKU:
		return "HAIKU";
	case SDL_SYSWM_KMSDRM:
		return "KMSDRM";
	case SDL_SYSWM_RISCOS:
		return "RISCOS";
	default:
		return "UNKNOWN";
	}
}

namespace Radium
{
	Application *currentApplication;

	Application::Application()
	{
		spdlog::set_level(spdlog::level::trace);
#ifdef __ANDROID__

		auto sink = std::make_shared<android_sink<std::mutex>>("MyNativeApp");
		auto logger = std::make_shared<spdlog::logger>("android", sink);
		spdlog::set_default_logger(logger);
		spdlog::set_level(spdlog::level::trace); // Optional

#endif

		spdlog::trace("Constructed application {}", this->GetTitle());
	}

	void Application::Run()
	{
		OnEarlyLoad();

		spdlog::trace("Initializing SDL");
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
		{
			spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
			SDL_Quit();
			exit(1);
		}

		IMG_Init(IMG_INIT_PNG);
		spdlog::trace("Initialized SDL successfuly");
		spdlog::trace("Creating window");

		Radium::Vector2i size = this->GetPreferredSize();

		bool wayland = false;
		void **handles = nullptr;

		Uint32 flags = SDL_WINDOW_SHOWN;
#ifdef __ANDROID__
		flags |= SDL_WINDOW_VULKAN;
#endif

		this->window =
			SDL_CreateWindow(this->GetTitle().c_str(), SDL_WINDOWPOS_CENTERED,
							 SDL_WINDOWPOS_CENTERED, size.x, size.y, flags);
		if (!window)
		{
			spdlog::error("Failed to create window: {}", SDL_GetError());
			exit(1);
		}
		spdlog::trace("Created window");
		spdlog::trace("Getting window info...");
#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
#if defined(__linux__)
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if (!SDL_GetWindowWMInfo(window, &wmInfo))
		{
			spdlog::error("Failed to get window info");
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(1);
		}

		Display *x11Display = nullptr;
		uint32_t x11Window = 0;

		spdlog::info("WM subsystem: {}", SDL_SYSWMTypeToString(wmInfo.subsystem));
		if (wmInfo.subsystem == SDL_SYSWM_X11)
		{
			x11Display = wmInfo.info.x11.display;
			x11Window = static_cast<uint32_t>(wmInfo.info.x11.window);
		}
		if (wmInfo.subsystem == SDL_SYSWM_WAYLAND)
		{
			handles = new void *[2];

			handles[0] = (void *)wmInfo.info.wl.display;
			handles[1] = (void *)wmInfo.info.wl.surface;
			x11Display = (Display *)handles;
			wayland = true;
		}

		spdlog::info("Display: {}, Window: {}", static_cast<void *>(x11Display),
					 x11Window);
#else

#if !defined(TARGET_OS_IOS) && !defined(_MSC_VER)
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if (!SDL_GetWindowWMInfo(window, &wmInfo) ||
			wmInfo.subsystem != SDL_SYSWM_COCOA)
		{
			spdlog::error("Failed to get MacOS window info");
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(1);
		}

		Display *x11Display = wmInfo.info.cocoa.window;
		uint32_t x11Window = 0;

#else
#ifdef TARGET_OS_IOS
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if (!SDL_GetWindowWMInfo(window, &wmInfo) ||
			wmInfo.subsystem != SDL_SYSWM_UIKIT)
		{
			spdlog::error("Failed to get UIKit window info");
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(1);
		}

		Display *x11Display = wmInfo.info.uikit.window;
		uint32_t x11Window = 0;
#else
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if (!SDL_GetWindowWMInfo(window, &wmInfo) ||
			wmInfo.subsystem != SDL_SYSWM_WINDOWS)
		{
			spdlog::error("Failed to get Windows window info");
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(1);
		}

		Display *x11Display = nullptr;
		uint32_t x11Window = 0;

		handles = new void *[2];

		handles[0] = (void *)wmInfo.info.win.window;
		handles[1] = (void *)wmInfo.info.win.hinstance;
		x11Display = (Display *)handles;

#endif

#endif

#endif

#else
#ifndef __ANDROID__
		Display *x11Display = nullptr;
		uint32_t x11Window = 0;
#else

		uint32_t x11Window = 0;
		Display *x11Display = nullptr;

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if (!SDL_GetWindowWMInfo(window, &wmInfo) ||
			wmInfo.subsystem != SDL_SYSWM_ANDROID)
		{
			spdlog::error("Failed to get wm info");
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(1);
		}
		x11Display = wmInfo.info.android.window;

		int w, h = 0;

		SDL_GetWindowSizeInPixels(window, &w, &h);
		size.x = w;
		size.y = h;

#endif
#endif

		spdlog::trace("Got window info.");
		spdlog::trace("Initializing Rune...");

		if (!Rune::Initialize(x11Display, x11Window, size.x, size.y, wayland))
		{
			spdlog::error("Rune initialization failed.");
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(1);
		}

		if (handles != nullptr)
		{
			delete handles;
		}

		spdlog::trace("Successfully initialized Rune.");
		spdlog::info("surface format: {}", static_cast<int>(Rune::caps.formats[0]));

#if (defined(__linux__) && !defined(__ANDROID__)) || defined(_MSC_VER)
		spdlog::trace("Setting up imgui...");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		spdlog::trace("Created context");

		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		spdlog::trace("Setting style");

		ImGui::StyleColorsDark();

		spdlog::trace("Creating renderer");

		ImGui_ImplRune_Init();
		ImGui_ImplSDL2_InitForOther(window);

		spdlog::trace("Done!");

		ImGui::StyleColorsDark();
		ImGuiStyle &style = ImGui::GetStyle();
		style.Colors[ImGuiCol_TableRowBg] =
			ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // dark row
		style.Colors[ImGuiCol_TableRowBgAlt] =
			ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // alternate row

#endif

		Radium::DebugRenderer::Setup();

		spdlog::trace("Creating physics stuff");

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = {GetGravity().x, GetGravity().y};

		worldId = b2CreateWorld(&worldDef);

		spdlog::trace("Done");

		spdlog::trace("Running user OnLoad");
		this->OnLoad();
		tree.OnLoad();
		spdlog::trace("Done");
		spdlog::trace("Starting main loop...");

#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop([]()
								 { Radium::currentApplication->RunFrame(0); }, 0,
								 1);

#else
		this->running = true;

		while (running)
		{
			RunFrame(0.0);
			// std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
#endif

		OnRelease();
		SpriteBatchRegistry::Clear();
		ImGui_ImplRune_Shutdown();
		IMG_Quit();
		SDL_Quit();
	}

	Radium::Vector2i Application::GetSize()
	{
		int w, h = 0;
		SDL_GetWindowSizeInPixels(window, &w, &h);

		return {w, h};
	}

	Radium::Camera* Application::GetCamera()
	{
		return &camera;
	}

	void Application::RunFrame(double time)
	{
		ZoneScoped;

		{
			ZoneScopedN("Event Poll");
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
#if (defined(__linux__) && !defined(__ANDROID__)) || defined(_MSC_VER)
				ImGui_ImplSDL2_ProcessEvent(&event);
#endif
				if (event.type == SDL_QUIT)
					this->running = false;
			}

			Radium::Input::Update();
		}

		//spdlog::info("FPS: {:.1f}", ImGui::GetIO().Framerate);
		{
			ZoneScopedN("Physics Tick");
			b2World_Step(worldId, 1.0f / 60.0f, 4);
		}
		{
			ZoneScopedN("User Tick");
			this->OnTick(0);
			tree.OnTick(0);
		}

		{
			Rune::Clear(1.0f, 0.0f, 0.0f, 1.0f);
			/*
			auto view = registry.view<
				Radium::Components::ClearColor
			>();
			for (auto [entity, clear] : view.each()) {
				Rune::Clear(clear.r, clear.g, clear.b, 1);
			}
			*/
		}
		{
			Rune::PreSetupFrame();
			ZoneScopedN("User Pre-Render");
			this->OnPreRender();
		}
		{
			ZoneScopedN("Frame Setup");

			Rune::SetupFrame();
		}
		{
			ZoneScopedN("Drawing Sprites");
			tree.OnRender();

			auto batches = Radium::SpriteBatchRegistry::GetAll();
			for (auto batch : batches)
			{
				if (!batch)
				{
					continue;
				}
				if (batch->started)
				{
					batch->End();
				}
			}
		}
		{
			ZoneScopedN("User Render");
			// this->OnRender();
		}
#if (defined(__linux__) && !defined(__ANDROID__)) || defined(_MSC_VER)

		{
			ZoneScopedN("ImGui setup");
			ImGui_ImplRune_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();
		}
		{
			ZoneScopedN("ImGui windows");

			this->OnImgui();
			tree.OnImgui();
		}

		{
			ZoneScopedN("ImGui Render");
			ImGui::Render();

			ImGui_ImplRune_RenderDrawData(ImGui::GetDrawData());
		}
#endif
		Radium::DebugRenderer::Draw();
		Input::LateUpdate();
		{
			ZoneScopedN("Finish frame");
			Rune::FinishFrame();
		}
	}

#ifdef __EMSCRIPTEN__
	bool RunFrameWrapper(double time, void *userdata)
	{
		Radium::currentApplication->RunFrame(time);
		return true;
	}
#endif
}; // namespace Radium
