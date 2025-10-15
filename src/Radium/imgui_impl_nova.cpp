#include <Radium/imgui_impl_nova.h>
#include <Nova/Key.hpp>

struct ImGui_ImplNova_Data {
    Nova::Window* window;
};

static ImGuiKey NovaKeyToImGuiKey(Key key) {
    switch (key) {
        case Key::KEY_TAB: return ImGuiKey_Tab;
        case Key::KEY_LEFT: return ImGuiKey_LeftArrow;
        case Key::KEY_RIGHT: return ImGuiKey_RightArrow;
        case Key::KEY_UP: return ImGuiKey_UpArrow;
        case Key::KEY_DOWN: return ImGuiKey_DownArrow;
        case Key::KEY_PAGEUP: return ImGuiKey_PageUp;
        case Key::KEY_PAGEDOWN: return ImGuiKey_PageDown;
        case Key::KEY_HOME: return ImGuiKey_Home;
        case Key::KEY_END: return ImGuiKey_End;
        case Key::KEY_INSERT: return ImGuiKey_Insert;
        case Key::KEY_DELETE: return ImGuiKey_Delete;
        case Key::KEY_BACKSPACE: return ImGuiKey_Backspace;
        case Key::KEY_SPACE: return ImGuiKey_Space;
        case Key::KEY_RETURN: return ImGuiKey_Enter;
        case Key::KEY_ESCAPE: return ImGuiKey_Escape;
        case Key::KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
        case Key::KEY_COMMA: return ImGuiKey_Comma;
        case Key::KEY_MINUS: return ImGuiKey_Minus;
        case Key::KEY_PERIOD: return ImGuiKey_Period;
        case Key::KEY_SLASH: return ImGuiKey_Slash;
        case Key::KEY_SEMICOLON: return ImGuiKey_Semicolon;
        case Key::KEY_EQUALS: return ImGuiKey_Equal;
        case Key::KEY_LEFTBRACKET: return ImGuiKey_LeftBracket;
        case Key::KEY_BACKSLASH: return ImGuiKey_Backslash;
        case Key::KEY_RIGHTBRACKET: return ImGuiKey_RightBracket;
        case Key::KEY_GRAVE: return ImGuiKey_GraveAccent;
        case Key::KEY_CAPSLOCK: return ImGuiKey_CapsLock;
        case Key::KEY_SCROLLLOCK: return ImGuiKey_ScrollLock;
        case Key::KEY_NUMLOCKCLEAR: return ImGuiKey_NumLock;
        case Key::KEY_PRINTSCREEN: return ImGuiKey_PrintScreen;
        case Key::KEY_PAUSE: return ImGuiKey_Pause;
        case Key::KEY_KP_0: return ImGuiKey_Keypad0;
        case Key::KEY_KP_1: return ImGuiKey_Keypad1;
        case Key::KEY_KP_2: return ImGuiKey_Keypad2;
        case Key::KEY_KP_3: return ImGuiKey_Keypad3;
        case Key::KEY_KP_4: return ImGuiKey_Keypad4;
        case Key::KEY_KP_5: return ImGuiKey_Keypad5;
        case Key::KEY_KP_6: return ImGuiKey_Keypad6;
        case Key::KEY_KP_7: return ImGuiKey_Keypad7;
        case Key::KEY_KP_8: return ImGuiKey_Keypad8;
        case Key::KEY_KP_9: return ImGuiKey_Keypad9;
        case Key::KEY_KP_PERIOD: return ImGuiKey_KeypadDecimal;
        case Key::KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
        case Key::KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case Key::KEY_KP_MINUS: return ImGuiKey_KeypadSubtract;
        case Key::KEY_KP_PLUS: return ImGuiKey_KeypadAdd;
        case Key::KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
        case Key::KEY_KP_EQUALS: return ImGuiKey_KeypadEqual;
        case Key::KEY_LCTRL: return ImGuiKey_LeftCtrl;
        case Key::KEY_LSHIFT: return ImGuiKey_LeftShift;
        case Key::KEY_LALT: return ImGuiKey_LeftAlt;
        case Key::KEY_LGUI: return ImGuiKey_LeftSuper;
        case Key::KEY_RCTRL: return ImGuiKey_RightCtrl;
        case Key::KEY_RSHIFT: return ImGuiKey_RightShift;
        case Key::KEY_RALT: return ImGuiKey_RightAlt;
        case Key::KEY_RGUI: return ImGuiKey_RightSuper;
        case Key::KEY_APPLICATION: return ImGuiKey_Menu;
        case Key::KEY__0: return ImGuiKey_0;
        case Key::KEY__1: return ImGuiKey_1;
        case Key::KEY__2: return ImGuiKey_2;
        case Key::KEY__3: return ImGuiKey_3;
        case Key::KEY__4: return ImGuiKey_4;
        case Key::KEY__5: return ImGuiKey_5;
        case Key::KEY__6: return ImGuiKey_6;
        case Key::KEY__7: return ImGuiKey_7;
        case Key::KEY__8: return ImGuiKey_8;
        case Key::KEY__9: return ImGuiKey_9;
        case Key::KEY_A: return ImGuiKey_A;
        case Key::KEY_B: return ImGuiKey_B;
        case Key::KEY_C: return ImGuiKey_C;
        case Key::KEY_D: return ImGuiKey_D;
        case Key::KEY_E: return ImGuiKey_E;
        case Key::KEY_F: return ImGuiKey_F;
        case Key::KEY_G: return ImGuiKey_G;
        case Key::KEY_H: return ImGuiKey_H;
        case Key::KEY_I: return ImGuiKey_I;
        case Key::KEY_J: return ImGuiKey_J;
        case Key::KEY_K: return ImGuiKey_K;
        case Key::KEY_L: return ImGuiKey_L;
        case Key::KEY_M: return ImGuiKey_M;
        case Key::KEY_N: return ImGuiKey_N;
        case Key::KEY_O: return ImGuiKey_O;
        case Key::KEY_P: return ImGuiKey_P;
        case Key::KEY_Q: return ImGuiKey_Q;
        case Key::KEY_R: return ImGuiKey_R;
        case Key::KEY_S: return ImGuiKey_S;
        case Key::KEY_T: return ImGuiKey_T;
        case Key::KEY_U: return ImGuiKey_U;
        case Key::KEY_V: return ImGuiKey_V;
        case Key::KEY_W: return ImGuiKey_W;
        case Key::KEY_X: return ImGuiKey_X;
        case Key::KEY_Y: return ImGuiKey_Y;
        case Key::KEY_Z: return ImGuiKey_Z;
        case Key::KEY_F1: return ImGuiKey_F1;
        case Key::KEY_F2: return ImGuiKey_F2;
        case Key::KEY_F3: return ImGuiKey_F3;
        case Key::KEY_F4: return ImGuiKey_F4;
        case Key::KEY_F5: return ImGuiKey_F5;
        case Key::KEY_F6: return ImGuiKey_F6;
        case Key::KEY_F7: return ImGuiKey_F7;
        case Key::KEY_F8: return ImGuiKey_F8;
        case Key::KEY_F9: return ImGuiKey_F9;
        case Key::KEY_F10: return ImGuiKey_F10;
        case Key::KEY_F11: return ImGuiKey_F11;
        case Key::KEY_F12: return ImGuiKey_F12;
        case Key::KEY_F13: return ImGuiKey_F13;
        case Key::KEY_F14: return ImGuiKey_F14;
        case Key::KEY_F15: return ImGuiKey_F15;
        case Key::KEY_F16: return ImGuiKey_F16;
        case Key::KEY_F17: return ImGuiKey_F17;
        case Key::KEY_F18: return ImGuiKey_F18;
        case Key::KEY_F19: return ImGuiKey_F19;
        case Key::KEY_F20: return ImGuiKey_F20;
        case Key::KEY_F21: return ImGuiKey_F21;
        case Key::KEY_F22: return ImGuiKey_F22;
        case Key::KEY_F23: return ImGuiKey_F23;
        case Key::KEY_F24: return ImGuiKey_F24;
        default: return ImGuiKey_None;
    }
}

