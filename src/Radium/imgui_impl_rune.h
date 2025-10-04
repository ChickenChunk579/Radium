#pragma once
#include "imgui.h"
#include <Rune/Texture.hpp>
#ifndef IMGUI_DISABLE

bool ImGui_ImplRune_Init();
void ImGui_ImplRune_Shutdown();
void ImGui_ImplRune_NewFrame();
void ImGui_ImplRune_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRune_UpdateTexture(ImTextureData* tex);
ImTextureID ImGui_ImplRune_TextureToID(Rune::Texture* texture);

#endif