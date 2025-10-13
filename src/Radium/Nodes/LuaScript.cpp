#include <Radium/Nodes/LuaScript.hpp>
#include <Radium/Nodes/Node.hpp>
#include <Radium/Random.hpp>
#include <Radium/PixelScaleUtil.hpp>
#include <Radium/AssetLoader.hpp>
#include <Radium/DebugRenderer.hpp>
#include <Radium/Key.hpp>
#include <Radium/Input.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>

namespace Radium::Nodes
{
    namespace Lua
    {
        std::unordered_map<std::string, lua_CFunction> luaFuncs;
        std::unordered_map<std::string, lua_CFunction> luaConstructors;
    }

    float get_float_arg(lua_State *L, int index)
    {
        int type = lua_type(L, index);

        switch (type)
        {
        case LUA_TNUMBER:
            return static_cast<float>(lua_tonumber(L, index));

        case LUA_TUSERDATA:
        {
            // Try IntRef
            if (luaL_testudata(L, index, "IntRef"))
            {
                IntRef *ref = static_cast<IntRef *>(luaL_checkudata(L, index, "IntRef"));
                return static_cast<float>(*ref->ptr);
            }
            // Try FloatRef
            if (luaL_testudata(L, index, "FloatRef"))
            {
                FloatRef *ref = static_cast<FloatRef *>(luaL_checkudata(L, index, "FloatRef"));
                return *ref->ptr;
            }
            break;
        }

        default:
            break;
        }

        luaL_error(L, "Expected number or IntRef/FloatRef at argument %d", index);
        return 0.0f; // Never reached
    }

    std::string get_string_arg(lua_State *L, int index)
    {
        int type = lua_type(L, index);

        switch (type)
        {
        case LUA_TSTRING:
            return std::string(lua_tostring(L, index));

        case LUA_TUSERDATA:
        {
            // Try StringRef
            if (luaL_testudata(L, index, "StringRef"))
            {
                StringRef *ref = static_cast<StringRef *>(luaL_checkudata(L, index, "StringRef"));
                return *ref->ptr;
            }
            break;
        }

        default:
            break;
        }

        luaL_error(L, "Expected string or StringRef at argument %d", index);
        return ""; // Never reached
    }

    int floatref_lt(lua_State *L)
    {
        float val1, val2;

        // Argument 1: FloatRef or number
        if (luaL_testudata(L, 1, "FloatRef"))
        {
            FloatRef *a = static_cast<FloatRef *>(luaL_checkudata(L, 1, "FloatRef"));
            val1 = *a->ptr;
        }
        else if (lua_isnumber(L, 1))
        {
            val1 = static_cast<float>(lua_tonumber(L, 1));
        }
        else
        {
            return luaL_error(L, "Expected FloatRef or number as first argument");
        }

        // Argument 2: FloatRef or number
        if (luaL_testudata(L, 2, "FloatRef"))
        {
            FloatRef *b = static_cast<FloatRef *>(luaL_checkudata(L, 2, "FloatRef"));
            val2 = *b->ptr;
        }
        else if (lua_isnumber(L, 2))
        {
            val2 = static_cast<float>(lua_tonumber(L, 2));
        }
        else
        {
            return luaL_error(L, "Expected FloatRef or number as second argument");
        }

        lua_pushboolean(L, val1 < val2);
        return 1;
    }

    int floatref_le(lua_State *L)
    {
        float val1, val2;

        if (luaL_testudata(L, 1, "FloatRef"))
        {
            FloatRef *a = static_cast<FloatRef *>(luaL_checkudata(L, 1, "FloatRef"));
            val1 = *a->ptr;
        }
        else if (lua_isnumber(L, 1))
        {
            val1 = static_cast<float>(lua_tonumber(L, 1));
        }
        else
        {
            return luaL_error(L, "Expected FloatRef or number as first argument");
        }

        if (luaL_testudata(L, 2, "FloatRef"))
        {
            FloatRef *b = static_cast<FloatRef *>(luaL_checkudata(L, 2, "FloatRef"));
            val2 = *b->ptr;
        }
        else if (lua_isnumber(L, 2))
        {
            val2 = static_cast<float>(lua_tonumber(L, 2));
        }
        else
        {
            return luaL_error(L, "Expected FloatRef or number as second argument");
        }

        lua_pushboolean(L, val1 <= val2);
        return 1;
    }

    int intref_lt(lua_State *L)
    {
        int val1, val2;

        if (luaL_testudata(L, 1, "IntRef"))
        {
            IntRef *a = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
            val1 = *a->ptr;
        }
        else if (lua_isinteger(L, 1))
        {
            val1 = static_cast<int>(lua_tointeger(L, 1));
        }
        else
        {
            return luaL_error(L, "Expected IntRef or integer as first argument");
        }

        if (luaL_testudata(L, 2, "IntRef"))
        {
            IntRef *b = static_cast<IntRef *>(luaL_checkudata(L, 2, "IntRef"));
            val2 = *b->ptr;
        }
        else if (lua_isinteger(L, 2))
        {
            val2 = static_cast<int>(lua_tointeger(L, 2));
        }
        else
        {
            return luaL_error(L, "Expected IntRef or integer as second argument");
        }

        lua_pushboolean(L, val1 < val2);
        return 1;
    }

