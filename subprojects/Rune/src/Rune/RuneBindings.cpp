// RuneBindings.c
#include "Rune.hpp"
#include "SpriteBatch.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Rune functions

bool Rune_Initialize(Display* x11Display, uint32_t x11Window, uint32_t width, uint32_t height) {
    return Rune::Initialize(x11Display, x11Window, width, height);
}

bool Rune_IsReady() {
    #ifdef __EMSCRIPTEN__
    return Rune::ready;
    #else
    return true;
    #endif
}

void Rune_Shutdown() {
    Rune::Shutdown();
}

void Rune_SetupFrame() {
    Rune::SetupFrame();
}

void Rune_FinishFrame() {
    Rune::FinishFrame();
}

void Rune_ProcessEvents() {
    Rune::ProcessEvents();
}

void Rune_Clear(float r, float g, float b, float a) {
    Rune::Clear(r, g, b, a);
}

void Rune_Log(const char* message) {
    if (message) {
        Rune::Log(std::string(message));
    }
}

// SpriteBatch wrapper: opaque pointer type
typedef void* Rune_SpriteBatchHandle;

Rune_SpriteBatchHandle Rune_SpriteBatch_Create(const char* texturePath) {
    
}

void Rune_SpriteBatch_Destroy(Rune_SpriteBatchHandle handle) {
    if (!handle) return;
    Rune::SpriteBatch* batch = static_cast<Rune::SpriteBatch*>(handle);
    batch->Destroy();
    delete batch;
}

void Rune_SpriteBatch_Begin(Rune_SpriteBatchHandle handle) {
    if (!handle) return;
    Rune::SpriteBatch* batch = static_cast<Rune::SpriteBatch*>(handle);
    batch->Begin();
}

void Rune_SpriteBatch_End(Rune_SpriteBatchHandle handle) {
    if (!handle) return;
    Rune::SpriteBatch* batch = static_cast<Rune::SpriteBatch*>(handle);
    batch->End();
}

void Rune_SpriteBatch_Draw(Rune_SpriteBatchHandle handle, 
                           float x, float y, uint32_t w, uint32_t h,
                           float r, float g, float b) {
    if (!handle) return;
    Rune::SpriteBatch* batch = static_cast<Rune::SpriteBatch*>(handle);
    batch->Draw(x, y, w, h, r, g, b);
}

void Rune_SpriteBatch_DrawTile(Rune_SpriteBatchHandle handle,
                               float x, float y, uint32_t w, uint32_t h,
                               float r, float g, float b,
                               uint32_t tileX, uint32_t tileY,
                               uint32_t tilesPerRow, uint32_t tilesPerCol) {
    if (!handle) return;
    Rune::SpriteBatch* batch = static_cast<Rune::SpriteBatch*>(handle);
    batch->DrawTile(x, y, w, h, r, g, b, tileX, tileY, tilesPerRow, tilesPerCol);
}

#ifdef __cplusplus
}
#endif
