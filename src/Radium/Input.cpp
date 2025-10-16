#include <Radium/Input.hpp>
#include <unordered_map>
#include <Nova/Key.hpp>

namespace Radium {
    namespace Input {
        static std::unordered_map<Key, bool> keyStates;
        static std::unordered_map<Key, bool> keyPressedThisFrame;
        static std::unordered_map<Key, bool> keyReleasedThisFrame;

        static bool leftMouseDown = false;
        static bool rightMouseDown = false;
        static bool middleMouseDown = false;

        static Vector2f mousePosition = {0.0f, 0.0f};

        // Process a single Nova event
        void ProcessEvent(Nova::Event* event) {
            if (!event) return;

            if (auto* e = dynamic_cast<Nova::KeyDownEvent*>(event)) {
                keyStates[e->key] = true;
                keyPressedThisFrame[e->key] = true;
            } else if (auto* e = dynamic_cast<Nova::KeyUpEvent*>(event)) {
                keyStates[e->key] = false;
                keyReleasedThisFrame[e->key] = true;
            } else if (auto* e = dynamic_cast<Nova::MouseMoveEvent*>(event)) {
                mousePosition = {static_cast<float>(e->x), static_cast<float>(e->y)};
            } else if (auto* e = dynamic_cast<Nova::MouseButtonDownEvent*>(event)) {
                switch (e->button) {
                    case Nova::MouseButton::Left:
                        leftMouseDown = true;
                        break;
                    case Nova::MouseButton::Right:
                        rightMouseDown = true;
                        break;
                    case Nova::MouseButton::Middle:
                        middleMouseDown = true;
                        break;
                }
            } else if (auto* e = dynamic_cast<Nova::MouseButtonUpEvent*>(event)) {
                switch (e->button) {
                    case Nova::MouseButton::Left:
                        leftMouseDown = false;
                        break;
                    case Nova::MouseButton::Right:
                        rightMouseDown = false;
                        break;
                    case Nova::MouseButton::Middle:
                        middleMouseDown = false;
                        break;
                }
            }
        }

        void Update() {
            // Called at the start of a frame
            keyPressedThisFrame.clear();
            keyReleasedThisFrame.clear();
        }

        void LateUpdate() {
            // Not needed in this simple case, but could be used to flush input queue if needed
        }

        bool IsKeyDown(Key key) {
            return keyStates[key];
        }

        bool IsKeyUp(Key key) {
            return !keyStates[key];
        }

        bool WasKeyPressedThisFrame(Key key) {
            return keyPressedThisFrame[key];
        }

        bool WasKeyReleasedThisFrame(Key key) {
            return keyReleasedThisFrame[key];
        }

        Vector2f GetMousePosition() {
            return mousePosition;
        }

        bool IsLeftMouseButtonDown() {
            return leftMouseDown;
        }

        bool IsMiddleMouseButtonDown() {
            return middleMouseDown;
        }

        bool IsRightMouseButtonDown() {
            return rightMouseDown;
        }
    }
}
