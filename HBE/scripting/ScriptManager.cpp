
#include "ScriptManager.h"
#include "core/utility/Log.h"

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

void ScriptManager::readTable(std::string path) {
    std::string p;
    if (doScript(path.c_str())) {
        lua_getglobal(L, "Texture");
        if (lua_istable(L, TOP)) {
            lua_pushstring(L, "path");   //stack = Texture   ---> path,Texture
            lua_gettable(L, TOP - 1);    //stack = "path_to_texture" ,Texture
            p = lua_tostring(L, TOP);    //get the top stack value as string
            lua_pop(L, 1);   //stack =  "path_to_texture",Texture  ---> Texture
            Log::message(p);
        } else {
            //is not table
        }
    }
}

void f(std::string s) {
    Log::message(s);
}

void ScriptManager::init() {
    L = luaL_newstate();
    luaL_openlibs(L);
    doScript("../res/scripts/test.lua");
    callFunction("test_fun");
}

void ScriptManager::terminate() {
    lua_close(L);
}


void ScriptManager::callFunction(std::string function_name) {
    lua_getglobal(L, function_name.c_str());
    if (lua_isfunction(L, TOP)) {
        int return_count = 0;
        int args_count = 0;
        if(checkLua(lua_pcall(L, args_count, return_count, 0))) {

        }
    }
}

