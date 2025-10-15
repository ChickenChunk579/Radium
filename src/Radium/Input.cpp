#include <Radium/Input.hpp>

namespace Radium {
    namespace Input {
        void Update() {
            
        }

        void LateUpdate() {
            
        }

        bool IsKeyDown(Key key) {
            return false;
        }

        bool IsKeyUp(Key key) {
            return false;
        }

        Vector2f GetMousePosition() {
            int x = 0;
            int y = 0;

            return {(float)x, (float)y};
        }

        bool IsLeftMouseButtonDown() {
            int x = 0;
            int y = 0;
            return false;
        }

        bool IsMiddleMouseButtonDown() {
            int x = 0;
            int y = 0;
            return false;
        }

        bool IsRightMouseButtonDown() {
            return false;
        }


        bool WasKeyPressedThisFrame(Key key) {
            return false;
        }

        bool WasKeyReleasedThisFrame(Key key) {
            return false;
        }
    }
}
