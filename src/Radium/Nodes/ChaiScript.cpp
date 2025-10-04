#include <Radium/Nodes/ChaiScript.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Input.hpp>
#include <Radium/Random.hpp>
#include <Radium/Nodes/2D/Node2D.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <spdlog/spdlog.h>

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

    void ChaiScript::SetMe(Node node)
    {
    }

    ChaiScript::ChaiScript(std::string path)
    {
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

        chai.add(chaiscript::user_type<Vector2f>(), "Vector2f");
        chai.add(chaiscript::user_type<Vector2i>(), "Vector2i");
        chai.add(chaiscript::user_type<RectangleF>(), "RectangleF");

        chai.add(chaiscript::constructor<Vector2f(float, float)>(), "Vector2f");
        chai.add(chaiscript::constructor<Vector2i(int, int)>(), "Vector2i");
        chai.add(chaiscript::constructor<RectangleF(float, float, float, float)>(), "RectangleF");

        chai.add(chaiscript::fun(&ChaiScript::GetMe), "GetMe");
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

        chai.add_global_const(chaiscript::const_var(Key::UNKNOWN), "UNKNOWN");
        chai.add_global_const(chaiscript::const_var(Key::A), "A");
        chai.add_global_const(chaiscript::const_var(Key::B), "B");
        chai.add_global_const(chaiscript::const_var(Key::C), "C");
        chai.add_global_const(chaiscript::const_var(Key::D), "D");
        chai.add_global_const(chaiscript::const_var(Key::E), "E");
        chai.add_global_const(chaiscript::const_var(Key::F), "F");
        chai.add_global_const(chaiscript::const_var(Key::G), "G");
        chai.add_global_const(chaiscript::const_var(Key::H), "H");
        chai.add_global_const(chaiscript::const_var(Key::I), "I");
        chai.add_global_const(chaiscript::const_var(Key::J), "J");
        chai.add_global_const(chaiscript::const_var(Key::K), "K");
        chai.add_global_const(chaiscript::const_var(Key::L), "L");
        chai.add_global_const(chaiscript::const_var(Key::M), "M");
        chai.add_global_const(chaiscript::const_var(Key::N), "N");
        chai.add_global_const(chaiscript::const_var(Key::O), "O");
        chai.add_global_const(chaiscript::const_var(Key::P), "P");
        chai.add_global_const(chaiscript::const_var(Key::Q), "Q");
        chai.add_global_const(chaiscript::const_var(Key::R), "R");
        chai.add_global_const(chaiscript::const_var(Key::S), "S");
        chai.add_global_const(chaiscript::const_var(Key::T), "T");
        chai.add_global_const(chaiscript::const_var(Key::U), "U");
        chai.add_global_const(chaiscript::const_var(Key::V), "V");
        chai.add_global_const(chaiscript::const_var(Key::W), "W");
        chai.add_global_const(chaiscript::const_var(Key::X), "X");
        chai.add_global_const(chaiscript::const_var(Key::Y), "Y");
        chai.add_global_const(chaiscript::const_var(Key::Z), "Z");
        chai.add_global_const(chaiscript::const_var(Key::_1), "_1");
        chai.add_global_const(chaiscript::const_var(Key::_2), "_2");
        chai.add_global_const(chaiscript::const_var(Key::_3), "_3");
        chai.add_global_const(chaiscript::const_var(Key::_4), "_4");
        chai.add_global_const(chaiscript::const_var(Key::_5), "_5");
        chai.add_global_const(chaiscript::const_var(Key::_6), "_6");
        chai.add_global_const(chaiscript::const_var(Key::_7), "_7");
        chai.add_global_const(chaiscript::const_var(Key::_8), "_8");
        chai.add_global_const(chaiscript::const_var(Key::_9), "_9");
        chai.add_global_const(chaiscript::const_var(Key::_0), "_0");
        chai.add_global_const(chaiscript::const_var(Key::RETURN), "RETURN");
        chai.add_global_const(chaiscript::const_var(Key::ESCAPE), "ESCAPE");
        chai.add_global_const(chaiscript::const_var(Key::BACKSPACE), "BACKSPACE");
        chai.add_global_const(chaiscript::const_var(Key::TAB), "TAB");
        chai.add_global_const(chaiscript::const_var(Key::SPACE), "SPACE");
        chai.add_global_const(chaiscript::const_var(Key::MINUS), "MINUS");
        chai.add_global_const(chaiscript::const_var(Key::EQUALS), "EQUALS");
        chai.add_global_const(chaiscript::const_var(Key::LEFTBRACKET), "LEFTBRACKET");
        chai.add_global_const(chaiscript::const_var(Key::RIGHTBRACKET), "RIGHTBRACKET");
        chai.add_global_const(chaiscript::const_var(Key::BACKSLASH), "BACKSLASH");
        chai.add_global_const(chaiscript::const_var(Key::NONUSHASH), "NONUSHASH");
        chai.add_global_const(chaiscript::const_var(Key::SEMICOLON), "SEMICOLON");
        chai.add_global_const(chaiscript::const_var(Key::APOSTROPHE), "APOSTROPHE");
        chai.add_global_const(chaiscript::const_var(Key::GRAVE), "GRAVE");
        chai.add_global_const(chaiscript::const_var(Key::COMMA), "COMMA");
        chai.add_global_const(chaiscript::const_var(Key::PERIOD), "PERIOD");
        chai.add_global_const(chaiscript::const_var(Key::SLASH), "SLASH");
        chai.add_global_const(chaiscript::const_var(Key::CAPSLOCK), "CAPSLOCK");
        chai.add_global_const(chaiscript::const_var(Key::F1), "F1");
        chai.add_global_const(chaiscript::const_var(Key::F2), "F2");
        chai.add_global_const(chaiscript::const_var(Key::F3), "F3");
        chai.add_global_const(chaiscript::const_var(Key::F4), "F4");
        chai.add_global_const(chaiscript::const_var(Key::F5), "F5");
        chai.add_global_const(chaiscript::const_var(Key::F6), "F6");
        chai.add_global_const(chaiscript::const_var(Key::F7), "F7");
        chai.add_global_const(chaiscript::const_var(Key::F8), "F8");
        chai.add_global_const(chaiscript::const_var(Key::F9), "F9");
        chai.add_global_const(chaiscript::const_var(Key::F10), "F10");
        chai.add_global_const(chaiscript::const_var(Key::F11), "F11");
        chai.add_global_const(chaiscript::const_var(Key::F12), "F12");
        chai.add_global_const(chaiscript::const_var(Key::PRINTSCREEN), "PRINTSCREEN");
        chai.add_global_const(chaiscript::const_var(Key::SCROLLLOCK), "SCROLLLOCK");
        chai.add_global_const(chaiscript::const_var(Key::PAUSE), "PAUSE");
        chai.add_global_const(chaiscript::const_var(Key::INSERT), "INSERT");
        chai.add_global_const(chaiscript::const_var(Key::HOME), "HOME");
        chai.add_global_const(chaiscript::const_var(Key::PAGEUP), "PAGEUP");
        chai.add_global_const(chaiscript::const_var(Key::DELETE), "DELETE");
        chai.add_global_const(chaiscript::const_var(Key::END), "END");
        chai.add_global_const(chaiscript::const_var(Key::PAGEDOWN), "PAGEDOWN");
        chai.add_global_const(chaiscript::const_var(Key::RIGHT), "RIGHT");
        chai.add_global_const(chaiscript::const_var(Key::LEFT), "LEFT");
        chai.add_global_const(chaiscript::const_var(Key::DOWN), "DOWN");
        chai.add_global_const(chaiscript::const_var(Key::UP), "UP");
        chai.add_global_const(chaiscript::const_var(Key::NUMLOCKCLEAR), "NUMLOCKCLEAR");
        chai.add_global_const(chaiscript::const_var(Key::KP_DIVIDE), "KP_DIVIDE");
        chai.add_global_const(chaiscript::const_var(Key::KP_MULTIPLY), "KP_MULTIPLY");
        chai.add_global_const(chaiscript::const_var(Key::KP_MINUS), "KP_MINUS");
        chai.add_global_const(chaiscript::const_var(Key::KP_PLUS), "KP_PLUS");
        chai.add_global_const(chaiscript::const_var(Key::KP_ENTER), "KP_ENTER");
        chai.add_global_const(chaiscript::const_var(Key::KP_1), "KP_1");
        chai.add_global_const(chaiscript::const_var(Key::KP_2), "KP_2");
        chai.add_global_const(chaiscript::const_var(Key::KP_3), "KP_3");
        chai.add_global_const(chaiscript::const_var(Key::KP_4), "KP_4");
        chai.add_global_const(chaiscript::const_var(Key::KP_5), "KP_5");
        chai.add_global_const(chaiscript::const_var(Key::KP_6), "KP_6");
        chai.add_global_const(chaiscript::const_var(Key::KP_7), "KP_7");
        chai.add_global_const(chaiscript::const_var(Key::KP_8), "KP_8");
        chai.add_global_const(chaiscript::const_var(Key::KP_9), "KP_9");
        chai.add_global_const(chaiscript::const_var(Key::KP_0), "KP_0");
        chai.add_global_const(chaiscript::const_var(Key::KP_PERIOD), "KP_PERIOD");
        chai.add_global_const(chaiscript::const_var(Key::NONUSBACKSLASH), "NONUSBACKSLASH");
        chai.add_global_const(chaiscript::const_var(Key::APPLICATION), "APPLICATION");
        chai.add_global_const(chaiscript::const_var(Key::POWER), "POWER");
        chai.add_global_const(chaiscript::const_var(Key::KP_EQUALS), "KP_EQUALS");
        chai.add_global_const(chaiscript::const_var(Key::F13), "F13");
        chai.add_global_const(chaiscript::const_var(Key::F14), "F14");
        chai.add_global_const(chaiscript::const_var(Key::F15), "F15");
        chai.add_global_const(chaiscript::const_var(Key::F16), "F16");
        chai.add_global_const(chaiscript::const_var(Key::F17), "F17");
        chai.add_global_const(chaiscript::const_var(Key::F18), "F18");
        chai.add_global_const(chaiscript::const_var(Key::F19), "F19");
        chai.add_global_const(chaiscript::const_var(Key::F20), "F20");
        chai.add_global_const(chaiscript::const_var(Key::F21), "F21");
        chai.add_global_const(chaiscript::const_var(Key::F22), "F22");
        chai.add_global_const(chaiscript::const_var(Key::F23), "F23");
        chai.add_global_const(chaiscript::const_var(Key::F24), "F24");
        chai.add_global_const(chaiscript::const_var(Key::EXECUTE), "EXECUTE");
        chai.add_global_const(chaiscript::const_var(Key::HELP), "HELP");
        chai.add_global_const(chaiscript::const_var(Key::MENU), "MENU");
        chai.add_global_const(chaiscript::const_var(Key::SELECT), "SELECT");
        chai.add_global_const(chaiscript::const_var(Key::STOP), "STOP");
        chai.add_global_const(chaiscript::const_var(Key::AGAIN), "AGAIN");
        chai.add_global_const(chaiscript::const_var(Key::UNDO), "UNDO");
        chai.add_global_const(chaiscript::const_var(Key::CUT), "CUT");
        chai.add_global_const(chaiscript::const_var(Key::COPY), "COPY");
        chai.add_global_const(chaiscript::const_var(Key::PASTE), "PASTE");
        chai.add_global_const(chaiscript::const_var(Key::FIND), "FIND");
        chai.add_global_const(chaiscript::const_var(Key::MUTE), "MUTE");
        chai.add_global_const(chaiscript::const_var(Key::VOLUMEUP), "VOLUMEUP");
        chai.add_global_const(chaiscript::const_var(Key::VOLUMEDOWN), "VOLUMEDOWN");
        chai.add_global_const(chaiscript::const_var(Key::KP_COMMA), "KP_COMMA");
        chai.add_global_const(chaiscript::const_var(Key::KP_EQUALSAS400), "KP_EQUALSAS400");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL1), "INTERNATIONAL1");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL2), "INTERNATIONAL2");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL3), "INTERNATIONAL3");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL4), "INTERNATIONAL4");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL5), "INTERNATIONAL5");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL6), "INTERNATIONAL6");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL7), "INTERNATIONAL7");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL8), "INTERNATIONAL8");
        chai.add_global_const(chaiscript::const_var(Key::INTERNATIONAL9), "INTERNATIONAL9");
        chai.add_global_const(chaiscript::const_var(Key::LANG1), "LANG1");
        chai.add_global_const(chaiscript::const_var(Key::LANG2), "LANG2");
        chai.add_global_const(chaiscript::const_var(Key::LANG3), "LANG3");
        chai.add_global_const(chaiscript::const_var(Key::LANG4), "LANG4");
        chai.add_global_const(chaiscript::const_var(Key::LANG5), "LANG5");
        chai.add_global_const(chaiscript::const_var(Key::LANG6), "LANG6");
        chai.add_global_const(chaiscript::const_var(Key::LANG7), "LANG7");
        chai.add_global_const(chaiscript::const_var(Key::LANG8), "LANG8");
        chai.add_global_const(chaiscript::const_var(Key::LANG9), "LANG9");
        chai.add_global_const(chaiscript::const_var(Key::ALTERASE), "ALTERASE");
        chai.add_global_const(chaiscript::const_var(Key::SYSREQ), "SYSREQ");
        chai.add_global_const(chaiscript::const_var(Key::CANCEL), "CANCEL");
        chai.add_global_const(chaiscript::const_var(Key::CLEAR), "CLEAR");
        chai.add_global_const(chaiscript::const_var(Key::PRIOR), "PRIOR");
        chai.add_global_const(chaiscript::const_var(Key::RETURN2), "RETURN2");
        chai.add_global_const(chaiscript::const_var(Key::SEPARATOR), "SEPARATOR");
        chai.add_global_const(chaiscript::const_var(Key::OUT), "OUT");
        chai.add_global_const(chaiscript::const_var(Key::OPER), "OPER");
        chai.add_global_const(chaiscript::const_var(Key::CLEARAGAIN), "CLEARAGAIN");
        chai.add_global_const(chaiscript::const_var(Key::CRSEL), "CRSEL");
        chai.add_global_const(chaiscript::const_var(Key::EXSEL), "EXSEL");
        chai.add_global_const(chaiscript::const_var(Key::KP_00), "KP_00");
        chai.add_global_const(chaiscript::const_var(Key::KP_000), "KP_000");
        chai.add_global_const(chaiscript::const_var(Key::THOUSANDSSEPARATOR), "THOUSANDSSEPARATOR");
        chai.add_global_const(chaiscript::const_var(Key::DECIMALSEPARATOR), "DECIMALSEPARATOR");
        chai.add_global_const(chaiscript::const_var(Key::CURRENCYUNIT), "CURRENCYUNIT");
        chai.add_global_const(chaiscript::const_var(Key::CURRENCYSUBUNIT), "CURRENCYSUBUNIT");
        chai.add_global_const(chaiscript::const_var(Key::KP_LEFTPAREN), "KP_LEFTPAREN");
        chai.add_global_const(chaiscript::const_var(Key::KP_RIGHTPAREN), "KP_RIGHTPAREN");
        chai.add_global_const(chaiscript::const_var(Key::KP_LEFTBRACE), "KP_LEFTBRACE");
        chai.add_global_const(chaiscript::const_var(Key::KP_RIGHTBRACE), "KP_RIGHTBRACE");
        chai.add_global_const(chaiscript::const_var(Key::KP_TAB), "KP_TAB");
        chai.add_global_const(chaiscript::const_var(Key::KP_BACKSPACE), "KP_BACKSPACE");
        chai.add_global_const(chaiscript::const_var(Key::KP_A), "KP_A");
        chai.add_global_const(chaiscript::const_var(Key::KP_B), "KP_B");
        chai.add_global_const(chaiscript::const_var(Key::KP_C), "KP_C");
        chai.add_global_const(chaiscript::const_var(Key::KP_D), "KP_D");
        chai.add_global_const(chaiscript::const_var(Key::KP_E), "KP_E");
        chai.add_global_const(chaiscript::const_var(Key::KP_F), "KP_F");
        chai.add_global_const(chaiscript::const_var(Key::KP_XOR), "KP_XOR");
        chai.add_global_const(chaiscript::const_var(Key::KP_POWER), "KP_POWER");
        chai.add_global_const(chaiscript::const_var(Key::KP_PERCENT), "KP_PERCENT");
        chai.add_global_const(chaiscript::const_var(Key::KP_LESS), "KP_LESS");
        chai.add_global_const(chaiscript::const_var(Key::KP_GREATER), "KP_GREATER");
        chai.add_global_const(chaiscript::const_var(Key::KP_AMPERSAND), "KP_AMPERSAND");
        chai.add_global_const(chaiscript::const_var(Key::KP_DBLAMPERSAND), "KP_DBLAMPERSAND");
        chai.add_global_const(chaiscript::const_var(Key::KP_VERTICALBAR), "KP_VERTICALBAR");
        chai.add_global_const(chaiscript::const_var(Key::KP_DBLVERTICALBAR), "KP_DBLVERTICALBAR");
        chai.add_global_const(chaiscript::const_var(Key::KP_COLON), "KP_COLON");
        chai.add_global_const(chaiscript::const_var(Key::KP_HASH), "KP_HASH");
        chai.add_global_const(chaiscript::const_var(Key::KP_SPACE), "KP_SPACE");
        chai.add_global_const(chaiscript::const_var(Key::KP_AT), "KP_AT");
        chai.add_global_const(chaiscript::const_var(Key::KP_EXCLAM), "KP_EXCLAM");
        chai.add_global_const(chaiscript::const_var(Key::KP_MEMSTORE), "KP_MEMSTORE");
        chai.add_global_const(chaiscript::const_var(Key::KP_MEMRECALL), "KP_MEMRECALL");
        chai.add_global_const(chaiscript::const_var(Key::KP_MEMCLEAR), "KP_MEMCLEAR");
        chai.add_global_const(chaiscript::const_var(Key::KP_MEMADD), "KP_MEMADD");
        chai.add_global_const(chaiscript::const_var(Key::KP_MEMSUBTRACT), "KP_MEMSUBTRACT");
        chai.add_global_const(chaiscript::const_var(Key::KP_MEMMULTIPLY), "KP_MEMMULTIPLY");
        chai.add_global_const(chaiscript::const_var(Key::KP_MEMDIVIDE), "KP_MEMDIVIDE");
        chai.add_global_const(chaiscript::const_var(Key::KP_PLUSMINUS), "KP_PLUSMINUS");
        chai.add_global_const(chaiscript::const_var(Key::KP_CLEAR), "KP_CLEAR");
        chai.add_global_const(chaiscript::const_var(Key::KP_CLEARENTRY), "KP_CLEARENTRY");
        chai.add_global_const(chaiscript::const_var(Key::KP_BINARY), "KP_BINARY");
        chai.add_global_const(chaiscript::const_var(Key::KP_OCTAL), "KP_OCTAL");
        chai.add_global_const(chaiscript::const_var(Key::KP_DECIMAL), "KP_DECIMAL");
        chai.add_global_const(chaiscript::const_var(Key::KP_HEXADECIMAL), "KP_HEXADECIMAL");
        chai.add_global_const(chaiscript::const_var(Key::LCTRL), "LCTRL");
        chai.add_global_const(chaiscript::const_var(Key::LSHIFT), "LSHIFT");
        chai.add_global_const(chaiscript::const_var(Key::LALT), "LALT");
        chai.add_global_const(chaiscript::const_var(Key::LGUI), "LGUI");
        chai.add_global_const(chaiscript::const_var(Key::RCTRL), "RCTRL");
        chai.add_global_const(chaiscript::const_var(Key::RSHIFT), "RSHIFT");
        chai.add_global_const(chaiscript::const_var(Key::RALT), "RALT");
        chai.add_global_const(chaiscript::const_var(Key::RGUI), "RGUI");
        chai.add_global_const(chaiscript::const_var(Key::MODE), "MODE");
        chai.add_global_const(chaiscript::const_var(Key::AUDIONEXT), "AUDIONEXT");
        chai.add_global_const(chaiscript::const_var(Key::AUDIOPREV), "AUDIOPREV");
        chai.add_global_const(chaiscript::const_var(Key::AUDIOSTOP), "AUDIOSTOP");
        chai.add_global_const(chaiscript::const_var(Key::AUDIOPLAY), "AUDIOPLAY");
        chai.add_global_const(chaiscript::const_var(Key::AUDIOMUTE), "AUDIOMUTE");
        chai.add_global_const(chaiscript::const_var(Key::MEDIASELECT), "MEDIASELECT");
        chai.add_global_const(chaiscript::const_var(Key::WWW), "WWW");
        chai.add_global_const(chaiscript::const_var(Key::MAIL), "MAIL");
        chai.add_global_const(chaiscript::const_var(Key::CALCULATOR), "CALCULATOR");
        chai.add_global_const(chaiscript::const_var(Key::COMPUTER), "COMPUTER");
        chai.add_global_const(chaiscript::const_var(Key::AC_SEARCH), "AC_SEARCH");
        chai.add_global_const(chaiscript::const_var(Key::AC_HOME), "AC_HOME");
        chai.add_global_const(chaiscript::const_var(Key::AC_BACK), "AC_BACK");
        chai.add_global_const(chaiscript::const_var(Key::AC_FORWARD), "AC_FORWARD");
        chai.add_global_const(chaiscript::const_var(Key::AC_STOP), "AC_STOP");
        chai.add_global_const(chaiscript::const_var(Key::AC_REFRESH), "AC_REFRESH");
        chai.add_global_const(chaiscript::const_var(Key::AC_BOOKMARKS), "AC_BOOKMARKS");
        chai.add_global_const(chaiscript::const_var(Key::BRIGHTNESSDOWN), "BRIGHTNESSDOWN");
        chai.add_global_const(chaiscript::const_var(Key::BRIGHTNESSUP), "BRIGHTNESSUP");
        chai.add_global_const(chaiscript::const_var(Key::DISPLAYSWITCH), "DISPLAYSWITCH");
        chai.add_global_const(chaiscript::const_var(Key::KBDILLUMTOGGLE), "KBDILLUMTOGGLE");
        chai.add_global_const(chaiscript::const_var(Key::KBDILLUMDOWN), "KBDILLUMDOWN");
        chai.add_global_const(chaiscript::const_var(Key::KBDILLUMUP), "KBDILLUMUP");
        chai.add_global_const(chaiscript::const_var(Key::EJECT), "EJECT");
        chai.add_global_const(chaiscript::const_var(Key::SLEEP), "SLEEP");
        chai.add_global_const(chaiscript::const_var(Key::APP1), "APP1");
        chai.add_global_const(chaiscript::const_var(Key::APP2), "APP2");
        chai.add_global_const(chaiscript::const_var(Key::AUDIOREWIND), "AUDIOREWIND");
        chai.add_global_const(chaiscript::const_var(Key::AUDIOFASTFORWARD), "AUDIOFASTFORWARD");
        chai.add_global_const(chaiscript::const_var(Key::SOFTLEFT), "SOFTLEFT");
        chai.add_global_const(chaiscript::const_var(Key::SOFTRIGHT), "SOFTRIGHT");
        chai.add_global_const(chaiscript::const_var(Key::CALL), "CALL");
        chai.add_global_const(chaiscript::const_var(Key::ENDCALL), "ENDCALL");
        chai.add_global_const(chaiscript::const_var(Key::NUM_SCANCODES), "NUM_SCANCODES");

        chai.eval_file(path);
    }

    void ChaiScript::OnLoad()
    {
        std::function<void()> onLoad = chai.eval<std::function<void()>>("onLoad");

        onLoad();
    }

    void ChaiScript::OnTick(float dt)
    {
        std::function<void(float)> onTick = chai.eval<std::function<void(float)>>("onTick");

        onTick(dt);
    }

    void ChaiScript::OnRender()
    {
        std::function<void()> onRender = chai.eval<std::function<void()>>("onRender");

        onRender();
    }

    void ChaiScript::OnImgui()
    {
        std::function<void()> onImgui = chai.eval<std::function<void()>>("onImgui");

        onImgui();
    }
}