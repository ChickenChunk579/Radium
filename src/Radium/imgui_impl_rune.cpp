#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_rune.h"
#include <stdint.h>
#include <vector>
#include <Rune/GeometryRenderer.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <unordered_set>
#undef Status

// texture pointer + whether this backend owns (allocated) the Texture instance
static std::unordered_map<ImTextureID, std::pair<Rune::Texture*, bool>> textures;

struct ImGui_ImplRune_Data
{
    Rune::GeometryRenderer *renderer;
};

static ImGui_ImplRune_Data *ImGui_ImplRune_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplRune_Data *)ImGui::GetIO().BackendRendererUserData : nullptr;
}

bool ImGui_ImplRune_Init()
{
    if (!ImGui::GetCurrentContext())
    {
        spdlog::error("ImGui context not created before calling ImGui_ImplRune_Init!");
        return false;
    }

    ImGuiIO &io = ImGui::GetIO();
    IMGUI_CHECKVERSION();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    spdlog::trace("No backend yet!");

    ImGui_ImplRune_Data *bd = IM_NEW(ImGui_ImplRune_Data)();
    memset(bd, 0, sizeof(ImGui_ImplRune_Data));
    spdlog::trace("Created backend data");
    bd->renderer = new Rune::GeometryRenderer(nullptr);

    spdlog::trace("Created renderer");

    io.BackendRendererUserData = (void *)bd;
    io.BackendRendererName = "imgui_impl_rune";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

    spdlog::trace("Set backend data");
    return true;
}

void ImGui_ImplRune_Shutdown()
{
    ImGui_ImplRune_Data *bd = ImGui_ImplRune_GetBackendData();
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");

    ImGuiIO &io = ImGui::GetIO();
    ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();

    delete bd->renderer;

    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasTextures);
    // platform_io.ClearPlatformHandlers();
    IM_DELETE(bd);


    for (auto &pair : textures) {
        Rune::Texture* t = pair.second.first;
        bool owned = pair.second.second;
        if (t) {
            if (owned) {
                t->Destroy();
                delete t;
            }
            // If not owned, don't delete — caller owns it
        }
    }
    textures.clear();
}

void ImGui_ImplRune_NewFrame()
{
    ImGui_ImplRune_Data *bd = ImGui_ImplRune_GetBackendData();
    IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplOpenGL2_Init()?");
    IM_UNUSED(bd);
}

void ImGui_ImplRune_SetupRenderState(ImDrawData *draw_data, int fb_width, int fb_height)
{
}

