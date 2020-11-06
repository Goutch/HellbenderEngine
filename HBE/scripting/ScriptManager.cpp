
#include "ScriptManager.h"
#include "core/utility/Log.h"
#include "assert.h"

#define TOP -1
#define BOTTOM 1

lua_State *ScriptManager::L = nullptr;

bool ScriptManager::checkLua(int code) {
    if (code != LUA_OK) {
        Log::warning(lua_tostring(L, TOP));
        return false;
    }
    return true;
}

bool ScriptManager::doScript(std::string path) {
    return checkLua(luaL_dofile(L, path.c_str()));
}

bool ScriptManager::doString(std::string command) {
    return checkLua(luaL_dostring(L, command.c_str()));
}

void print_stack(lua_State *L) {
    printf("LUA_STACK:\n");
    int top = lua_gettop(L);
    printf("%s\t%s\t%s\n", "index", "type", "value");
    for (int i = 1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L, i));
        switch (lua_type(L, i)) {
            case LUA_TNUMBER:
                printf("%g\n", lua_tonumber(L, i));
                break;
            case LUA_TSTRING:
                printf("%s\n", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
                break;
            case LUA_TNIL:
                printf("%s\n", "nil");
                break;
            default:
                printf("%p\n", lua_topointer(L, i));
                break;
        }
    }
    printf("END\n");
}

void ScriptManager::readTable(int table_index) {
    printf("{\n");

    if (lua_istable(L, table_index)) {
        lua_pushnil(L);

        int key_index = -2;
        int value_index = -1;
        while (lua_next(L, table_index) != 0) {
            print_stack(L);
            std::string key_type = luaL_typename(L, key_index);
            std::string value_type = luaL_typename(L, value_index);
            printf("%s = %s ", key_type.c_str(), value_type.c_str());
            switch (lua_type(L, value_index)) {
                case LUA_TNUMBER:
                    printf("%g\n", lua_tonumber(L, value_index));
                    break;
                case LUA_TSTRING:
                    printf("%s\n", lua_tostring(L, value_index));
                    break;
                case LUA_TBOOLEAN:
                    printf("%s\n", (lua_toboolean(L, value_index) ? "true" : "false"));
                    break;
                case LUA_TNIL:
                    printf("%s\n", "null");
                    break;
                case LUA_TTABLE:
                    readTable(value_index);
                    break;
                default:
                    printf("%s\n", lua_topointer(L, value_index));
                    break;
            }
            print_stack(L);
            lua_pop(L, -1);
            print_stack(L);
        }

    }
    printf("}\n");
}


void ScriptManager::init() {
    L = luaL_newstate();
    luaL_openlibs(L);
    doScript("../res/scripts/test.lua");
    lua_getglobal(L, "table_root");
    readTable(lua_gettop(L));
}

void ScriptManager::terminate() {
    lua_close(L);
}


void ScriptManager::callFunction(std::string function_name) {
    lua_getglobal(L, function_name.c_str());
    if (lua_isfunction(L, TOP)) {
        int return_count = 0;
        int args_count = 0;
        if (checkLua(lua_pcall(L, args_count, return_count, 0))) {

        }
    }
}

