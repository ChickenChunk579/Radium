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

/**
 * @brief Exposes a function within a class to Lua
 * 
 * This macro automatically inserts the class into the luaFuncs map, registering it into the lua metatables.
 */
#define LUA_FUNC(path, fn) Radium::Nodes::Lua::luaFuncs[path] = fn;
/**
 * @brief Exposes a constructor of a class to Lua
 * 
 * This macro exposes a constructor function to Lua so a class can be constructed
 */
#define LUA_CONSTRUCTOR(path, fn) Radium::Nodes::Lua::luaConstructors[path] = fn;

namespace Radium::Nodes {
    /**
     * @brief Lua reference to an intager
     * 
     * Wraps an int* so Lua can seamlessly read and write to values in a class
     */
    struct IntRef {
        int* ptr;
    };

    /**
     * @brief Lua reference to an float
     * 
     * Wraps an float* so Lua can seamlessly read and write to values in a class
     */
    struct FloatRef {
        float* ptr;
    };

    /**
     * @brief Lua reference to an bool
     * 
     * Wraps an bool* so Lua can seamlessly read and write to values in a class
     */
    struct BoolRef {
        bool* ptr;
    };

    /**
     * @brief Lua reference to an string
     * 
     * Wraps an std::string* so Lua can seamlessly read and write to values in a class
     */
    struct StringRef {
        std::string* ptr;
    };

    float get_float_arg(lua_State* L, int index);
    std::string get_string_arg(lua_State* L, int index);

    /**
     * @brief Wrap a ClassDB class for use in Lua
     * 
     * @param L Lua state.
     * @param obj A pointer to a class inheriting Object
     * 
     * Pushes a ClassDB metatable to the stack wrapping the given object.
     
        @return int Number of results pushed to the Lua stack
     */
    int classdb_lua_wrap(lua_State *L, Radium::Nodes::Object *obj);

    namespace Lua {
        /**
         * @brief Container for all registered functions
         * 
         * Holds the functions for all classes to be later exposed to Lua
         */
        extern std::unordered_map<std::string, lua_CFunction> luaFuncs;
        /**
         * @brief Container for all registered constructors
         * 
         * Holds the constructors for all classes to be later exposed to Lua
         */
        extern std::unordered_map<std::string, lua_CFunction> luaConstructors;
    }

    /**
     * @brief Wraps Lua scripts so they can be used on nodes
     * 
     * Loads, manages and executes Lua scripts on a node.
     */
    class LuaScript: public Script {
    public:
        /**
         * @brief Construct a LuaScript from a path and a tree
         * 
         * Constructs a LuaScript by loading the file from path, and executing it
         * 
         * @param path Path to a lua script
         * @param tree Scene tree to load the script into
         * @param realLoad Whether or not to actually load it (used in editor)
         */
        LuaScript(std::string path, SceneTree* tree, bool realLoad = true);
        ~LuaScript();

        /**
         * @brief Call Lua Onload on load
         * 
         * Wraps around the lua function to call it and use it
         */
        void OnLoad() override;
        /**
         * @brief Call Lua OnTick to run every tick.
         * 
         * Wraps around the lua function to call it and use it
         * 
         * @param dt Delta time
         */
        void OnTick(float dt) override;
        /**
         * @brief Call Lua OnRender every render
         * 
         * Wraps around the lua function to call it and use it
         */
        void OnRender() override;
        /**
         * @brief Call Lua OnImgui every imgui render
         * 
         * Wraps around the lua function to call it and use it
         */
        void OnImgui() override;

        /**
         * Path to the script
         */
        std::string path;
        /**
         * @brief Get the node the script is attached to
         * 
         * Helper function to expose the node to the lua script
         */
        Node* GetMe();
        /**
         * @brief Get the scene tree the script is attached to
         * 
         * Helper function to expose the scene tree to the lua script
         */
        SceneTree* GetSceneTree();
        /**
         * @brief Change the node the script is attached to
         * 
         * Helper function to change the node the script is attached to
         */
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