    int intref_le(lua_State *L)
    {
        int val1, val2;

        if (luaL_testudata(L, 1, "IntRef"))
        {
            IntRef *a = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
            val1 = *a->ptr;
        }
        else if (lua_isinteger(L, 1))
        {
            val1 = static_cast<int>(lua_tointeger(L, 1));
        }
        else
        {
            return luaL_error(L, "Expected IntRef or integer as first argument");
        }

        if (luaL_testudata(L, 2, "IntRef"))
        {
            IntRef *b = static_cast<IntRef *>(luaL_checkudata(L, 2, "IntRef"));
            val2 = *b->ptr;
        }
        else if (lua_isinteger(L, 2))
        {
            val2 = static_cast<int>(lua_tointeger(L, 2));
        }
        else
        {
            return luaL_error(L, "Expected IntRef or integer as second argument");
        }

        lua_pushboolean(L, val1 <= val2);
        return 1;
    }

    int boolref_eq(lua_State *L)
    {
        BoolRef *a = static_cast<BoolRef *>(luaL_testudata(L, 1, "BoolRef"));
        BoolRef *b = static_cast<BoolRef *>(luaL_testudata(L, 2, "BoolRef"));

        bool val1 = a ? *a->ptr : lua_toboolean(L, 1);
        bool val2 = b ? *b->ptr : lua_toboolean(L, 2);

        lua_pushboolean(L, val1 == val2);
        return 1;
    }

    int floatref_add(lua_State *L)
    {
        FloatRef *a = static_cast<FloatRef *>(luaL_testudata(L, 1, "FloatRef"));
        FloatRef *b = static_cast<FloatRef *>(luaL_testudata(L, 2, "FloatRef"));

        float val1 = a ? *a->ptr : static_cast<float>(luaL_checknumber(L, 1));
        float val2 = b ? *b->ptr : static_cast<float>(luaL_checknumber(L, 2));

        lua_pushnumber(L, val1 + val2);
        return 1;
    }

    int floatref_sub(lua_State *L)
    {
        FloatRef *a = static_cast<FloatRef *>(luaL_testudata(L, 1, "FloatRef"));
        FloatRef *b = static_cast<FloatRef *>(luaL_testudata(L, 2, "FloatRef"));

        float val1 = a ? *a->ptr : static_cast<float>(luaL_checknumber(L, 1));
        float val2 = b ? *b->ptr : static_cast<float>(luaL_checknumber(L, 2));

        lua_pushnumber(L, val1 - val2);
        return 1;
    }

    int floatref_mul(lua_State *L)
    {
        FloatRef *a = static_cast<FloatRef *>(luaL_testudata(L, 1, "FloatRef"));
        FloatRef *b = static_cast<FloatRef *>(luaL_testudata(L, 2, "FloatRef"));

        float val1 = a ? *a->ptr : static_cast<float>(luaL_checknumber(L, 1));
        float val2 = b ? *b->ptr : static_cast<float>(luaL_checknumber(L, 2));

        lua_pushnumber(L, val1 * val2);
        return 1;
    }

    int floatref_div(lua_State *L)
    {
        FloatRef *a = static_cast<FloatRef *>(luaL_testudata(L, 1, "FloatRef"));
        FloatRef *b = static_cast<FloatRef *>(luaL_testudata(L, 2, "FloatRef"));

        float val1 = a ? *a->ptr : static_cast<float>(luaL_checknumber(L, 1));
        float val2 = b ? *b->ptr : static_cast<float>(luaL_checknumber(L, 2));

        if (val2 == 0.0f)
        {
            return luaL_error(L, "Division by zero");
        }

        lua_pushnumber(L, val1 / val2);
        return 1;
    }

    int floatref_eq(lua_State *L)
    {
        FloatRef *a = static_cast<FloatRef *>(luaL_testudata(L, 1, "FloatRef"));
        FloatRef *b = static_cast<FloatRef *>(luaL_testudata(L, 2, "FloatRef"));

        float val1 = a ? *a->ptr : static_cast<float>(luaL_checknumber(L, 1));
        float val2 = b ? *b->ptr : static_cast<float>(luaL_checknumber(L, 2));

        lua_pushboolean(L, val1 == val2);
        return 1;
    }

    int intref_add(lua_State *L)
    {
        IntRef *a = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        IntRef *b = static_cast<IntRef *>(luaL_checkudata(L, 2, "IntRef"));
        lua_pushinteger(L, *a->ptr + *b->ptr);
        return 1;
    }

    int intref_sub(lua_State *L)
    {
        IntRef *a = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        IntRef *b = static_cast<IntRef *>(luaL_checkudata(L, 2, "IntRef"));
        lua_pushinteger(L, *a->ptr - *b->ptr);
        return 1;
    }

    int intref_mul(lua_State *L)
    {
        IntRef *a = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        IntRef *b = static_cast<IntRef *>(luaL_checkudata(L, 2, "IntRef"));
        lua_pushinteger(L, *a->ptr * *b->ptr);
        return 1;
    }

    int intref_div(lua_State *L)
    {
        IntRef *a = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        IntRef *b = static_cast<IntRef *>(luaL_checkudata(L, 2, "IntRef"));
        if (*b->ptr == 0)
        {
            return luaL_error(L, "Division by zero");
        }
        lua_pushinteger(L, *a->ptr / *b->ptr);
        return 1;
    }

