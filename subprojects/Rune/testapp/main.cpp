#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <tuple>
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "Rune.hpp"
#include "SpriteBatch.hpp"
// ... (same includes as before)
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

#endif
using namespace Rune;

struct Ball {
    float x, y;
    float vx, vy;
    float size;
    float r, g, b, a;
    int tileX, tileY;
};

struct AppState {
    SDL_Window* window = nullptr;
    bool running = true;
    bool adding = false;
    std::vector<Ball> balls;
    Rune::SpriteBatch* batch = nullptr;

    std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> speedDist{-200.0f, 200.0f};
    std::uniform_real_distribution<float> colorDist{0.2f, 1.0f};
    std::uniform_int_distribution<int> tileDistX;
    std::uniform_int_distribution<int> tileDistY;

    int frameCount = 0;
    double fps = 0.0;
    std::chrono::high_resolution_clock::time_point fpsTimerStart;

    AppState(int tileCols, int tileRows)
        : tileDistX(0, tileCols - 1), tileDistY(0, tileRows - 1),
          fpsTimerStart(std::chrono::high_resolution_clock::now()) {}
};

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;
constexpr int targetFPS = 60;
constexpr int frameDelayMs = 1000 / targetFPS;
constexpr int tileCols = 512 / 16;
constexpr int tileRows = 1024 / 16;

bool RunFrame(double time, void* userData) {
    AppState* state = static_cast<AppState*>(userData);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            state->running = false;
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
            state->adding = !state->adding;
    }

    if (state->adding) {
        Ball b{};
        b.x = screenWidth / 2.0f;
        b.y = screenHeight / 2.0f;
        b.vx = state->speedDist(state->rng);
        b.vy = state->speedDist(state->rng);
        b.size = 64.0f;
        b.r = state->colorDist(state->rng);
        b.g = state->colorDist(state->rng);
        b.b = state->colorDist(state->rng);
        b.a = 1.0f;
        b.tileX = state->tileDistX(state->rng);
        b.tileY = state->tileDistY(state->rng);
        state->balls.push_back(b);
    }

    float deltaTime = 1.0f / targetFPS;
    for (auto& ball : state->balls) {
        ball.x += ball.vx * deltaTime;
        ball.y += ball.vy * deltaTime;

        if (ball.x <= 0 || ball.x + ball.size >= screenWidth) {
            ball.vx *= -1;
            ball.x = std::max(0.0f, std::min(ball.x, screenWidth - ball.size));
        }

        if (ball.y <= 0 || ball.y + ball.size >= screenHeight) {
            ball.vy *= -1;
            ball.y = std::max(0.0f, std::min(ball.y, screenHeight - ball.size));
        }
    }

    Rune::Clear(0, 0, 0, 1);
    Rune::SetupFrame();

    state->batch->Begin();
    for (const auto& ball : state->balls) {
        state->batch->DrawTile(
            static_cast<int>(ball.x), static_cast<int>(ball.y),
            static_cast<int>(ball.size), static_cast<int>(ball.size),
            1, 1, 1,
            ball.tileX, ball.tileY,
            tileRows, tileCols
        );
    }
    state->batch->End();
    Rune::FinishFrame();

    state->frameCount++;
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - state->fpsTimerStart;

    if (elapsed.count() >= 1.0) {
        state->fps = state->frameCount / elapsed.count();
        state->frameCount = 0;
        state->fpsTimerStart = now;

        std::string title = "Rune App - FPS: " + std::to_string(static_cast<int>(state->fps)) +
                            " | Balls: " + std::to_string(state->balls.size());
        SDL_SetWindowTitle(state->window, title.c_str());
    }

    return true;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Rune App",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Display* x11Display = nullptr;
    uint32_t x11Window = 0;

#ifndef __EMSCRIPTEN__
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(window, &wmInfo) || wmInfo.subsystem != SDL_SYSWM_X11) {
        std::cerr << "Failed to get X11 window info" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    x11Display = wmInfo.info.x11.display;
    x11Window = static_cast<uint32_t>(wmInfo.info.x11.window);
#endif

    if (!Rune::Initialize(x11Display, x11Window, screenWidth, screenHeight)) {
        std::cerr << "Rune initialization failed." << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    auto* batch = new Rune::SpriteBatch("assets/atlas.png");
    auto* state = new AppState(tileCols, tileRows);
    state->window = window;
    state->batch = batch;

    // Add some starter balls
    for (int i = 0; i < 10; i++) {
        Ball b{};
        b.x = screenWidth / 2.0f;
        b.y = screenHeight / 2.0f;
        b.vx = state->speedDist(state->rng);
        b.vy = state->speedDist(state->rng);
        b.size = 64.0f;
        b.r = state->colorDist(state->rng);
        b.g = state->colorDist(state->rng);
        b.b = state->colorDist(state->rng);
        b.a = 1.0f;
        b.tileX = state->tileDistX(state->rng);
        b.tileY = state->tileDistY(state->rng);
        state->balls.push_back(b);
    }

#ifdef __EMSCRIPTEN__
    emscripten_request_animation_frame_loop(RunFrame, state);
    while (true) {
        emscripten_sleep(1);
    }
#else
    // Native: use while loop
    while (state->running) {
        RunFrame(0.0, state);
        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelayMs));
    }
#endif

    Rune::Shutdown();
    delete batch;
    SDL_DestroyWindow(window);
    SDL_Quit();
    delete state;

    return 0;
}
