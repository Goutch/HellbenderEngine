
#include "ScriptManager.h"
#include "core/utility/Log.h"
#define TOP -1
#define BOTTOM 1

#include "core/utility/Function.h"

lua_State* ScriptManager::L= nullptr;

int lua_f(lua_State *L)
{
    int number_args=lua_gettop(L);
    for (int i = 0; i < number_args; ++i) {

    }
    std::string s=lua_tostring(L,1);
    f(s);
    return 1;
}


bool checkScript(lua_State* L,std::string path){
    if(luaL_dofile(L,path.c_str())!=LUA_OK)
    {
        std::string error_msg=lua_tostring(L, TOP);
        Log::error(error_msg);
        return false;
    }
    return true;
}
/*void table()
{
    std::string p;
    if(checkScript(L,"../res/scripts/logo.lua"))
    {
        lua_getglobal(L,"Texture");
        if(lua_istable(L,TOP))
        {
            lua_pushstring(L,"path");   //stack = Texture   ---> path,Texture
            lua_gettable(L,TOP-1);    //stack = "path_to_texture" ,Texture
            p=lua_tostring(L,TOP);    //get the top stack value as string
            lua_pop(L,1);   //stack =  "path_to_texture",Texture  ---> Texture
            Log::message(p);
        } else{
            //is not table
        }
    }
}*/

void ScriptManager::init() {
    L = luaL_newstate();
    luaL_openlibs(L);
    registerFunction(&f,"h_f");
    if(checkScript(L,"../res/scripts/test.lua"))
    {

    }
}

void ScriptManager::terminate() {
    lua_close(L);
}

void ScriptManager::doCommand(std::string command) {
    int result = luaL_dostring(L, command.c_str());
    if (result == LUA_OK) {

    } else {
        std::string error = lua_tostring(L, -1);
        Log::error(error);
    }
}