    int intref_eq(lua_State *L)
    {
        IntRef *a = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        IntRef *b = static_cast<IntRef *>(luaL_checkudata(L, 2, "IntRef"));
        lua_pushboolean(L, *a->ptr == *b->ptr);
        return 1;
    }

    int intref_tostring(lua_State *L)
    {
        IntRef *ref = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        lua_pushfstring(L, "%d", *ref->ptr);
        return 1;
    }

    int floatref_tostring(lua_State *L)
    {
        FloatRef *ref = static_cast<FloatRef *>(luaL_checkudata(L, 1, "FloatRef"));
        lua_pushfstring(L, "%f", *ref->ptr);
        return 1;
    }

    int boolref_tostring(lua_State *L)
    {
        BoolRef *ref = static_cast<BoolRef *>(luaL_checkudata(L, 1, "BoolRef"));
        lua_pushstring(L, *ref->ptr ? "true" : "false");
        return 1;
    }

    int stringref_tostring(lua_State *L)
    {
        StringRef *ref = static_cast<StringRef *>(luaL_checkudata(L, 1, "StringRef"));
        lua_pushfstring(L, "%s", ref->ptr->c_str());
        return 1;
    }

    int intref_index(lua_State *L)
    {
        IntRef *ref = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        lua_pushinteger(L, *ref->ptr);
        return 1;
    }

    int intref_newindex(lua_State *L)
    {
        IntRef *ref = static_cast<IntRef *>(luaL_checkudata(L, 1, "IntRef"));
        *ref->ptr = luaL_checkinteger(L, 2);
        return 0;
    }

    int floatref_index(lua_State *L)
    {
        FloatRef *ref = static_cast<FloatRef *>(luaL_checkudata(L, 1, "FloatRef"));
        lua_pushnumber(L, *ref->ptr);
        return 1;
    }

    int floatref_newindex(lua_State *L)
    {
        FloatRef *ref = static_cast<FloatRef *>(luaL_checkudata(L, 1, "FloatRef"));
        *ref->ptr = static_cast<float>(luaL_checknumber(L, 2));
        return 0;
    }

    int boolref_index(lua_State *L)
    {
        BoolRef *ref = static_cast<BoolRef *>(luaL_checkudata(L, 1, "BoolRef"));
        lua_pushboolean(L, *ref->ptr);
        return 1;
    }

    int stringref_index(lua_State *L)
    {
        StringRef *ref = static_cast<StringRef *>(luaL_checkudata(L, 1, "StringRef"));
        lua_pushstring(L, ref->ptr->c_str());
        return 1;
    }

    int stringref_newindex(lua_State *L)
    {
        StringRef *ref = static_cast<StringRef *>(luaL_checkudata(L, 1, "StringRef"));
        const char *val = luaL_checkstring(L, 2);
        *ref->ptr = val;
        return 0;
    }

    int boolref_newindex(lua_State *L)
    {
        BoolRef *ref = static_cast<BoolRef *>(luaL_checkudata(L, 1, "BoolRef"));
        *ref->ptr = lua_toboolean(L, 2);
        return 0;
    }

    void register_reference_metatables(lua_State *L)
    {
        if (luaL_newmetatable(L, "IntRef"))
        {
            lua_pushcfunction(L, intref_index);
            lua_setfield(L, -2, "__index");

            lua_pushcfunction(L, intref_newindex);
            lua_setfield(L, -2, "__newindex");

            lua_pushcfunction(L, intref_tostring);
            lua_setfield(L, -2, "__tostring");

            // Operators
            lua_pushcfunction(L, intref_add);
            lua_setfield(L, -2, "__add");

            lua_pushcfunction(L, intref_sub);
            lua_setfield(L, -2, "__sub");

            lua_pushcfunction(L, intref_mul);
            lua_setfield(L, -2, "__mul");

            lua_pushcfunction(L, intref_div);
            lua_setfield(L, -2, "__div");

            lua_pushcfunction(L, intref_eq);
            lua_setfield(L, -2, "__eq");

            lua_pushcfunction(L, intref_lt);
            lua_setfield(L, -2, "__lt");

            lua_pushcfunction(L, intref_le);
            lua_setfield(L, -2, "__le");
        }

        lua_pop(L, 1);

        if (luaL_newmetatable(L, "FloatRef"))
        {
            lua_pushcfunction(L, floatref_index);
            lua_setfield(L, -2, "__index");

            lua_pushcfunction(L, floatref_newindex);
            lua_setfield(L, -2, "__newindex");

            lua_pushcfunction(L, floatref_tostring);
            lua_setfield(L, -2, "__tostring");

            lua_pushcfunction(L, floatref_add);
            lua_setfield(L, -2, "__add");

            lua_pushcfunction(L, floatref_sub);
            lua_setfield(L, -2, "__sub");

            lua_pushcfunction(L, floatref_mul);
            lua_setfield(L, -2, "__mul");

            lua_pushcfunction(L, floatref_div);
            lua_setfield(L, -2, "__div");

            lua_pushcfunction(L, floatref_eq);
            lua_setfield(L, -2, "__eq");

            lua_pushcfunction(L, floatref_lt);
            lua_setfield(L, -2, "__lt");

            lua_pushcfunction(L, floatref_le);
            lua_setfield(L, -2, "__le");
        }

        lua_pop(L, 1);

        if (luaL_newmetatable(L, "BoolRef"))
        {
            lua_pushcfunction(L, boolref_index);
            lua_setfield(L, -2, "__index");

            lua_pushcfunction(L, boolref_newindex);
            lua_setfield(L, -2, "__newindex");

            lua_pushcfunction(L, boolref_tostring);
            lua_setfield(L, -2, "__tostring");

            lua_pushcfunction(L, boolref_eq);
            lua_setfield(L, -2, "__eq");
        }

        lua_pop(L, 1);

        if (luaL_newmetatable(L, "StringRef"))
        {
            lua_pushcfunction(L, stringref_index);
            lua_setfield(L, -2, "__index");

            lua_pushcfunction(L, stringref_newindex);
            lua_setfield(L, -2, "__newindex");

            lua_pushcfunction(L, stringref_tostring);
            lua_setfield(L, -2, "__tostring");
        }
        lua_pop(L, 1);
    }

