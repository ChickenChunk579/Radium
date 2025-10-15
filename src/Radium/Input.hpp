#pragma once
#include <Radium/Application.hpp>
#include <Nova/Key.hpp>

namespace Radium {
    namespace Input {
        /// @brief Check if a key is pressed, and is currently down
        /// @param key The key to check
        /// @return State of the key
        bool IsKeyDown(Key key);

        /// @brief Check if a key is not pressed
        /// @param key The key to check
        /// @return Invert state of the key, so if it is not pressed or not
        bool IsKeyUp(Key key);

        /// @brief Check if a key was pressed this frame (only calls once when the key is first pressed)
        /// @param key The key to check
        /// @return The state of the key
        bool WasKeyPressedThisFrame(Key key);

        /// @brief Get mouse position
        Vector2f GetMousePosition();

        /// @brief Check if LMB is down
        bool IsLeftMouseButtonDown();

        /// @brief Check if RMB is down
        bool IsRightMouseButtonDown();

        /// @brief Check if MMB is down
        bool IsMiddleMouseButtonDown();

        /// @brief Called internally by Application, manages input for WasKeyPressedThisFrame
        void Update();

        /// @brief Called internally by Application, manages input for WasKeyPressedThisFrame
        void LateUpdate();
    }
}