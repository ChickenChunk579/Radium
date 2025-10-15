#pragma once
#include "imgui.h"      // IMGUI_IMPL_API
#include <Nova/Nova.hpp>
#ifndef IMGUI_DISABLE

IMGUI_IMPL_API bool     ImGui_ImplNova_Init(Nova::Window* window);
IMGUI_IMPL_API void     ImGui_ImplNova_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplNova_ProcessEvent(Nova::Event* event);

#endif // #ifndef IMGUI_DISABLE