void ImGui_ImplRune_RenderDrawData(ImDrawData *draw_data)
{
    ImGui_ImplRune_Data *bd = ImGui_ImplRune_GetBackendData();
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;

    if (draw_data->Textures != nullptr)
        for (ImTextureData *tex : *draw_data->Textures)
            if (tex->Status != ImTextureStatus_OK)
                ImGui_ImplRune_UpdateTexture(tex);

    ImGui_ImplRune_SetupRenderState(draw_data, fb_width, fb_height);

    int drawCalls = 0;

    static std::unordered_set<uintptr_t> s_missingTexIDs;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        std::vector<float> vertices;
        const ImDrawList *draw_list = draw_data->CmdLists[n];
        const ImDrawVert *vtx_buffer = draw_list->VtxBuffer.Data;
        const ImDrawIdx *idx_buffer = draw_list->IdxBuffer.Data;
        int idx_count = draw_list->IdxBuffer.Size;
        for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd *pcmd = &draw_list->CmdBuffer[cmd_i];

            for (int i = 0; i < idx_count; i += 3)
            {
                // Get triangle indices
                ImDrawIdx idx0 = idx_buffer[i];
                ImDrawIdx idx1 = idx_buffer[i + 1];
                ImDrawIdx idx2 = idx_buffer[i + 2];

                const ImDrawVert &v0 = vtx_buffer[idx0];
                const ImDrawVert &v1 = vtx_buffer[idx1];
                const ImDrawVert &v2 = vtx_buffer[idx2];

                auto push_vert = [&](const ImDrawVert &v)
                {
                    // Position
                    vertices.push_back(v.pos.x);
                    vertices.push_back(v.pos.y);

                    // Color
                    float r = ((v.col >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f;
                    float g = ((v.col >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f;
                    float b = ((v.col >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f;
                    float a = ((v.col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f;
                    vertices.push_back(r);
                    vertices.push_back(g);
                    vertices.push_back(b);

                    // UV
                    vertices.push_back(v.uv.x);
                    vertices.push_back(v.uv.y);
                };

                push_vert(v0);
                push_vert(v1);
                push_vert(v2);
            }
            if (!pcmd)
            {
                continue;
            }
            ImTextureRef id = pcmd->TexRef;
            auto it = textures.find(pcmd->GetTexID());
            Rune::Texture* tx = nullptr;
            if (it != textures.end()) {
                tx = it->second.first;
            } else {
                // Fallback: perhaps the key representation differs; try to find by pointer equality
                Rune::Texture* candidate = reinterpret_cast<Rune::Texture*>(pcmd->GetTexID());
                for (const auto &pair : textures) {
                    if (pair.second.first == candidate) {
                        tx = pair.second.first;
                        break;
                    }
                }
            }

            if (!tx) {
                uintptr_t key = (uintptr_t)pcmd->GetTexID();
                if (s_missingTexIDs.find(key) == s_missingTexIDs.end()) {
                    s_missingTexIDs.insert(key);
                    spdlog::warn("ImGui_ImplRune: missing texture for id {}", key);
                }
            }
            bd->renderer->SetTexture(tx);
            bd->renderer->SetVertices(vertices);
            bd->renderer->Draw();
            drawCalls += 1;
        }
    }
}

void ImGui_ImplRune_UpdateTexture(ImTextureData *tex)
{
    if (tex->Status == ImTextureStatus_WantCreate)
    {
        IM_ASSERT(tex->TexID == 0 && tex->BackendUserData == nullptr);
        IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);
        void *pixels = tex->GetPixels();
        int w = tex->Width;
        int h = tex->Height;
    Rune::Texture *rtex = new Rune::Texture(w, h, tex->GetPitch(), pixels, Rune::SamplingMode::Linear);

    uintptr_t fakeTexID = reinterpret_cast<uintptr_t>(rtex);
    tex->SetTexID((ImTextureID)fakeTexID);
    textures[fakeTexID] = std::make_pair(rtex, true); // we own textures we create here
    }
    else if (tex->Status == ImTextureStatus_WantUpdates)
    {
        // Destroy and delete previous texture instance if we own it
        auto itOld = textures.find(tex->TexID);
        if (itOld != textures.end()) {
            if (itOld->second.second && itOld->second.first) {
                itOld->second.first->Destroy();
                delete itOld->second.first;
            }
            textures.erase(itOld);
        }

        IM_ASSERT(tex->TexID == 0 && tex->BackendUserData == nullptr);
        IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);
        void *pixels = tex->GetPixels();
        int w = tex->Width;
        int h = tex->Height;
        Rune::Texture *rtex = new Rune::Texture(w, h, tex->GetPitch(), pixels, Rune::SamplingMode::Linear);

        ImTextureID id = tex->TexID;

        textures[id] = std::make_pair(rtex, true);
    }
    else if (tex->Status == ImTextureStatus_WantDestroy)
    {
        auto it = textures.find(tex->TexID);
        if (it != textures.end()) {
            if (it->second.second && it->second.first) {
                it->second.first->Destroy();
                delete it->second.first;
            }
            textures.erase(it);
        }

        tex->SetTexID(ImTextureID_Invalid);
        tex->SetStatus(ImTextureStatus_Destroyed);
    }
}

ImTextureID ImGui_ImplRune_TextureToID(Rune::Texture* texture)
{
    // Search for existing texture ID
    for (const auto& pair : textures)
    {
        if (pair.second.first == texture)
            return pair.first;
    }

    // Create a new ID by casting the pointer to ImTextureID
    ImTextureID id = reinterpret_cast<ImTextureID>(texture);

    // Store in the map but mark as not owned by the backend (caller owns it)
    textures[id] = std::make_pair(texture, false);

    return id;
}


#endif