static ImGui_ImplNova_Data* ImGui_ImplNova_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplNova_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

bool ImGui_ImplNova_Init(Nova::Window* window) {
    ImGuiIO& io = ImGui::GetIO();
    IMGUI_CHECKVERSION();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    ImGui_ImplNova_Data* bd = IM_NEW(ImGui_ImplNova_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_nova";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;           // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;            // We can honor io.WantSetMousePos requests (optional, rarely used)

    bd->window = window;

    io.DisplaySize.x = window->width;
    io.DisplaySize.y = window->height;

    return true;
}


void ImGui_ImplNova_ProcessEvent(Nova::Event* event) {
    ImGuiIO& io = ImGui::GetIO();

    if (Nova::MouseMoveEvent* move = dynamic_cast<Nova::MouseMoveEvent*>(event)) {
        io.AddMousePosEvent((float)move->x, (float)move->y);
    }

    if (Nova::MouseButtonDownEvent* mouseButtonDown = dynamic_cast<Nova::MouseButtonDownEvent*>(event)) {
        int button = -1;
        switch (mouseButtonDown->button) {
            case Nova::MouseButton::Left:   button = 0; break;
            case Nova::MouseButton::Right:  button = 1; break;
            case Nova::MouseButton::Middle: button = 2; break;
            default: break;
        }
        if (button != -1)
            io.AddMouseButtonEvent(button, true);
    }

    if (Nova::MouseButtonUpEvent* mouseButtonUp = dynamic_cast<Nova::MouseButtonUpEvent*>(event)) {
        int button = -1;
        switch (mouseButtonUp->button) {
            case Nova::MouseButton::Left:   button = 0; break;
            case Nova::MouseButton::Right:  button = 1; break;
            case Nova::MouseButton::Middle: button = 2; break;
            default: break;
        }
        if (button != -1)
            io.AddMouseButtonEvent(button, false);
    }

    if (Nova::KeyDownEvent* keyDownEvent = dynamic_cast<Nova::KeyDownEvent*>(event)) {
        ImGuiIO& io = ImGui::GetIO();
        
        Key key = keyDownEvent->key;
        ImGuiKey imguiKey = NovaKeyToImGuiKey(key);
        if (imguiKey != ImGuiKey_None)
            io.AddKeyEvent(imguiKey, true);


        std::optional<char> c = KeyToChar(key, keyDownEvent->shift);
        if (c.has_value()) {
            io.AddInputCharacter(c.value());
        }
        
    }


    if (Nova::KeyUpEvent* keyUpEvent = dynamic_cast<Nova::KeyUpEvent*>(event)) {
        ImGuiIO& io = ImGui::GetIO();
        
        Key key = keyUpEvent->key;

        Flux::Info("Key up: {}", (int)key);

        if (key != Key::KEY_UNKNOWN) {
            io.AddKeyEvent(NovaKeyToImGuiKey(key), false);
        }
    }
}
