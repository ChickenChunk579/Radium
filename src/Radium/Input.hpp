#pragma once
#include <Radium/Application.hpp>
#include <Radium/Key.hpp>

namespace Radium {
    namespace Input {
        bool IsKeyDown(Key key);
        bool IsKeyUp(Key key);
        bool WasKeyPressedThisFrame(Key key);
        void Update();
        void LateUpdate();
    }
}