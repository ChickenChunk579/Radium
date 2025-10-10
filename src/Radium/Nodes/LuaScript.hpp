#pragma once
#include <Radium/Nodes/Script.hpp>
#include <Radium/Nodes/Tree.hpp>
#include <string>
#include <memory>
#include <unordered_map>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#define LUA_FUNC(path, fn) Radium::Nodes::Lua::luaFuncs[path] = fn;
#define LUA_CONSTRUCTOR(path, fn) Radium::Nodes::Lua::luaConstructors[path] = fn;

namespace Radium::Nodes {
    struct IntRef {
        int* ptr;
    };

    struct FloatRef {
        float* ptr;
    };

    struct BoolRef {
        bool* ptr;
    };

    struct StringRef {
        std::string* ptr;
    };

    int classdb_lua_wrap(lua_State *L, Radium::Nodes::Object *obj);

    namespace Lua {
        extern std::unordered_map<std::string, lua_CFunction> luaFuncs;
        extern std::unordered_map<std::string, lua_CFunction> luaConstructors;
    }

    class LuaScript: public Script {
    public:
        LuaScript(std::string path, SceneTree* tree, bool realLoad = true);
        ~LuaScript();

        void OnLoad() override;
        void OnTick(float dt) override;
        void OnRender() override;
        void OnImgui() override;

        std::string path;
        Node* GetMe();
        SceneTree* GetSceneTree();
        void SetMe(Node node);

    private:
        SceneTree* sceneTree;
        bool stubbed = false;
        lua_State* L;
        
        // Lua function wrappers
        static int lua_info(lua_State* L);
        static int lua_error(lua_State* L);
        static int lua_warn(lua_State* L);
        static int lua_debug(lua_State* L);
        static int lua_trace(lua_State* L);
    };
}