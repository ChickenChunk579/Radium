#include <Radium/Application.hpp>
#include <Radium/DebugRenderer.hpp>
#include <Radium/Input.hpp>
#include <Nova/Nova.hpp>
#include <Rune/Rune.hpp>
#include <Flux/Flux.hpp>
#include <Iris/Iris.hpp>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "imgui.h"
#include "imgui_impl_rune.h"
#include "imgui_impl_nova.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
#include <Radium/SpriteBatchRegistry.hpp>
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#ifdef __ANDROID__

#endif

namespace Radium
{
	Application *currentApplication;

	Application::Application()
	{
#ifdef __ANDROID__

#endif

		Flux::Trace("Constructed application {}", this->GetTitle());
	}

	void Application::Run()
	{
		OnEarlyLoad();

		Flux::Trace("Initializing SDL");

		Iris::codecs.push_back(std::make_unique<Iris::PNGCodec>());
		Iris::codecs.push_back(std::make_unique<Iris::BMPCodec>());

		Flux::Trace("Initialized SDL successfuly");
		Flux::Trace("Creating window");

		Radium::Vector2i size = this->GetPreferredSize();

		bool wayland = false;
		void **handles = nullptr;

		window = new Nova::Window(GetTitle(), GetPreferredSize().x, GetPreferredSize().y);

		Flux::Trace("Created window");
		Flux::Trace("Getting window info...");

		Nova::X11PlatformData* platformData = (Nova::X11PlatformData*)window->platformData;

		Flux::Trace("Got window info.");
		Flux::Trace("Initializing Rune...");

		if (!Rune::Initialize(platformData->display, static_cast<uint32_t>(platformData->window), size.x, size.y, wayland))
		{
			Flux::Error("Rune initialization failed.");
			exit(1);
		}

		if (handles != nullptr)
		{
			delete handles;
		}

		Flux::Trace("Successfully initialized Rune.");
		Flux::Info("surface format: {}", static_cast<int>(Rune::caps.formats[0]));

#if (defined(__linux__) && !defined(__ANDROID__)) || defined(_MSC_VER)
		Flux::Trace("Setting up imgui...");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		Flux::Trace("Created context");

		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		Flux::Trace("Setting style");

		ImGui::StyleColorsDark();

		Flux::Trace("Creating renderer");

		ImGui_ImplNova_Init(window);
		ImGui_ImplRune_Init();

		Flux::Trace("Done!");

		ImGui::StyleColorsDark();
		ImGuiStyle &style = ImGui::GetStyle();
		style.Colors[ImGuiCol_TableRowBg] =
			ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // dark row
		style.Colors[ImGuiCol_TableRowBgAlt] =
			ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // alternate row

#endif

		Radium::DebugRenderer::Setup();

		Flux::Trace("Creating physics stuff");

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = {GetGravity().x, GetGravity().y};

		worldId = b2CreateWorld(&worldDef);

		Flux::Trace("Done");

		Flux::Trace("Running user OnLoad");
		this->OnLoad();
		tree.OnLoad();
		Flux::Trace("Done");
		Flux::Trace("Starting main loop...");

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
	}

	Radium::Vector2i Application::GetSize()
	{
		int w, h = 0;

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

			running = window->PollEvents();

			while (window->HasEvents()){
				Nova::Event* event = window->PopEvent();
				ImGui_ImplNova_ProcessEvent(event);
			}

			//Radium::Input::Update();
		}

		//Flux::Info("FPS: {:.1f}", ImGui::GetIO().Framerate);
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
