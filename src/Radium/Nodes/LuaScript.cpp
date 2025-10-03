#include <Radium/Nodes/LuaScript.hpp>
#include <spdlog/spdlog.h>
#include <Radium/Math.hpp>
#include <Radium/Random.hpp>

namespace Radium::Nodes
{
    LuaScript::~LuaScript()
    {
        if (L)
        {
            lua_close(L);
            L = nullptr;
        }
    }

    int classdb_class_wrap_new(lua_State *L, void* obj);

    int classdb_class_wrap_index(lua_State *L)
    {
        return 0;
    }

    int classdb_class_wrap_new_index(lua_State *L)
    {
       return 0;
    }

    int classdb_class_wrap_new(lua_State *L, void *obj)
    {
        return 0;
    }

    int lua_script_get_me(lua_State *L)
    {
        Script *script = static_cast<Script *>(lua_touserdata(L, 1));
        if (script && script->me)
        {
            //classdb_class_wrap_new(L, static_cast<Radium::Nodes::Object *>(script->me));
        }
        else
        {
            lua_pushnil(L);
        }
        return 1;
    }

    int lua_log_info(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::info("[Lua] {}", message);
        return 0;
    }

    int lua_log_error(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::error("[Lua] {}", message);
        return 0;
    }

    int lua_log_warn(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::warn("[Lua] {}", message);
        return 0;
    }

    int lua_log_debug(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::debug("[Lua] {}", message);
        return 0;
    }

    int lua_log_trace(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::trace("[Lua] {}", message);
        return 0;
    }

    int lua_random_float_between(lua_State *L)
    {
        float min = static_cast<float>(luaL_checknumber(L, 1));
        float max = static_cast<float>(luaL_checknumber(L, 2));
        float value = Radium::Random::FloatBetween(min, max);
        lua_pushnumber(L, value);
        return 1;
    }

    int lua_random_between(lua_State *L)
    {
        int min = static_cast<int>(luaL_checkinteger(L, 1));
        int max = static_cast<int>(luaL_checkinteger(L, 2));
        int value = Radium::Random::IntBetween(min, max);
        lua_pushinteger(L, value);
        return 1;
    }

    static const luaL_Reg radium_script[] = {
        {"me", lua_script_get_me},
        {NULL, NULL}};

    static const luaL_Reg radium_log[] = {
        {"info", lua_log_info},
        {"error", lua_log_error},
        {"warn", lua_log_warn},
        {"debug", lua_log_debug},
        {"trace", lua_log_trace},
        {NULL, NULL}};

    static const luaL_Reg radium_random[] = {
        {"float_between", lua_random_float_between},
        {"int_between", lua_random_between},
        {NULL, NULL}};

    int luaopen_radium_classdb(lua_State *L)
    {
        //luaL_newlib(L, radium_classdb);
        return 1;
    }

    int luaopen_radium_script(lua_State *L)
    {
        luaL_newlib(L, radium_script);
        return 1;
    }

    int luaopen_radium_log(lua_State *L)
    {
        luaL_newlib(L, radium_log);
        return 1;
    }

    int luaopen_radium_random(lua_State *L)
    {
        luaL_newlib(L, radium_random);
        return 1;
    }

    LuaScript::LuaScript(const std::string &scriptPath) : scriptPath(scriptPath)
    {
        L = luaL_newstate();
        if (L)
        {
            luaL_openlibs(L);
            // Get 'package.preload' table
            lua_getglobal(L, "package");
            lua_getfield(L, -1, "preload");

            // Stack now: ..., package, preload

            // Add 'radium_classdb' to package.preload
            lua_pushcfunction(L, luaopen_radium_classdb);
            lua_setfield(L, -2, "radium::classdb");

            // Add 'radium_script' to package.preload
            lua_pushcfunction(L, luaopen_radium_script);
            lua_setfield(L, -2, "radium::script");

            // Add 'radium_log' to package.preload
            lua_pushcfunction(L, luaopen_radium_log);
            lua_setfield(L, -2, "radium::log");

            // Add 'radium_random' to package.preload
            lua_pushcfunction(L, luaopen_radium_random);
            lua_setfield(L, -2, "radium::random");

            // Pop 'preload' and 'package' off the stack
            lua_pop(L, 2);

            lua_pushlightuserdata(L, this);
            lua_setglobal(L, "this_script");

            spdlog::info("Loading Lua script: {}", scriptPath);

            if (luaL_dofile(L, scriptPath.c_str()) != LUA_OK)
            {
                spdlog::error("Error loading Lua script: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
            else
            {
                spdlog::info("Lua script loaded successfully.");
            }
        }
        else
        {
            spdlog::error("Failed to create Lua state.");
        }
    }

    void LuaScript::OnLoad()
    {
        if (loaded || !L)
            return;
        lua_getglobal(L, "OnLoad");
        if (lua_isfunction(L, -1))
        {
            int result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                const char *error = lua_tostring(L, -1);
                // handle or log the error
                spdlog::error("Lua error: {}", error);

                lua_pop(L, 1); // remove error message from stack
            }
        }
        else
        {
            lua_pop(L, 1);
        }
        loaded = true;
    }

    void LuaScript::OnTick(float dt)
    {

        if (!L)
            return;
        lua_getglobal(L, "OnTick");
        if (lua_isfunction(L, -1))
        {
            lua_pushnumber(L, dt);
            int result = lua_pcall(L, 1, 0, 0);
            if (result != LUA_OK)
            {
                const char *error = lua_tostring(L, -1);
                // handle or log the error
                spdlog::error("Lua error: {}", error);

                lua_pop(L, 1); // remove error message from stack
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    void LuaScript::OnRender()
    {
        if (!L)
            return;
        lua_getglobal(L, "OnRender");
        if (lua_isfunction(L, -1))
        {
            int result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                const char *error = lua_tostring(L, -1);
                // handle or log the error
                spdlog::error("Lua error: {}", error);

                lua_pop(L, 1); // remove error message from stack
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    void LuaScript::OnImgui()
    {
        if (!L)
            return;
        lua_getglobal(L, "OnImgui");
        if (lua_isfunction(L, -1))
        {
            int result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                const char *error = lua_tostring(L, -1);
                // handle or log the error
                spdlog::error("Lua error: {}", error);

                lua_pop(L, 1); // remove error message from stack
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    void LuaScript::SetGlobalInteger(const std::string &name, int value)
    {
        if (!L)
            return;
        lua_pushinteger(L, value);
        lua_setglobal(L, name.c_str());
    }

    void LuaScript::SetGlobalNumber(const std::string &name, float value)
    {
        if (!L)
            return;
        lua_pushnumber(L, value);
        lua_setglobal(L, name.c_str());
    }

    void LuaScript::SetGlobalBoolean(const std::string &name, bool value)
    {
        if (!L)
            return;
        lua_pushboolean(L, value);
        lua_setglobal(L, name.c_str());
    }

    void LuaScript::SetGlobalString(const std::string &name, const std::string &value)
    {
        if (!L)
            return;
        lua_pushstring(L, value.c_str());
        lua_setglobal(L, name.c_str());
    }
}