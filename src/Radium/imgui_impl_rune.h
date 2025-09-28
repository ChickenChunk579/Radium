#pragma once
#include "imgui.h"
#ifndef IMGUI_DISABLE

bool ImGui_ImplRune_Init();
void ImGui_ImplRune_Shutdown();
void ImGui_ImplRune_NewFrame();
void ImGui_ImplRune_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRune_UpdateTexture(ImTextureData* tex);

#endif