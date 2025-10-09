#include <Radium/Nodes/ChaiScript.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Input.hpp>
#include <Radium/Random.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <spdlog/spdlog.h>
#include <Radium/AssetLoader.hpp>

namespace Radium::Nodes
{
    void infoWrap(std::string log)
    {
        spdlog::info("[Chai] {}", log);
    }
    void errorWrap(std::string log)
    {
        spdlog::error("[Chai] {}", log);
    }
    void warnWrap(std::string log)
    {
        spdlog::warn("[Chai] {}", log);
    }
    void traceWrap(std::string log)
    {
        spdlog::trace("[Chai] {}", log);
    }
    void debugWrap(std::string log)
    {
        spdlog::debug("[Chai] {}", log);
    }

    Node *ChaiScript::GetMe()
    {
        return me;
    }

    SceneTree* ChaiScript::GetSceneTree() {
        return sceneTree;
    }

    void ChaiScript::SetMe(Node node)
    {
    }

    ChaiScript::ChaiScript(std::string path, SceneTree* tree, bool real)
    {
        this->path = path;
        this->sceneTree = tree;
        if (!real) {
            stubbed = true;
            return;
        }

        chai.add(chaiscript::fun(&infoWrap), "info");
        chai.add(chaiscript::fun(&errorWrap), "error");
        chai.add(chaiscript::fun(&warnWrap), "warn");
        chai.add(chaiscript::fun(&debugWrap), "debug");
        chai.add(chaiscript::fun(&traceWrap), "trace");

        chai.add(chaiscript::base_class<Script, ChaiScript>());
        chai.add(chaiscript::base_class<Node, Node2D>());
        chai.add(chaiscript::base_class<Node2D, Sprite2D>());
        chai.add(chaiscript::user_type<Node>(), "Node");
        chai.add(chaiscript::user_type<Script>(), "Script");
        chai.add(chaiscript::user_type<ChaiScript>(), "ChaiScript");
        chai.add(chaiscript::user_type<Node2D>(), "Node2D");
        chai.add(chaiscript::user_type<Sprite2D>(), "Sprite2D");
        chai.add(chaiscript::user_type<SceneTree>(), "SceneTree");

        chai.add(chaiscript::user_type<Vector2f>(), "Vector2f");
        chai.add(chaiscript::user_type<Vector2i>(), "Vector2i");
        chai.add(chaiscript::user_type<RectangleF>(), "RectangleF");

        chai.add(chaiscript::constructor<Vector2f(float, float)>(), "Vector2f");
        chai.add(chaiscript::constructor<Vector2i(int, int)>(), "Vector2i");
        chai.add(chaiscript::constructor<RectangleF(float, float, float, float)>(), "RectangleF");

        chai.add(chaiscript::fun(&ChaiScript::GetMe), "GetMe");
        chai.add(chaiscript::fun(&ChaiScript::GetSceneTree), "GetSceneTree");
        chai.add(chaiscript::fun(&Node2D::GetPosition), "GetPosition");
        chai.add(chaiscript::fun(&Node2D::SetPosition), "SetPosition");
        chai.add(chaiscript::fun(&Node2D::GetGlobalPosition), "GetGlobalPosition");
        chai.add(chaiscript::fun(&Node2D::SetGlobalPosition), "SetGlobalPosition");
        chai.add(chaiscript::fun(&Node2D::GetSize), "GetSize");
        chai.add(chaiscript::fun(&Node2D::SetSize), "SetSize");
        chai.add(chaiscript::fun(&Vector2f::SetX), "SetX");
        chai.add(chaiscript::fun(&Vector2f::SetY), "SetY");
        chai.add(chaiscript::fun(&Vector2i::SetX), "SetX");
        chai.add(chaiscript::fun(&Vector2i::SetY), "SetY");
        chai.add(chaiscript::fun(&RectangleF::SetX), "SetX");
        chai.add(chaiscript::fun(&RectangleF::SetY), "SetY");
        chai.add(chaiscript::fun(&RectangleF::SetW), "SetW");
        chai.add(chaiscript::fun(&RectangleF::SetH), "SetH");
        chai.add(chaiscript::fun(&SceneTree::GetNodeByPath), "GetNodeByPath");
        chai.add(chaiscript::fun(&AABB), "AABB");
        chai.add(chaiscript::fun(&exit), "exit");
        chai.add(chaiscript::fun(&Random::FloatBetween), "randf_floatbetween");
        chai.add(chaiscript::fun(&Random::IntBetween), "randf_intbetween");

        chai.add(chaiscript::fun(&Vector2f::GetX), "GetX");
        chai.add(chaiscript::fun(&Vector2f::GetY), "GetY");
        chai.add(chaiscript::fun(&Vector2i::GetX), "GetX");
        chai.add(chaiscript::fun(&Vector2i::GetY), "GetY");
        chai.add(chaiscript::fun(&RectangleF::GetX), "GetX");
        chai.add(chaiscript::fun(&RectangleF::GetY), "GetY");
        chai.add(chaiscript::fun(&RectangleF::GetW), "GetW");
        chai.add(chaiscript::fun(&RectangleF::GetH), "GetH");

        chai.add(chaiscript::fun(&Input::IsKeyDown), "IsKeyDown");
        chai.add(chaiscript::fun(&Input::IsKeyUp), "IsKeyUp");
        chai.add(chaiscript::fun(&Input::WasKeyPressedThisFrame), "WasKeyPressedThisFrame");

        chai.add_global(chaiscript::var(this), "self");

        chai.add(chaiscript::user_type<Key>(), "Key");

        chai.add_global_const(chaiscript::const_var(Key::KEY_UNKNOWN), "UNKNOWN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_A), "A");
        chai.add_global_const(chaiscript::const_var(Key::KEY_B), "B");
        chai.add_global_const(chaiscript::const_var(Key::KEY_C), "C");
        chai.add_global_const(chaiscript::const_var(Key::KEY_D), "D");
        chai.add_global_const(chaiscript::const_var(Key::KEY_E), "E");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F), "F");
        chai.add_global_const(chaiscript::const_var(Key::KEY_G), "G");
        chai.add_global_const(chaiscript::const_var(Key::KEY_H), "H");
        chai.add_global_const(chaiscript::const_var(Key::KEY_I), "I");
        chai.add_global_const(chaiscript::const_var(Key::KEY_J), "J");
        chai.add_global_const(chaiscript::const_var(Key::KEY_K), "K");
        chai.add_global_const(chaiscript::const_var(Key::KEY_L), "L");
        chai.add_global_const(chaiscript::const_var(Key::KEY_M), "M");
        chai.add_global_const(chaiscript::const_var(Key::KEY_N), "N");
        chai.add_global_const(chaiscript::const_var(Key::KEY_O), "O");
        chai.add_global_const(chaiscript::const_var(Key::KEY_P), "P");
        chai.add_global_const(chaiscript::const_var(Key::KEY_Q), "Q");
        chai.add_global_const(chaiscript::const_var(Key::KEY_R), "R");
        chai.add_global_const(chaiscript::const_var(Key::KEY_S), "S");
        chai.add_global_const(chaiscript::const_var(Key::KEY_T), "T");
        chai.add_global_const(chaiscript::const_var(Key::KEY_U), "U");
        chai.add_global_const(chaiscript::const_var(Key::KEY_V), "V");
        chai.add_global_const(chaiscript::const_var(Key::KEY_W), "W");
        chai.add_global_const(chaiscript::const_var(Key::KEY_X), "X");
        chai.add_global_const(chaiscript::const_var(Key::KEY_Y), "Y");
        chai.add_global_const(chaiscript::const_var(Key::KEY_Z), "Z");
        chai.add_global_const(chaiscript::const_var(Key::KEY__1), "_1");
        chai.add_global_const(chaiscript::const_var(Key::KEY__2), "_2");
        chai.add_global_const(chaiscript::const_var(Key::KEY__3), "_3");
        chai.add_global_const(chaiscript::const_var(Key::KEY__4), "_4");
        chai.add_global_const(chaiscript::const_var(Key::KEY__5), "_5");
        chai.add_global_const(chaiscript::const_var(Key::KEY__6), "_6");
        chai.add_global_const(chaiscript::const_var(Key::KEY__7), "_7");
        chai.add_global_const(chaiscript::const_var(Key::KEY__8), "_8");
        chai.add_global_const(chaiscript::const_var(Key::KEY__9), "_9");
        chai.add_global_const(chaiscript::const_var(Key::KEY__0), "_0");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RETURN), "RETURN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_ESCAPE), "ESCAPE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_BACKSPACE), "BACKSPACE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_TAB), "TAB");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SPACE), "SPACE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_MINUS), "MINUS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_EQUALS), "EQUALS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LEFTBRACKET), "LEFTBRACKET");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RIGHTBRACKET), "RIGHTBRACKET");
        chai.add_global_const(chaiscript::const_var(Key::KEY_BACKSLASH), "BACKSLASH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_NONUSHASH), "NONUSHASH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SEMICOLON), "SEMICOLON");
        chai.add_global_const(chaiscript::const_var(Key::KEY_APOSTROPHE), "APOSTROPHE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_GRAVE), "GRAVE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_COMMA), "COMMA");
        chai.add_global_const(chaiscript::const_var(Key::KEY_PERIOD), "PERIOD");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SLASH), "SLASH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CAPSLOCK), "CAPSLOCK");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F1), "F1");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F2), "F2");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F3), "F3");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F4), "F4");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F5), "F5");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F6), "F6");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F7), "F7");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F8), "F8");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F9), "F9");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F10), "F10");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F11), "F11");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F12), "F12");
        chai.add_global_const(chaiscript::const_var(Key::KEY_PRINTSCREEN), "PRINTSCREEN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SCROLLLOCK), "SCROLLLOCK");
        chai.add_global_const(chaiscript::const_var(Key::KEY_PAUSE), "PAUSE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INSERT), "INSERT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_HOME), "HOME");
        chai.add_global_const(chaiscript::const_var(Key::KEY_PAGEUP), "PAGEUP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_DELETE), "DELETE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_END), "END");
        chai.add_global_const(chaiscript::const_var(Key::KEY_PAGEDOWN), "PAGEDOWN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RIGHT), "RIGHT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LEFT), "LEFT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_DOWN), "DOWN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_UP), "UP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_NUMLOCKCLEAR), "NUMLOCKCLEAR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_DIVIDE), "KP_DIVIDE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MULTIPLY), "KP_MULTIPLY");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MINUS), "KP_MINUS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_PLUS), "KP_PLUS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_ENTER), "KP_ENTER");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_1), "KP_1");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_2), "KP_2");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_3), "KP_3");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_4), "KP_4");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_5), "KP_5");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_6), "KP_6");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_7), "KP_7");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_8), "KP_8");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_9), "KP_9");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_0), "KP_0");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_PERIOD), "KP_PERIOD");
        chai.add_global_const(chaiscript::const_var(Key::KEY_NONUSBACKSLASH), "NONUSBACKSLASH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_APPLICATION), "APPLICATION");
        chai.add_global_const(chaiscript::const_var(Key::KEY_POWER), "POWER");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_EQUALS), "KP_EQUALS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F13), "F13");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F14), "F14");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F15), "F15");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F16), "F16");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F17), "F17");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F18), "F18");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F19), "F19");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F20), "F20");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F21), "F21");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F22), "F22");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F23), "F23");
        chai.add_global_const(chaiscript::const_var(Key::KEY_F24), "F24");
        chai.add_global_const(chaiscript::const_var(Key::KEY_EXECUTE), "EXECUTE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_HELP), "HELP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_MENU), "MENU");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SELECT), "SELECT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_STOP), "STOP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AGAIN), "AGAIN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_UNDO), "UNDO");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CUT), "CUT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_COPY), "COPY");
        chai.add_global_const(chaiscript::const_var(Key::KEY_PASTE), "PASTE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_FIND), "FIND");
        chai.add_global_const(chaiscript::const_var(Key::KEY_MUTE), "MUTE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_VOLUMEUP), "VOLUMEUP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_VOLUMEDOWN), "VOLUMEDOWN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_COMMA), "KP_COMMA");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_EQUALSAS400), "KP_EQUALSAS400");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL1), "INTERNATIONAL1");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL2), "INTERNATIONAL2");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL3), "INTERNATIONAL3");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL4), "INTERNATIONAL4");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL5), "INTERNATIONAL5");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL6), "INTERNATIONAL6");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL7), "INTERNATIONAL7");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL8), "INTERNATIONAL8");
        chai.add_global_const(chaiscript::const_var(Key::KEY_INTERNATIONAL9), "INTERNATIONAL9");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG1), "LANG1");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG2), "LANG2");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG3), "LANG3");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG4), "LANG4");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG5), "LANG5");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG6), "LANG6");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG7), "LANG7");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG8), "LANG8");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LANG9), "LANG9");
        chai.add_global_const(chaiscript::const_var(Key::KEY_ALTERASE), "ALTERASE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SYSREQ), "SYSREQ");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CANCEL), "CANCEL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CLEAR), "CLEAR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_PRIOR), "PRIOR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RETURN2), "RETURN2");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SEPARATOR), "SEPARATOR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_OUT), "OUT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_OPER), "OPER");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CLEARAGAIN), "CLEARAGAIN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CRSEL), "CRSEL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_EXSEL), "EXSEL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_00), "KP_00");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_000), "KP_000");
        chai.add_global_const(chaiscript::const_var(Key::KEY_THOUSANDSSEPARATOR), "THOUSANDSSEPARATOR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_DECIMALSEPARATOR), "DECIMALSEPARATOR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CURRENCYUNIT), "CURRENCYUNIT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CURRENCYSUBUNIT), "CURRENCYSUBUNIT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_LEFTPAREN), "KP_LEFTPAREN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_RIGHTPAREN), "KP_RIGHTPAREN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_LEFTBRACE), "KP_LEFTBRACE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_RIGHTBRACE), "KP_RIGHTBRACE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_TAB), "KP_TAB");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_BACKSPACE), "KP_BACKSPACE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_A), "KP_A");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_B), "KP_B");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_C), "KP_C");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_D), "KP_D");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_E), "KP_E");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_F), "KP_F");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_XOR), "KP_XOR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_POWER), "KP_POWER");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_PERCENT), "KP_PERCENT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_LESS), "KP_LESS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_GREATER), "KP_GREATER");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_AMPERSAND), "KP_AMPERSAND");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_DBLAMPERSAND), "KP_DBLAMPERSAND");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_VERTICALBAR), "KP_VERTICALBAR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_DBLVERTICALBAR), "KP_DBLVERTICALBAR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_COLON), "KP_COLON");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_HASH), "KP_HASH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_SPACE), "KP_SPACE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_AT), "KP_AT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_EXCLAM), "KP_EXCLAM");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MEMSTORE), "KP_MEMSTORE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MEMRECALL), "KP_MEMRECALL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MEMCLEAR), "KP_MEMCLEAR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MEMADD), "KP_MEMADD");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MEMSUBTRACT), "KP_MEMSUBTRACT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MEMMULTIPLY), "KP_MEMMULTIPLY");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_MEMDIVIDE), "KP_MEMDIVIDE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_PLUSMINUS), "KP_PLUSMINUS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_CLEAR), "KP_CLEAR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_CLEARENTRY), "KP_CLEARENTRY");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_BINARY), "KP_BINARY");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_OCTAL), "KP_OCTAL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_DECIMAL), "KP_DECIMAL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KP_HEXADECIMAL), "KP_HEXADECIMAL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LCTRL), "LCTRL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LSHIFT), "LSHIFT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LALT), "LALT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_LGUI), "LGUI");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RCTRL), "RCTRL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RSHIFT), "RSHIFT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RALT), "RALT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_RGUI), "RGUI");
        chai.add_global_const(chaiscript::const_var(Key::KEY_MODE), "MODE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AUDIONEXT), "AUDIONEXT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AUDIOPREV), "AUDIOPREV");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AUDIOSTOP), "AUDIOSTOP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AUDIOPLAY), "AUDIOPLAY");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AUDIOMUTE), "AUDIOMUTE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_MEDIASELECT), "MEDIASELECT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_WWW), "WWW");
        chai.add_global_const(chaiscript::const_var(Key::KEY_MAIL), "MAIL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CALCULATOR), "CALCULATOR");
        chai.add_global_const(chaiscript::const_var(Key::KEY_COMPUTER), "COMPUTER");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AC_SEARCH), "AC_SEARCH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AC_HOME), "AC_HOME");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AC_BACK), "AC_BACK");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AC_FORWARD), "AC_FORWARD");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AC_STOP), "AC_STOP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AC_REFRESH), "AC_REFRESH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AC_BOOKMARKS), "AC_BOOKMARKS");
        chai.add_global_const(chaiscript::const_var(Key::KEY_BRIGHTNESSDOWN), "BRIGHTNESSDOWN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_BRIGHTNESSUP), "BRIGHTNESSUP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_DISPLAYSWITCH), "DISPLAYSWITCH");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KBDILLUMTOGGLE), "KBDILLUMTOGGLE");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KBDILLUMDOWN), "KBDILLUMDOWN");
        chai.add_global_const(chaiscript::const_var(Key::KEY_KBDILLUMUP), "KBDILLUMUP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_EJECT), "EJECT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SLEEP), "SLEEP");
        chai.add_global_const(chaiscript::const_var(Key::KEY_APP1), "APP1");
        chai.add_global_const(chaiscript::const_var(Key::KEY_APP2), "APP2");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AUDIOREWIND), "AUDIOREWIND");
        chai.add_global_const(chaiscript::const_var(Key::KEY_AUDIOFASTFORWARD), "AUDIOFASTFORWARD");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SOFTLEFT), "SOFTLEFT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_SOFTRIGHT), "SOFTRIGHT");
        chai.add_global_const(chaiscript::const_var(Key::KEY_CALL), "CALL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_ENDCALL), "ENDCALL");
        chai.add_global_const(chaiscript::const_var(Key::KEY_NUM_SCANCODES), "NUM_SCANCODES");


        chai.eval(ReadFileToString(path));
    }

    void ChaiScript::OnLoad()
    {
        if (stubbed) {
            return;
        }
        std::function<void()> onLoad = chai.eval<std::function<void()>>("onLoad");

        onLoad();
    }

    void ChaiScript::OnTick(float dt)
    {
        if (stubbed) {
            return;
        }
        std::function<void(float)> onTick = chai.eval<std::function<void(float)>>("onTick");

        onTick(dt);
    }

    void ChaiScript::OnRender()
    {
        if (stubbed) {
            return;
        }
        std::function<void()> onRender = chai.eval<std::function<void()>>("onRender");

        onRender();
    }

    void ChaiScript::OnImgui()
    {
        if (stubbed) {
            return;
        }
        std::function<void()> onImgui = chai.eval<std::function<void()>>("onImgui");

        onImgui();
    }
}