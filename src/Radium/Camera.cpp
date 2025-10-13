#include <Radium/Camera.hpp>

namespace Radium {

    Camera::Camera() : offset(0.0f, 0.0f) {
    }

    Camera::Camera(const Vector2f& offset) : offset(offset) {
    }

    void Camera::Register() {
        CLASSDB_REGISTER(Camera);
        CLASSDB_DECLARE_PROPERTY(Camera, Vector2f, offset);
    }

    RectangleF Camera::GetViewport(float screenWidth, float screenHeight) const {
        return RectangleF(
            offset.x,
            offset.y,
            screenWidth,
            screenHeight
        );
    }

} // namespace Radium