    void push_integer_reference(lua_State *L, int *ptr)
    {
        auto *ref = static_cast<IntRef *>(lua_newuserdata(L, sizeof(IntRef)));
        ref->ptr = ptr;
        luaL_getmetatable(L, "IntRef");
        lua_setmetatable(L, -2);
    }

    void push_float_reference(lua_State *L, float *ptr)
    {
        auto *ref = static_cast<FloatRef *>(lua_newuserdata(L, sizeof(FloatRef)));
        ref->ptr = ptr;
        luaL_getmetatable(L, "FloatRef");
        lua_setmetatable(L, -2);
    }

    void push_bool_reference(lua_State *L, bool *ptr)
    {
        auto *ref = static_cast<BoolRef *>(lua_newuserdata(L, sizeof(BoolRef)));
        ref->ptr = ptr;
        luaL_getmetatable(L, "BoolRef");
        lua_setmetatable(L, -2);
    }

    void push_string_reference(lua_State *L, std::string *ptr)
    {
        auto *ref = static_cast<StringRef *>(lua_newuserdata(L, sizeof(StringRef)));
        ref->ptr = ptr;
        luaL_getmetatable(L, "StringRef");
        lua_setmetatable(L, -2);
    }

    int classdb_lua_newindex(lua_State *L)
    {
        Object **udata = static_cast<Object **>(lua_touserdata(L, 1));
        Object *obj = *udata;

        const char *key = luaL_checkstring(L, 2);
        if (!key || !obj)
            return luaL_error(L, "Invalid object or key");

        std::string prop_name = key;
        auto props = ClassDB::GetProperties(obj);

        for (const auto &prop : props)
        {
            if (prop.name == prop_name)
            {
                const std::string &type = prop.type;

                if (type == "int")
                {
                    int val = luaL_checkinteger(L, 3);
                    ClassDB::SetProperty<int>(prop.name, obj, val);
                    return 0;
                }
                else if (type == "float" || type == "double")
                {
                    float val = get_float_arg(L, 3);
                    ClassDB::SetProperty<float>(prop.name, obj, val);
                    return 0;
                }
                else if (type == "bool")
                {
                    bool val = lua_toboolean(L, 3);
                    ClassDB::SetProperty<bool>(prop.name, obj, val);
                    return 0;
                }
                else if (type == "std::string")
                {
                    const char *val = luaL_checkstring(L, 3);
                    ClassDB::SetProperty<std::string>(prop.name, obj, std::string(val));
                    return 0;
                }
            }
        }

        return luaL_error(L, "Property '%s' not found on object", key);
    }

    int classdb_lua_index(lua_State *L)
    {
        Radium::Nodes::Object **udata = static_cast<Radium::Nodes::Object **>(lua_touserdata(L, 1));
        Radium::Nodes::Object *obj = *udata;

        const char *key = luaL_checkstring(L, 2);

        if (!obj || !key)
        {
            lua_pushnil(L);
            return 1;
        }

        std::string prop_name = key;

        // 👇 SPECIAL CASE for __ptr
        if (prop_name == "__ptr")
        {
            lua_pushlightuserdata(L, obj);
            return 1;
        }

        // -------- Property lookup ----------
        auto props = Radium::Nodes::ClassDB::GetProperties(obj);

        for (const auto &prop : props)
        {
            if (prop.name == prop_name)
            {
                const std::string &type = prop.type;

                if (type == "int")
                {
                    auto *val = Radium::Nodes::ClassDB::GetPropertyPointer<int>(prop.name, obj);
                    push_integer_reference(L, val);
                    return 1;
                }
                else if (type == "float" || type == "double")
                {
                    auto *val = Radium::Nodes::ClassDB::GetPropertyPointer<float>(prop.name, obj);
                    push_float_reference(L, val);
                    return 1;
                }
                else if (type == "bool")
                {
                    auto *val = Radium::Nodes::ClassDB::GetPropertyPointer<bool>(prop.name, obj);
                    push_bool_reference(L, val);
                    return 1;
                }
                else if (type == "std::string")
                {
                    auto *val = Radium::Nodes::ClassDB::GetPropertyPointer<std::string>(prop.name, obj);
                    push_string_reference(L, val);
                    return 1;
                }
                else if (Radium::Nodes::ClassDB::IsEnum(type))
                {
                    auto *val = Radium::Nodes::ClassDB::GetPropertyPointer<int>(prop.name, obj);
                    push_integer_reference(L, val);
                    return 1;
                }
                else
                {
                    auto newObj = Radium::Nodes::ClassDB::GetPropertyAsObject(obj, prop.name);
                    return classdb_lua_wrap(L, newObj);
                }
            }
        }

        // -------- Method lookup with inheritance ----------
        ClassDB::ClassInfo classInfoR = ClassDB::GetClassInfo(obj);
        ClassDB::ClassInfo *classInfo = &classInfoR;

        while (classInfo)
        {
            std::string fullFuncName = classInfo->name + "::" + prop_name;

            auto it = Lua::luaFuncs.find(fullFuncName);
            if (it != Lua::luaFuncs.end())
            {
                lua_pushlightuserdata(L, obj); // upvalue 1: 'self'
                lua_pushcclosure(L, it->second, 1);
                return 1;
            }

            classInfo = classInfo->parent;
        }

        // Not found
        lua_pushnil(L);
        return 1;
    }

