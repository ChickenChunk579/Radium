#pragma once
#include "imgui.h"
#include <Rune/Texture.hpp>
#ifndef IMGUI_DISABLE

/// @brief Initialize the Rune ImGui backend
/// @return Successful or not
bool ImGui_ImplRune_Init();
/// @brief Shutdown the Rune ImGui backend
void ImGui_ImplRune_Shutdown();
/// @brief Start a frame in the ImGui backend
void ImGui_ImplRune_NewFrame();
/**
 * @brief Render draw data from ImGui to Rune
 * 
 * @param draw_data DrawData from ImGui::Render()
 */
void ImGui_ImplRune_RenderDrawData(ImDrawData* draw_data);
/**
 * @brief Called by RenderDrawData, updates a texture from ImTextureData
 * 
 * Creates a Rune texture from imgui texture data.
 */
void ImGui_ImplRune_UpdateTexture(ImTextureData* tex);
/**
 * @brief Convert a Rune texture to a TextureID
 */
ImTextureID ImGui_ImplRune_TextureToID(Rune::Texture* texture);

#endif