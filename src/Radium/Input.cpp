#include <Radium/Input.hpp>

namespace Radium {
    namespace Input {
        static const Uint8* sdlState = nullptr;
        static Uint8 currentState[SDL_NUM_SCANCODES] = {0};
        static Uint8 previousState[SDL_NUM_SCANCODES] = {0};

        void Update() {
            // Get the current key state from SDL
            sdlState = SDL_GetKeyboardState(nullptr);

            // Copy the SDL state into our own currentState buffer
            memcpy(currentState, sdlState, SDL_NUM_SCANCODES);
        }

        void LateUpdate() {
            // Copy current state into previous state AFTER processing input
            memcpy(previousState, currentState, SDL_NUM_SCANCODES);
        }

        bool IsKeyDown(Key key) {
            return currentState[(Uint8)key];
        }

        bool IsKeyUp(Key key) {
            return !currentState[(Uint8)key];
        }

        bool WasKeyPressedThisFrame(Key key) {
            return currentState[(Uint8)key] && !previousState[(Uint8)key];
        }

        bool WasKeyReleasedThisFrame(Key key) {
            return !currentState[(Uint8)key] && previousState[(Uint8)key];
        }
    }
}