    int classdb_lua_wrap(lua_State *L, Radium::Nodes::Object *obj)
    {
        if (!obj)
        {
            lua_pushnil(L);
            return 1;
        }

        // Allocate userdata
        auto **udata = static_cast<Radium::Nodes::Object **>(
            lua_newuserdata(L, sizeof(Radium::Nodes::Object *)));
        *udata = obj;

        // Set the metatable
        if (luaL_newmetatable(L, "ClassDBObject"))
        {
            lua_pushcfunction(L, classdb_lua_index);
            lua_setfield(L, -2, "__index");

            lua_pushcfunction(L, classdb_lua_newindex);
            lua_setfield(L, -2, "__newindex");
        }

        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua wrapper functions for logging
    int LuaScript::lua_info(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::info("[Lua] {}", message);
        return 0;
    }

    int LuaScript::lua_error(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::error("[Lua] {}", message);
        return 0;
    }

    int LuaScript::lua_warn(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::warn("[Lua] {}", message);
        return 0;
    }

    int LuaScript::lua_debug(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::debug("[Lua] {}", message);
        return 0;
    }

    int LuaScript::lua_trace(lua_State *L)
    {
        const char *message = luaL_checkstring(L, 1);
        spdlog::trace("[Lua] {}", message);
        return 0;
    }

    int lua_get_me(lua_State *L)
    {
        LuaScript *script = (LuaScript *)lua_touserdata(L, 1);

        if (!script)
        {
            spdlog::warn("script doenst have me!");
            return 0;
        }

        classdb_lua_wrap(L, script->GetMe());

        return 1;
    }

    Node *LuaScript::GetMe()
    {
        return me;
    }

    SceneTree *LuaScript::GetSceneTree()
    {
        return sceneTree;
    }

    void LuaScript::SetMe(Node node)
    {
        // Implementation for setting the node reference
    }

    int lua_is_key_down(lua_State *L)
    {
        Key k = (Key)lua_tointeger(L, 1);

        bool keystate = Input::IsKeyDown(k);
        lua_pushboolean(L, keystate);
        return 1;
    }

    int lua_is_key_up(lua_State *L)
    {
        Key k = (Key)lua_tointeger(L, 1);

        bool keystate = Input::IsKeyDown(k);
        lua_pushboolean(L, keystate);
        return 1;
    }

    int lua_was_key_pressed_this_frame(lua_State *L)
    {
        Key k = (Key)lua_tointeger(L, 1);

        bool keystate = Input::WasKeyPressedThisFrame(k);
        lua_pushboolean(L, keystate);
        return 1;
    }

    LuaScript::LuaScript(std::string path, SceneTree *tree, bool realLoad)
    {
        this->path = path;
        this->sceneTree = tree;

        if (!realLoad)
        {
            stubbed = true;
            return;
        }

        // Initialize Lua state
        L = luaL_newstate();
        luaL_openlibs(L);

        // Register logging functions
        lua_register(L, "info", lua_info);
        lua_register(L, "error", lua_error);
        lua_register(L, "warn", lua_warn);
        lua_register(L, "debug", lua_debug);
        lua_register(L, "trace", lua_trace);
        lua_register(L, "randf_between", [](lua_State *L) -> int
                     {
            float min = lua_tonumber(L, 1);
            float max = lua_tonumber(L, 2);

            lua_pushnumber(L, Random::FloatBetween(min, max));
            return 1; });

        lua_register(L, "exit", [](lua_State *L) -> int
                     {
            exit(lua_tointeger(L, 1));
            
            return 0; });

        register_reference_metatables(L);

        lua_pushlightuserdata(L, this);
        lua_setglobal(L, "script");

        classdb_lua_wrap(L, tree);
        lua_setglobal(L, "tree");

        lua_register(L, "me", lua_get_me);

        lua_newtable(L);

        lua_pushstring(L, "SPACE");
        lua_pushinteger(L, (int)Key::KEY_SPACE);
        lua_settable(L, -3);
        lua_pushstring(L, "A");
        lua_pushinteger(L, (int)Key::KEY_A);
        lua_settable(L, -3);
        lua_pushstring(L, "B");
        lua_pushinteger(L, (int)Key::KEY_B);
        lua_settable(L, -3);
        lua_pushstring(L, "C");
        lua_pushinteger(L, (int)Key::KEY_C);
        lua_settable(L, -3);
        lua_pushstring(L, "D");
        lua_pushinteger(L, (int)Key::KEY_D);
        lua_settable(L, -3);
        lua_pushstring(L, "E");
        lua_pushinteger(L, (int)Key::KEY_E);
        lua_settable(L, -3);
        lua_pushstring(L, "F");
        lua_pushinteger(L, (int)Key::KEY_F);
        lua_settable(L, -3);
        lua_pushstring(L, "G");
        lua_pushinteger(L, (int)Key::KEY_G);
        lua_settable(L, -3);
        lua_pushstring(L, "H");
        lua_pushinteger(L, (int)Key::KEY_H);
        lua_settable(L, -3);
        lua_pushstring(L, "I");
        lua_pushinteger(L, (int)Key::KEY_I);
        lua_settable(L, -3);
        lua_pushstring(L, "J");
        lua_pushinteger(L, (int)Key::KEY_J);
        lua_settable(L, -3);
        lua_pushstring(L, "K");
        lua_pushinteger(L, (int)Key::KEY_K);
        lua_settable(L, -3);
        lua_pushstring(L, "L");
        lua_pushinteger(L, (int)Key::KEY_L);
        lua_settable(L, -3);
        lua_pushstring(L, "M");
        lua_pushinteger(L, (int)Key::KEY_M);
        lua_settable(L, -3);
        lua_pushstring(L, "N");
        lua_pushinteger(L, (int)Key::KEY_N);
        lua_settable(L, -3);
        lua_pushstring(L, "O");
        lua_pushinteger(L, (int)Key::KEY_O);
        lua_settable(L, -3);
        lua_pushstring(L, "P");
        lua_pushinteger(L, (int)Key::KEY_P);
        lua_settable(L, -3);
        lua_pushstring(L, "Q");
        lua_pushinteger(L, (int)Key::KEY_Q);
        lua_settable(L, -3);
        lua_pushstring(L, "R");
        lua_pushinteger(L, (int)Key::KEY_R);
        lua_settable(L, -3);
        lua_pushstring(L, "S");
        lua_pushinteger(L, (int)Key::KEY_S);
        lua_settable(L, -3);
        lua_pushstring(L, "T");
        lua_pushinteger(L, (int)Key::KEY_T);
        lua_settable(L, -3);
        lua_pushstring(L, "U");
        lua_pushinteger(L, (int)Key::KEY_U);
        lua_settable(L, -3);
        lua_pushstring(L, "V");
        lua_pushinteger(L, (int)Key::KEY_V);
        lua_settable(L, -3);
        lua_pushstring(L, "W");
        lua_pushinteger(L, (int)Key::KEY_W);
        lua_settable(L, -3);
        lua_pushstring(L, "X");
        lua_pushinteger(L, (int)Key::KEY_X);
        lua_settable(L, -3);
        lua_pushstring(L, "Y");
        lua_pushinteger(L, (int)Key::KEY_Y);
        lua_settable(L, -3);
        lua_pushstring(L, "Z");
        lua_pushinteger(L, (int)Key::KEY_Z);
        lua_settable(L, -3);

        lua_pushstring(L, "0");
        lua_pushinteger(L, (int)Key::KEY__0);
        lua_settable(L, -3);
        lua_pushstring(L, "1");
        lua_pushinteger(L, (int)Key::KEY__1);
        lua_settable(L, -3);
        lua_pushstring(L, "2");
        lua_pushinteger(L, (int)Key::KEY__2);
        lua_settable(L, -3);
        lua_pushstring(L, "3");
        lua_pushinteger(L, (int)Key::KEY__3);
        lua_settable(L, -3);
        lua_pushstring(L, "4");
        lua_pushinteger(L, (int)Key::KEY__4);
        lua_settable(L, -3);
        lua_pushstring(L, "5");
        lua_pushinteger(L, (int)Key::KEY__5);
        lua_settable(L, -3);
        lua_pushstring(L, "6");
        lua_pushinteger(L, (int)Key::KEY__6);
        lua_settable(L, -3);
        lua_pushstring(L, "7");
        lua_pushinteger(L, (int)Key::KEY__7);
        lua_settable(L, -3);
        lua_pushstring(L, "8");
        lua_pushinteger(L, (int)Key::KEY__8);
        lua_settable(L, -3);
        lua_pushstring(L, "9");
        lua_pushinteger(L, (int)Key::KEY__9);
        lua_settable(L, -3);

        lua_pushstring(L, "F1");
        lua_pushinteger(L, (int)Key::KEY_F1);
        lua_settable(L, -3);
        lua_pushstring(L, "F2");
        lua_pushinteger(L, (int)Key::KEY_F2);
        lua_settable(L, -3);
        lua_pushstring(L, "F3");
        lua_pushinteger(L, (int)Key::KEY_F3);
        lua_settable(L, -3);
        lua_pushstring(L, "F4");
        lua_pushinteger(L, (int)Key::KEY_F4);
        lua_settable(L, -3);
        lua_pushstring(L, "F5");
        lua_pushinteger(L, (int)Key::KEY_F5);
        lua_settable(L, -3);
        lua_pushstring(L, "F6");
        lua_pushinteger(L, (int)Key::KEY_F6);
        lua_settable(L, -3);
        lua_pushstring(L, "F7");
        lua_pushinteger(L, (int)Key::KEY_F7);
        lua_settable(L, -3);
        lua_pushstring(L, "F8");
        lua_pushinteger(L, (int)Key::KEY_F8);
        lua_settable(L, -3);
        lua_pushstring(L, "F9");
        lua_pushinteger(L, (int)Key::KEY_F9);
        lua_settable(L, -3);
        lua_pushstring(L, "F10");
        lua_pushinteger(L, (int)Key::KEY_F10);
        lua_settable(L, -3);
        lua_pushstring(L, "F11");
        lua_pushinteger(L, (int)Key::KEY_F11);
        lua_settable(L, -3);
        lua_pushstring(L, "F12");
        lua_pushinteger(L, (int)Key::KEY_F12);
        lua_settable(L, -3);

        lua_pushstring(L, "DELETE");
        lua_pushinteger(L, (int)Key::KEY_DELETE);
        lua_settable(L, -3);
        lua_pushstring(L, "INSERT");
        lua_pushinteger(L, (int)Key::KEY_INSERT);
        lua_settable(L, -3);
        lua_pushstring(L, "HOME");
        lua_pushinteger(L, (int)Key::KEY_HOME);
        lua_settable(L, -3);
        lua_pushstring(L, "END");
        lua_pushinteger(L, (int)Key::KEY_END);
        lua_settable(L, -3);
        lua_pushstring(L, "RIGHT");
        lua_pushinteger(L, (int)Key::KEY_RIGHT);
        lua_settable(L, -3);
        lua_pushstring(L, "LEFT");
        lua_pushinteger(L, (int)Key::KEY_LEFT);
        lua_settable(L, -3);
        lua_pushstring(L, "UP");
        lua_pushinteger(L, (int)Key::KEY_UP);
        lua_settable(L, -3);
        lua_pushstring(L, "DOWN");
        lua_pushinteger(L, (int)Key::KEY_DOWN);
        lua_settable(L, -3);

        lua_setglobal(L, "Key");

        lua_newtable(L);
        lua_pushstring(L, "IsKeyDown");
        lua_pushcfunction(L, lua_is_key_down);
        lua_settable(L, -3);

        lua_pushstring(L, "IsKeyUp");
        lua_pushcfunction(L, lua_is_key_up);
        lua_settable(L, -3);

        lua_pushstring(L, "WasKeyPressedThisFrame");
        lua_pushcfunction(L, lua_was_key_pressed_this_frame);
        lua_settable(L, -3);

        lua_pushstring(L, "GetMousePosition");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
                              Vector2f pos = Input::GetMousePosition();

                              lua_newtable(L);
                              lua_pushnumber(L, pos.x);
                              lua_setfield(L, -2, "x");
                              lua_pushnumber(L, pos.y);
                              lua_setfield(L, -2, "y");
                              return 1; // classdb_lua_wrap should take ownership
                          });
        lua_settable(L, -3);

