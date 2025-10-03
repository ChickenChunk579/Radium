#pragma once

#include <Radium/Nodes/Script.hpp>
#include <Radium/Nodes/Node.hpp>
#include <string>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace Radium::Nodes {
    class LuaScript : public Script {
    public:
        LuaScript(const std::string& scriptPath);
        ~LuaScript();

        void OnLoad() override;
        void OnTick(float dt) override;
        void OnRender() override;
        void OnImgui() override;

        void SetGlobalInteger(const std::string& name, int value);
        void SetGlobalNumber(const std::string& name, float value);
        void SetGlobalBoolean(const std::string& name, bool value);
        void SetGlobalString(const std::string& name, const std::string& value);

    private:
        lua_State* L = nullptr;
        std::string scriptPath;
        bool loaded = false;
    };
};