        lua_pushstring(L, "IsLeftMouseButtonDown");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            lua_pushboolean(L, Input::IsLeftMouseButtonDown());
            return 1; });
        lua_settable(L, -3);

        lua_pushstring(L, "IsMiddleMouseButtonDown");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            lua_pushboolean(L, Input::IsMiddleMouseButtonDown());
            return 1; });
        lua_settable(L, -3);

        lua_pushstring(L, "IsRightMouseButtonDown");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            lua_pushboolean(L, Input::IsRightMouseButtonDown());
            return 1; });
        lua_settable(L, -3);

        lua_setglobal(L, "Input");

        lua_newtable(L);

        lua_pushstring(L, "AddLine");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            float x1 = get_float_arg(L, 1);
            float y1 = get_float_arg(L, 2);
            float x2 = get_float_arg(L, 3);
            float y2 = get_float_arg(L, 4);
            float r  = get_float_arg(L, 5);
            float g  = get_float_arg(L, 6);
            float b  = get_float_arg(L, 7);
            float thickness = get_float_arg(L, 8);
            int origin = static_cast<int>(lua_tointeger(L, 9)); // origin should still be int


            Radium::DebugRenderer::AddLine({x1, y1}, {x2, y2}, r, g, b, thickness, (Radium::Nodes::CoordinateOrigin)origin);

            return 0; });
        lua_settable(L, -3);

        lua_pushstring(L, "AddPoint");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            float x = get_float_arg(L, 1);
            float y = get_float_arg(L, 2);
            float r  = get_float_arg(L, 3);
            float g  = get_float_arg(L, 4);
            float b  = get_float_arg(L, 5);
            float radius = get_float_arg(L, 6);
            float resolution = get_float_arg(L, 7);
            int origin = static_cast<int>(lua_tointeger(L, 8)); // origin should still be int


            Radium::DebugRenderer::AddPoint({x, y}, r, g, b, radius, resolution, (Radium::Nodes::CoordinateOrigin)origin);

            return 0; });
        lua_settable(L, -3);

        lua_pushstring(L, "AddRect");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            float x = get_float_arg(L, 1);
            float y = get_float_arg(L, 2);
            float w = get_float_arg(L, 3);
            float h = get_float_arg(L, 4);
            float r = get_float_arg(L, 5);
            float g = get_float_arg(L, 6);
            float b = get_float_arg(L, 7);
            float thickness = get_float_arg(L, 8);
            int origin = static_cast<int>(lua_tointeger(L, 9));

            Radium::DebugRenderer::AddRect({x, y, w, h}, r, g, b, thickness, (Radium::Nodes::CoordinateOrigin)origin);

            return 0; });
        lua_settable(L, -3);

        lua_pushstring(L, "AddString");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            std::string str = get_string_arg(L, 1);
            float x  = get_float_arg(L, 2);
            float y  = get_float_arg(L, 3);
            float r  = get_float_arg(L, 4);
            float g  = get_float_arg(L, 5);
            float b  = get_float_arg(L, 6);
            float thickness = get_float_arg(L, 7);

            Radium::DebugRenderer::AddString(str, {x, y}, r, g, b, thickness);

            return 0; });
        lua_settable(L, -3);

        lua_setglobal(L, "Debug");

        lua_newtable(L);

        lua_pushstring(L, "GetPixelScale");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            lua_pushnumber(L, GetPixelScale());
            return 1; });
        lua_settable(L, -3);

        lua_pushstring(L, "GetWindowWidth");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            lua_pushnumber(L, Rune::windowWidth);
            return 1; });
        lua_settable(L, -3);

        lua_pushstring(L, "GetWindowHeight");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            lua_pushnumber(L, Rune::windowHeight);
            return 1; });
        lua_settable(L, -3);

        lua_pushstring(L, "ScreenToWorld");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
            float x = get_float_arg(L, 1);
            float y = get_float_arg(L, 2);

            lua_newtable(L);
            lua_pushnumber(L, x / GetPixelScale());
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, y / GetPixelScale());
            lua_setfield(L, -2, "y");

            return 1; });
        lua_settable(L, -3);

        lua_pushstring(L, "WorldTopLeftToWorldCenter");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
    float x = get_float_arg(L, 1);  // world X (from top-left origin)
    float y = get_float_arg(L, 2);  // world Y

    float scale = GetPixelScale();
    float w = Rune::windowWidth / scale;
    float h = Rune::windowHeight / scale;

    float centeredX = x - (w / 2.0f);
    float centeredY = -(y - (h / 2.0f));

    lua_newtable(L);
    lua_pushnumber(L, centeredX);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, centeredY);
    lua_setfield(L, -2, "y");

    return 1; });
        lua_settable(L, -3);

        lua_pushstring(L, "GetCamera");
        lua_pushcfunction(L, [](lua_State *L) -> int
                          {
    return classdb_lua_wrap(L, Radium::currentApplication->GetCamera()); });
        lua_settable(L, -3);

        lua_setglobal(L, "Util");

        for (auto constructor : Lua::luaConstructors)
        {
            lua_pushcfunction(L, constructor.second);
            lua_setglobal(L, constructor.first.c_str());
        }

        // Load and execute the Lua script
        try
        {
            std::string script = ReadFileToString(path);

            // Load the script
            int result = luaL_loadstring(L, script.c_str());
            if (result != LUA_OK)
            {
                spdlog::error("Failed to load Lua script: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
                return;
            }

            // Execute the script
            result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                spdlog::error("Failed to execute Lua script: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
                return;
            }

            spdlog::info("Lua script loaded successfully: {}", path);
        }
        catch (const std::exception &e)
        {
            spdlog::error("Lua script error: {}", e.what());
        }
    }

    LuaScript::~LuaScript()
    {
        if (L)
        {
            lua_close(L);
        }
    }

    void LuaScript::OnLoad()
    {
        if (stubbed || !L)
        {
            return;
        }

        // Get the onLoad function from Lua
        lua_getglobal(L, "onLoad");
        if (lua_isfunction(L, -1))
        {
            int result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                spdlog::error("Lua OnLoad error: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1); // Remove the non-function value
        }
    }

    void LuaScript::OnTick(float dt)
    {
        if (stubbed || !L)
        {
            return;
        }

        // Get the onTick function from Lua
        lua_getglobal(L, "onTick");
        if (lua_isfunction(L, -1))
        {
            int result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                spdlog::error("Lua OnTick error: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1); // Remove the non-function value
        }
    }

    void LuaScript::OnRender()
    {
        if (stubbed || !L)
        {
            return;
        }

        // Get the onRender function from Lua
        lua_getglobal(L, "onRender");
        if (lua_isfunction(L, -1))
        {
            int result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                spdlog::error("Lua OnRender error: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1); // Remove the non-function value
        }
    }

    void LuaScript::OnImgui()
    {
        if (stubbed || !L)
        {
            return;
        }

        // Get the onImgui function from Lua
        lua_getglobal(L, "onImgui");
        if (lua_isfunction(L, -1))
        {
            int result = lua_pcall(L, 0, 0, 0);
            if (result != LUA_OK)
            {
                spdlog::error("Lua OnImgui error: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1); // Remove the non-function value
        }
    }
}